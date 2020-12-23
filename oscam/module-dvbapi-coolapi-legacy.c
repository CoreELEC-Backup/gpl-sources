/* Reversed from libcoolstream.so, this comes without any warranty */

#define MODULE_LOG_PREFIX "dvbcool"

#include "globals.h"

#if defined(HAVE_DVBAPI) && defined(WITH_COOLAPI)

#include "extapi/coolapi.h"

#include "module-dvbapi.h"
#include "module-dvbapi-coolapi.h"
#include "oscam-string.h"

#define MAX_COOL_DMX 4

typedef struct s_cool_filter
{
	int32_t     fd;
	int32_t     channel;
	int32_t     pid;
	uint8_t     filter16[16];
	uint8_t     mask16[16];
} S_COOL_FILTER;

typedef struct s_cool_chanhandle
{
	int32_t     pid;
	void        *buffer1;
	void        *buffer2;
	void        *channel;
	int32_t     demux_index;
} S_COOL_CHANHANDLE;

struct cool_dmx
{
	int32_t     opened;
	int32_t     fd;
	uint8_t     buffer[4096];
	void        *buffer1;
	void        *buffer2;
	void        *channel;
	void        *filter;
	void        *device;
	int32_t     pid;
	pthread_mutex_t mutex;
	int32_t     demux_index;
	int32_t     demux_id;
	int32_t     filter_num;
	int32_t     type;
};
typedef struct cool_dmx dmx_t;

typedef struct
{
	int32_t type;
	uint32_t size;
	int32_t unknown1;
	int16_t unknown2;
	int32_t unknown3;
	int32_t unknown[4];
} buffer_open_arg_t;

typedef struct
{
	int32_t type;
	int32_t unknown[2];
} channel_open_arg_t;

typedef struct
{
	uint32_t number;
	int32_t unknown1;
	int32_t unknown2;
	int32_t unknown3;
	int32_t unknown4;
	int32_t unknown5;
	int32_t unknown6;
	int32_t unknown[6];
} device_open_arg_t;

typedef struct
{
	uint32_t    length;
	uint8_t     filter[18]; //strange: initialization with max 18 possible but length limited to 12
	uint8_t     mask[18];
	uint8_t     nmask[18];
	int8_t      fvernum;
	int8_t      crcchange;
	int8_t      keeprbytes;
	int32_t     mode;
} filter_set_t;


typedef enum
{
	CONTINUOUS_ACQUIRE   = 0,
	ONE_SHOT_ACQUIRE,
	TOGGLE_ACQUIRE
} DATA_ACQUIRE_MODE;

typedef enum
{
	DATA_ACQUIRED = 1,
	CHANNEL_TIMEOUT,
	CRC_ERROR,
	BUF_OVERFLOW,
	PES_ERROR,
	COPY_DONE,
	CHANNEL_INFO
} DATA_ACQUIRE_STATUS;

typedef struct
{
	uint32_t PTSLow;
	uint32_t PTSHi;
} DMX_PTS;

typedef struct
{
	void                *channel;
	DATA_ACQUIRE_STATUS  type;
	DMX_PTS              ptssnapshop;
	DATA_ACQUIRE_MODE    mode;
	void                *buf;
	uint32_t             len;
	uint32_t             num;
	void                *filters[64];
	void                *tags[64];
} dmx_callback_data_t;

/* These functions are implemented in libnxp */
extern int32_t cnxt_cbuf_open(void **handle, buffer_open_arg_t *arg, void *, void *);
extern int32_t cnxt_dmx_open(void **device, device_open_arg_t *arg, void *, void *);
extern int32_t cnxt_dmx_channel_open(void *device, void **channel, channel_open_arg_t *arg, void *callback, void *);
extern int32_t cnxt_dmx_set_filter(void *handle, filter_set_t *arg, void *);
extern int32_t cnxt_dmx_channel_suspend(void *handle, int32_t enable);

/* Local coolapi functions */
static void coolapi_open(void);
static int32_t coolapi_read(dmx_t *dmx, dmx_callback_data_t *data);

static int8_t dmx_opened;
int8_t cool_kal_opened = 0;

static void *dmx_device[MAX_COOL_DMX];
static dmx_t cdemuxes[MAX_COOL_DMX][MAX_FILTER];

extern void *dvbapi_client;

static LLIST   *ll_cool_filter     = NULL;
static LLIST   *ll_cool_chanhandle = NULL;

#define COOLDEMUX_FD(device, num) (('O' << 24) | ('S' << 16) | (device << 8) | num)
#define COOLDEMUX_DMX_DEV(fd) (((fd) >> 8) & 0xFF)

static dmx_t *find_demux(int32_t fd, int32_t dmx_dev_num)
{
	if(dmx_dev_num < 0 || dmx_dev_num >= MAX_COOL_DMX)
	{
		cs_log("Invalid demux %d", dmx_dev_num);
		return NULL;
	}

	int32_t i, idx;

	idx = dmx_dev_num;
	if(fd == 0)
	{
		for(i = 0; i < MAX_FILTER; i++)
		{
			if(!cdemuxes[idx][i].opened)
			{
				cdemuxes[idx][i].fd = COOLDEMUX_FD(dmx_dev_num, i);
				cs_log_dbg(D_DVBAPI, "opening new fd: %08x", cdemuxes[idx][i].fd);
				cdemuxes[idx][i].demux_index = dmx_dev_num;
				return &cdemuxes[idx][i];
			}
		}
		cs_log_dbg(D_DVBAPI, "ERROR: no free demux found");
		return NULL;
	}

	idx = COOLDEMUX_DMX_DEV(fd);
	for(i = 0; i < MAX_FILTER; i++)
	{
		if(cdemuxes[idx][i].fd == fd)
			{ return &cdemuxes[idx][i]; }
	}

	cs_log_dbg(D_DVBAPI, "ERROR: CANT FIND Demux %08x", fd);

	return NULL;
}

static void coolapi_read_data(dmx_t *dmx, dmx_callback_data_t *data)
{
	if(!dmx)
	{
		cs_log_dbg(D_DVBAPI, "handle is NULL!");
		return;
	}

	int32_t ret;

	SAFE_SETSPECIFIC(getclient, dvbapi_client);
	SAFE_MUTEX_LOCK(&dmx->mutex);
	memset(dmx->buffer, 0, 4096);
	ret = coolapi_read(dmx, data);
	SAFE_MUTEX_UNLOCK(&dmx->mutex);
	if(ret > -1)
		{ dvbapi_process_input(dmx->demux_id, dmx->filter_num, dmx->buffer, data->len, 0); }
}

static void dmx_callback(void *UNUSED(unk), dmx_t *dmx, int32_t type, dmx_callback_data_t *data)
{
	if(!dmx)
	{
		cs_log_dbg(D_DVBAPI, "wrong dmx pointer !!!");
		return;
	}

	if(data != NULL)
	{
		switch(type)
		{
		case 0xE:
			if(data->type == 1 && data->len > 0)
			{
				coolapi_read_data(dmx, data);
			}
			else
				{ cs_log_dbg(D_DVBAPI, "unknown callback data %d len %d", data->type, data->len); }
			break;
		default:
			break;

		}
	}
}

int32_t coolapi_set_filter(int32_t fd, int32_t num, int32_t pid, uint8_t *flt, uint8_t *mask, int32_t type)
{
	dmx_t *dmx =  find_demux(fd, 0);
	if(!dmx)
	{
		cs_log_dbg(D_DVBAPI, "dmx is NULL!");
		return -1;
	}

	int32_t result, channel_found = 0;

	void *channel = NULL;

	if(ll_count(ll_cool_chanhandle) > 0)
	{
		LL_ITER itr = ll_iter_create(ll_cool_chanhandle);
		S_COOL_CHANHANDLE *handle_item;
		while((handle_item = ll_iter_next(&itr)))
		{
			if(handle_item->demux_index == dmx->demux_index && handle_item->pid == pid)
			{
				channel = handle_item->channel;
				channel_found = 1;
				break;
			}
		}
	}

	if(!channel)
	{
		buffer_open_arg_t bufarg;
		int32_t uBufferSize = 8256;
		memset(&bufarg, 0, sizeof(bufarg));

		bufarg.type = 3;
		bufarg.size = uBufferSize;
		bufarg.unknown3 = (uBufferSize * 7) / 8;

		result = cnxt_cbuf_open(&dmx->buffer1, &bufarg, NULL, NULL);
		coolapi_check_error("cnxt_cbuf_open", result);

		bufarg.type = 0;

		result = cnxt_cbuf_open(&dmx->buffer2, &bufarg, NULL, NULL);
		coolapi_check_error("cnxt_cbuf_open", result);

		channel_open_arg_t chanarg;
		memset(&chanarg, 0, sizeof(channel_open_arg_t));
		chanarg.type = 4;

		result = cnxt_dmx_channel_open(dmx->device, &dmx->channel, &chanarg, dmx_callback, dmx);
		coolapi_check_error("cnxt_dmx_channel_open", result);

		result = cnxt_dmx_set_channel_buffer(dmx->channel, 0, dmx->buffer1);
		coolapi_check_error("cnxt_dmx_set_channel_buffer", result);

		result = cnxt_dmx_channel_attach(dmx->channel, 0xB, 0, dmx->buffer2);
		coolapi_check_error("cnxt_dmx_channel_attach", result);

		result = cnxt_cbuf_attach(dmx->buffer2, 2, dmx->channel);
		coolapi_check_error("cnxt_cbuf_attach", result);

		result = cnxt_dmx_set_channel_pid(dmx->channel, pid);
		coolapi_check_error("cnxt_dmx_set_channel_pid", result);

		result = cnxt_cbuf_flush(dmx->buffer1, 0);
		coolapi_check_error("cnxt_cbuf_flush", result);
		result = cnxt_cbuf_flush(dmx->buffer2, 0);
		coolapi_check_error("cnxt_cbuf_flush", result);

		S_COOL_CHANHANDLE *handle_item;
		if(cs_malloc(&handle_item, sizeof(S_COOL_CHANHANDLE)))
		{
			handle_item->pid            = pid;
			handle_item->channel        = dmx->channel;
			handle_item->buffer1        = dmx->buffer1;
			handle_item->buffer2        = dmx->buffer2;
			handle_item->demux_index    = dmx->demux_index;
			ll_append(ll_cool_chanhandle, handle_item);
		}
		cs_log_dbg(D_DVBAPI, "opened new channel %x", (int32_t) dmx->channel);
	}
	else
	{
		channel_found = 1;
		dmx->channel = channel;
		dmx->buffer1 = NULL;
		dmx->buffer2 = NULL;
	}

	cs_log_dbg(D_DVBAPI, "setting new filter fd=%08x demux=%d channel=%x num=%d pid=%04x flt=%x mask=%x", fd, dmx->demux_index, (int32_t) dmx->channel, num, pid, flt[0], mask[0]);

	SAFE_MUTEX_LOCK(&dmx->mutex);

	filter_set_t filter;
	dmx->filter_num = num;
	dmx->pid = pid;
	dmx->type = type;

	memset(&filter, 0, sizeof(filter));
	filter.length = 12;
	memcpy(filter.filter, flt, 16);
	memcpy(filter.mask, mask, 16);

	result = cnxt_dmx_open_filter(dmx->device, &dmx->filter);
	coolapi_check_error("cnxt_dmx_open_filter", result);

	result = cnxt_dmx_set_filter(dmx->filter, &filter, NULL);
	coolapi_check_error("cnxt_dmx_set_filter", result);

	result = cnxt_dmx_channel_suspend(dmx->channel, 1);
	coolapi_check_error("cnxt_dmx_channel_suspend", result);
	result = cnxt_dmx_channel_attach_filter(dmx->channel, dmx->filter);
	coolapi_check_error("cnxt_dmx_channel_attach_filter", result);
	result = cnxt_dmx_channel_suspend(dmx->channel, 0);
	coolapi_check_error("cnxt_dmx_channel_suspend", result);

	if(channel_found)
	{
		result = cnxt_dmx_channel_ctrl(dmx->channel, 0, 0);
		coolapi_check_error("cnxt_dmx_channel_ctrl", result);
	}

	result = cnxt_dmx_channel_ctrl(dmx->channel, 2, 0);
	coolapi_check_error("cnxt_dmx_channel_ctrl", result);

	SAFE_MUTEX_UNLOCK(&dmx->mutex);

	S_COOL_FILTER *filter_item;
	if(cs_malloc(&filter_item, sizeof(S_COOL_FILTER)))
	{
		// fill filter item
		filter_item->fd = fd;
		filter_item->pid = pid;
		filter_item->channel = (int32_t) dmx->channel;
		memcpy(filter_item->filter16, flt, 16);
		memcpy(filter_item->mask16, mask, 16);

		//add filter item
		ll_append(ll_cool_filter, filter_item);
	}
	return 0;
}

int32_t coolapi_remove_filter(int32_t fd, int32_t num)
{
	dmx_t *dmx = find_demux(fd, 0);
	if(!dmx)
	{
		cs_log_dbg(D_DVBAPI, "dmx is NULL!");
		return -1;
	}

	if(dmx->pid <= 0)
		{ return -1; }

	int32_t result, filter_on_channel = 0;

	cs_log_dbg(D_DVBAPI, "removing filter fd=%08x num=%d pid=%04x on channel=%x", fd, num, dmx->pid, (int32_t) dmx->channel);

	SAFE_MUTEX_LOCK(&dmx->mutex);

	if(dmx->filter)
	{
		result = cnxt_dmx_channel_suspend(dmx->channel, 1);
		coolapi_check_error("cnxt_dmx_channel_suspend", result);
		result = cnxt_dmx_channel_detach_filter(dmx->channel, dmx->filter);
		coolapi_check_error("cnxt_dmx_channel_detach_filter", result);
		result = cnxt_dmx_channel_suspend(dmx->channel, 0);
		coolapi_check_error("cnxt_dmx_channel_suspend", result);
		result = cnxt_dmx_close_filter(dmx->filter);
		coolapi_check_error("cnxt_dmx_close_filter", result);
		dmx->filter = NULL;
		result = cnxt_dmx_channel_ctrl(dmx->channel, 0, 0);
		coolapi_check_error("cnxt_dmx_channel_ctrl", result);
	}

	LL_ITER itr = ll_iter_create(ll_cool_filter);
	S_COOL_FILTER *filter_item;
	while((filter_item = ll_iter_next(&itr)))
	{
		if(filter_item->channel == (int32_t) dmx->channel)
			{ filter_on_channel++; }
		if(filter_item->fd == fd)
		{
			ll_iter_remove_data(&itr);
			filter_on_channel--;
		}
	}

	if(!filter_on_channel)
	{
		cs_log_dbg(D_DVBAPI, "closing channel %x", (int32_t) dmx->channel);

		itr = ll_iter_create(ll_cool_chanhandle);
		S_COOL_CHANHANDLE *handle_item;
		while((handle_item = ll_iter_next(&itr)))
		{
			if(handle_item->demux_index == dmx->demux_index && handle_item->pid == dmx->pid)
			{
				dmx->buffer1 = handle_item->buffer1;
				dmx->buffer2 = handle_item->buffer2;
				ll_iter_remove_data(&itr);
				break;
			}
		}

		if(!dmx->buffer1 || !dmx->buffer2)
			{ cs_log_dbg(D_DVBAPI, "WARNING: buffer handle not found!"); }

		result = cnxt_dmx_channel_ctrl(dmx->channel, 0, 0);
		coolapi_check_error("cnxt_dmx_channel_ctrl", result);

		result = cnxt_dmx_set_channel_pid(dmx->channel, 0x1FFF);
		coolapi_check_error("cnxt_dmx_set_channel_pid", result);

		result = cnxt_cbuf_flush(dmx->buffer1, 0);
		coolapi_check_error("cnxt_cbuf_flush", result);

		result = cnxt_cbuf_flush(dmx->buffer2, 0);
		coolapi_check_error("cnxt_cbuf_flush", result);

		result = cnxt_cbuf_detach(dmx->buffer2, 2, dmx->channel);
		coolapi_check_error("cnxt_cbuf_detach", result);
		result = cnxt_dmx_channel_detach(dmx->channel, 0xB, 0, dmx->buffer1);
		coolapi_check_error("cnxt_dmx_channel_detach", result);

		result = cnxt_dmx_channel_close(dmx->channel);
		coolapi_check_error("cnxt_dmx_channel_close", result);

		result = cnxt_cbuf_close(dmx->buffer2);
		coolapi_check_error("cnxt_cbuf_close", result);

		result = cnxt_cbuf_close(dmx->buffer1);
		coolapi_check_error("cnxt_cbuf_close", result);
	}

	if(filter_on_channel)
	{
		result = cnxt_dmx_channel_ctrl(dmx->channel, 2, 0);
		coolapi_check_error("cnxt_dmx_channel_ctrl", result);
	}

	SAFE_MUTEX_UNLOCK(&dmx->mutex);

	dmx->pid = -1;
	return 0;
}

int32_t coolapi_open_device(int32_t demux_index, int32_t demux_id)
{
	dmx_t *dmx;

	coolapi_open();

	dmx = find_demux(0, demux_index);
	if(!dmx)
	{
		cs_log("no free demux found");
		return 0;
	}

	if(!ll_cool_filter)
		{ ll_cool_filter = ll_create("ll_cool_filter"); }

	if(!ll_cool_chanhandle)
		{ ll_cool_chanhandle = ll_create("ll_cool_chanhandle"); }

	dmx->demux_index = demux_index;
	dmx->demux_id = demux_id;
	dmx->pid = -1;

	dmx->device = dmx_device[demux_index];
	dmx->opened = 1;

	pthread_mutexattr_t attr;
	SAFE_MUTEXATTR_INIT(&attr);
	SAFE_MUTEXATTR_SETTYPE(&attr, PTHREAD_MUTEX_ERRORCHECK_NP);
	SAFE_MUTEX_INIT(&dmx->mutex, &attr);

	return dmx->fd;
}

int32_t coolapi_close_device(int32_t fd)
{
	dmx_t *dmx = find_demux(fd, 0);
	if(!dmx)
	{
		cs_log_dbg(D_DVBAPI, "dmx is NULL!");
		return -1;
	}

	cs_log_dbg(D_DVBAPI, "closing fd=%08x", fd);
	dmx->opened = 0;
	pthread_mutex_destroy(&dmx->mutex);

	memset(dmx, 0, sizeof(dmx_t));
	return 0;
}

/* write cw to all demuxes in mask with passed index */
int32_t coolapi_write_cw(int32_t mask, uint16_t *STREAMpids, int32_t count, ca_descr_t *ca_descr)
{
	int32_t i, idx = ca_descr->index;
	int32_t result;
	void *channel;

	cs_log_dbg(D_DVBAPI, "cw%d: mask %d index %d pid count %d", ca_descr->parity, mask, idx, count);
	for(i = 0; i < count; i++)
	{
		int32_t pid = STREAMpids[i];
		int32_t j;
		for(j = 0; j < MAX_COOL_DMX; j++)
		{
			if(mask & (1 << j))
			{
				result = cnxt_dmx_get_channel_from_pid(dmx_device[j], pid, &channel);
				if(result == 0)
				{
					cs_log_dbg(D_DVBAPI, "Found demux %d channel %x for pid %04x", j, (int32_t) channel, pid);
					result = cnxt_dmx_set_channel_key(channel, 0, ca_descr->parity, ca_descr->cw, 8);
					coolapi_check_error("cnxt_dmx_set_channel_key", result);
					if(result != 0)
					{
						cs_log("set_channel_key failed for demux %d pid %04x", j, pid);
					}
				}
			}
		}
	}
	return 0;
}

static int32_t coolapi_read(dmx_t *dmx, dmx_callback_data_t *data)
{
	if(!dmx)
	{
		cs_log_dbg(D_DVBAPI, "dmx is NULL!");
		return -1;
	}

	int32_t result;
	uint32_t done = 0, toread, len = data->len;
	uint8_t *buff = &dmx->buffer[0];
	uint32_t bytes_used = 0;

	//cs_log_dbg(D_DVBAPI, "dmx channel %x pid %x len %d",  (int) dmx->channel, dmx->pid, len);

	result = cnxt_cbuf_get_used(data->buf, &bytes_used);
	coolapi_check_error("cnxt_cbuf_get_used", result);
	if(bytes_used == 0)
		{ return -1; }

	result = cnxt_cbuf_read_data(data->buf, buff, 3, &done);
	coolapi_check_error("cnxt_cbuf_read_data", result);

	if(done != 3)
		{ return -1; }

	toread = ((buff[1] << 8) | buff[2]) & 0xFFF;
	if((toread + 3) > len)
		{ return -1; }
	result = cnxt_cbuf_read_data(data->buf, buff + 3, toread, &done);
	coolapi_check_error("cnxt_cbuf_read_data", result);
	if(done != toread)
		{ return -1; }
	done += 3;

	//cs_log_dbg(D_DVBAPI, "bytes read %d\n", done);

	return 0;
}

void coolapi_open_all(void)
{
	cnxt_kal_initialize();
	cnxt_drv_init();
	cnxt_smc_init(NULL);
	cool_kal_opened = 1;
}

static void coolapi_open(void)
{
	int32_t result = 0;
	device_open_arg_t devarg;

	if(!dmx_opened)
	{
		int32_t i;

		cs_log_dbg(D_DVBAPI, "Open Coolstream DMX API");
		cnxt_cbuf_init(NULL);
		cnxt_dmx_init(NULL);

		memset(&devarg, 0, sizeof(device_open_arg_t));

		devarg.unknown1 = 1;
		devarg.unknown3 = 3;
		devarg.unknown6 = 1;
		for(i = 0; i < MAX_COOL_DMX; i++)
		{
			devarg.number = i;
			result = cnxt_dmx_open(&dmx_device[i], &devarg, NULL, NULL);
			coolapi_check_error("cnxt_dmx_open", result);
		}
		dmx_opened = 1;
	}
}

void coolapi_close_all(void)
{
	if(dmx_opened)
	{
		int32_t result;
		int32_t i, j;

		for(i = 0; i < MAX_COOL_DMX; i++)
		{
			for(j = 0; j < MAX_FILTER; j++)
			{
				if(cdemuxes[i][j].fd > 0)
				{
					coolapi_remove_filter(cdemuxes[i][j].fd, cdemuxes[i][j].filter_num);
					coolapi_close_device(cdemuxes[i][j].fd);
				}
			}
		}
		for(i = 0; i < MAX_COOL_DMX; i++)
		{
			result = cnxt_dmx_close(dmx_device[i]);
			coolapi_check_error("cnxt_dmx_close", result);
			dmx_device[i] = NULL;
		}
	}
	cool_kal_opened = 0;
	cnxt_kal_terminate();
	cnxt_drv_term();
}
#elif defined(HAVE_DVBAPI) && defined(WITH_SU980)
#include "extapi/coolapi.h"
void cnxt_css_drv_init(void);
void cnxt_css_drv_term(void);
void cnxt_smc_term(void);

int32_t  cool_kal_opened = 0;
void coolapi_open_all(void)
{
	cnxt_kal_initialize();
	cnxt_css_drv_init();
	cnxt_smc_init(0);
	cool_kal_opened = 1;
}

void coolapi_close_all(void)
{
	cnxt_kal_terminate();
	cool_kal_opened = 0;
}
#endif

