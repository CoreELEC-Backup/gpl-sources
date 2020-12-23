/***************************************************************************

    hiscore.c

    Manages the hiscore system.

    Copyright (c) 1996-2007, Nicola Salmoria and the MAME Team.
    Visit http://mamedev.org for licensing and usage restrictions.

***************************************************************************/

#include "emu.h"
#include "emuopts.h"
#include "hiscore.h"
#include "deprecat.h"
#include "hiscore.h"
#include "../../precompile/hiscore_dat.h"

#include "retromain.h"

#define MAX_CONFIG_LINE_SIZE 48

static emu_timer *timer;

const char *db_filename = "hiscore.dat"; /* high score definition file */

char *parse_hiscoredat(char *s, int n, int *const index);


struct _memory_range
{
	UINT32 cpu, addr, num_bytes, start_value, end_value;
	struct _memory_range *next;
};
typedef struct _memory_range memory_range;


static struct
{
	int hiscores_have_been_loaded;
	memory_range *mem_range;
} state;


/*****************************************************************************/

static void copy_to_memory (running_machine *machine, int cpu, int addr, const UINT8 *source, int num_bytes)
{
	int i;
	const address_space *targetspace;
	if (strstr(machine->gamedrv->source_file,"cinemat.c") != NULL)
	{
		 targetspace = cpu_get_address_space(machine->cpu[cpu], ADDRESS_SPACE_DATA);
	}
	else
	{
		 targetspace = cpu_get_address_space(machine->cpu[cpu], ADDRESS_SPACE_PROGRAM);
	}

	for (i=0; i<num_bytes; i++)
	{
		memory_write_byte (targetspace, addr+i, source[i]);
	}
}

static void copy_from_memory (running_machine *machine, int cpu, int addr, UINT8 *dest, int num_bytes)
{
	int i;
	const address_space *targetspace;
	if (strstr(machine->gamedrv->source_file,"cinemat.c") != NULL)
	{
		 targetspace = cpu_get_address_space(machine->cpu[cpu], ADDRESS_SPACE_DATA);
	}
	else
	{
		 targetspace = cpu_get_address_space(machine->cpu[cpu], ADDRESS_SPACE_PROGRAM);
	}
	for (i=0; i<num_bytes; i++)
	{
		dest[i] = memory_read_byte (targetspace, addr+i);
	}
}

/*****************************************************************************/

/*  hexstr2num extracts and returns the value of a hexadecimal field from the
    character buffer pointed to by pString.

    When hexstr2num returns, *pString points to the character following
    the first non-hexadecimal digit, or NULL if an end-of-string marker
    (0x00) is encountered.

*/
static UINT32 hexstr2num (const char **pString)
{
	const char *string = *pString;
	UINT32 result = 0;
	if (string)
	{
		for(;;)
		{
			char c = *string++;
			int digit;

			if (c>='0' && c<='9')
			{
				digit = c-'0';
			}
			else if (c>='a' && c<='f')
			{
				digit = 10+c-'a';
			}
			else if (c>='A' && c<='F')
			{
				digit = 10+c-'A';
			}
			else
			{
				/* not a hexadecimal digit */
				/* safety check for premature EOL */
				if (!c) string = NULL;
				break;
			}
			result = result*16 + digit;
		}
		*pString = string;
	}
	return result;
}

/*  given a line in the hiscore.dat file, determine if it encodes a
    memory range (or a game name).
    For now we assume that CPU number is always a decimal digit, and
    that no game name starts with a decimal digit.
*/
static int is_mem_range (const char *pBuf)
{
	char c;
	for(;;)
	{
		c = *pBuf++;
		if (c == 0) return 0; /* premature EOL */
		if (c == ':') break;
	}
	c = *pBuf; /* character following first ':' */

	return	(c>='0' && c<='9') ||
			(c>='a' && c<='f') ||
			(c>='A' && c<='F');
}

/*  matching_game_name is used to skip over lines until we find <gamename>: */
static int matching_game_name (const char *pBuf, const char *name)
{
	while (*name)
	{
		if (*name++ != *pBuf++) return 0;
	}
	return (*pBuf == ':');
}

/*****************************************************************************/

/* safe_to_load checks the start and end values of each memory range */
static int safe_to_load (running_machine *machine)
{
	memory_range *mem_range = state.mem_range;
	const address_space *srcspace;
	if (strstr(machine->gamedrv->source_file,"cinemat.c") != NULL)
	{
		srcspace = cpu_get_address_space(machine->cpu[mem_range->cpu], ADDRESS_SPACE_DATA);
	}
	else
	{
		srcspace = cpu_get_address_space(machine->cpu[mem_range->cpu], ADDRESS_SPACE_PROGRAM);
	}
	while (mem_range)
	{
		if (memory_read_byte (srcspace, mem_range->addr) !=
			mem_range->start_value)
		{
			return 0;
		}
		if (memory_read_byte (srcspace, mem_range->addr + mem_range->num_bytes - 1) !=
			mem_range->end_value)
		{
			return 0;
		}
		mem_range = mem_range->next;
	}
	return 1;
}

/* hiscore_free disposes of the mem_range linked list */
static void hiscore_free (void)
{
	memory_range *mem_range = state.mem_range;
	while (mem_range)
	{
		memory_range *next = mem_range->next;
		free (mem_range);
		mem_range = next;
	}
	state.mem_range = NULL;
}

static void hiscore_load (running_machine *machine)
{
	file_error filerr;
 	mame_file *f;
	astring *fname;
    
    fname = astring_assemble_2(astring_alloc(), machine->basename(), ".hi");
    retro_log(RETRO_LOG_INFO, "[MAME 2010] attempting to load hiscore data from: %s\n", fname);    
    filerr = mame_fopen(hiscore_directory, astring_c(fname), OPEN_FLAG_READ, &f);
    astring_free(fname);
    state.hiscores_have_been_loaded = 1;
    if (filerr == FILERR_NONE)
    {
        memory_range *mem_range = state.mem_range;
        retro_log(RETRO_LOG_INFO, "[MAME 2010] hiscore datafile found.\n");
        while (mem_range)
        {
            UINT8 *data = global_alloc_array(UINT8, mem_range->num_bytes);
            if (data)
            {
                /*  this buffer will almost certainly be small
                    enough to be dynamically allocated, but let's
                    avoid memory trashing just in case
                */
                mame_fread (f, data, mem_range->num_bytes);
                copy_to_memory (machine,mem_range->cpu, mem_range->addr, data, mem_range->num_bytes);
                global_free (data);
            }
            mem_range = mem_range->next;
        }
        mame_fclose (f);
	}
}

static void hiscore_save (running_machine *machine)
{
    file_error filerr;
 	mame_file *f;
	astring *fname;
    fname = astring_assemble_2(astring_alloc(), machine->basename(), ".hi");
    retro_log(RETRO_LOG_INFO, "[MAME 2010] attempting to save hiscore hiscore datafile: %s\n", fname);
    filerr = mame_fopen(hiscore_directory, astring_c(fname), OPEN_FLAG_WRITE | OPEN_FLAG_CREATE | OPEN_FLAG_CREATE_PATHS, &f);
    astring_free(fname);
    
    if (filerr == FILERR_NONE)
    {
        memory_range *mem_range = state.mem_range;
        retro_log(RETRO_LOG_INFO, "[MAME 2010] saving hiscore datafile.\n");
        while (mem_range)
        {
            UINT8 *data = global_alloc_array(UINT8, mem_range->num_bytes);
            if (data)
            {
                /*  this buffer will almost certainly be small
                    enough to be dynamically allocated, but let's
                    avoid memory trashing just in case
                */
                copy_from_memory (machine, mem_range->cpu, mem_range->addr, data, mem_range->num_bytes);
                mame_fwrite(f, data, mem_range->num_bytes);
                global_free (data);
            }
            mem_range = mem_range->next;
        }
        mame_fclose(f);
    }
}


/* call hiscore_update periodically (i.e. once per frame) */
static TIMER_CALLBACK( hiscore_periodic )
{
	if (state.mem_range)
	{
		if (!state.hiscores_have_been_loaded)
		{
			if (safe_to_load(machine))
			{
				hiscore_load(machine);
				timer_enable(timer, FALSE);
			}
		}
	}
}


/* call hiscore_close when done playing game */
void hiscore_close (running_machine &machine)
{
	if (state.hiscores_have_been_loaded) hiscore_save(&machine);
	hiscore_free();
}


/*****************************************************************************/
/* public API */

/* call hiscore_open once after loading a game */
void hiscore_init (running_machine *machine)
{
	file_error filerr;
 	mame_file *f;
    
    memory_range *mem_range = state.mem_range;
    
    const char *name = machine->gamedrv->name;
    enum { FIND_NAME, FIND_DATA, FETCH_DATA } mode;    
    mode = FIND_NAME; 
	state.hiscores_have_been_loaded = 0;

	while (mem_range)
	{

		if (strstr(machine->gamedrv->source_file,"cinemat.c") != NULL)
		{
			memory_write_byte(
				cpu_get_address_space(machine->cpu[mem_range->cpu], ADDRESS_SPACE_DATA),
				mem_range->addr,
				~mem_range->start_value
			);
	
			memory_write_byte(
				cpu_get_address_space(machine->cpu[mem_range->cpu], ADDRESS_SPACE_DATA),
				mem_range->addr + mem_range->num_bytes-1,
				~mem_range->end_value
			);
			mem_range = mem_range->next;
		}
		else
		{
			memory_write_byte(
				cpu_get_address_space(machine->cpu[mem_range->cpu], ADDRESS_SPACE_PROGRAM),
				mem_range->addr,
				~mem_range->start_value
			);
	
			memory_write_byte(
				cpu_get_address_space(machine->cpu[mem_range->cpu], ADDRESS_SPACE_PROGRAM),
				mem_range->addr + mem_range->num_bytes-1,
				~mem_range->end_value
			);
			mem_range = mem_range->next;
		}
	}

	state.mem_range = NULL;
    
    if(use_external_hiscore)
    {
        
        retro_log(RETRO_LOG_INFO, "[MAME 2010] Trying to open %s hiscore data file from %s\n", db_filename, libretro_system_directory);   
        filerr = mame_fopen(libretro_system_directory, db_filename, OPEN_FLAG_READ, &f);
        if(filerr == FILERR_NONE)
        {
            retro_log(RETRO_LOG_INFO, "[MAME 2010] Succesfully opened %s\n", db_filename);
            
            char buffer[MAX_CONFIG_LINE_SIZE];

            while (mame_fgets (buffer, MAX_CONFIG_LINE_SIZE, f))
            {
                if (mode==FIND_NAME)
                {
                    if (matching_game_name (buffer, name))
                    {
                        mode = FIND_DATA;
                        retro_log(RETRO_LOG_INFO, "[MAME 2010] hiscore configuration found for %s\n", name);
                    }
                }
                else if (is_mem_range (buffer))
                {
                    const char *pBuf = buffer;
                    mem_range = (memory_range *)malloc(sizeof(memory_range));
                    if (mem_range)
                    {
                        mem_range->cpu = hexstr2num (&pBuf);
                        mem_range->addr = hexstr2num (&pBuf);
                        mem_range->num_bytes = hexstr2num (&pBuf);
                        mem_range->start_value = hexstr2num (&pBuf);
                        mem_range->end_value = hexstr2num (&pBuf);

                        mem_range->next = NULL;
                        {
                            memory_range *last = state.mem_range;
                            while (last && last->next) last = last->next;
                            if (last == NULL)
                            {
                                state.mem_range = mem_range;
                            }
                            else
                            {
                                last->next = mem_range;
                            }
                        }

                        mode = FETCH_DATA;
                    }
                    else
                    {
                        hiscore_free();
                        break;
                    }
                }
                else
                {
                    /* line is a game name */
                    if (mode == FETCH_DATA) break;
                }
            }
            mame_fclose (f);
        }
    }
    else /* use internal hiscore data */
    {
        int hiscoredat_index = 0;
        static char buffer[MAX_CONFIG_LINE_SIZE];
        
        retro_log(RETRO_LOG_INFO, "[MAME 2010] Searching for %s in internal hiscore data.\n", name);

        while(parse_hiscoredat(buffer, MAX_CONFIG_LINE_SIZE, &hiscoredat_index))
        {
            if (mode==FIND_NAME)
            {
                if (matching_game_name (buffer, name))
                {
                    mode = FIND_DATA;
                    retro_log(RETRO_LOG_INFO, "[MAME 2010] hiscore configuration found for %s\n", name);
                }
            }
               else if (is_mem_range (buffer))
                {
                    const char *pBuf = buffer;
                    mem_range = (memory_range *)malloc(sizeof(memory_range));
                    if (mem_range)
                    {
                        mem_range->cpu = hexstr2num (&pBuf);
                        mem_range->addr = hexstr2num (&pBuf);
                        mem_range->num_bytes = hexstr2num (&pBuf);
                        mem_range->start_value = hexstr2num (&pBuf);
                        mem_range->end_value = hexstr2num (&pBuf);

                        mem_range->next = NULL;
                        {
                            memory_range *last = state.mem_range;
                            while (last && last->next) last = last->next;
                            if (last == NULL)
                            {
                                state.mem_range = mem_range;
                            }
                            else
                            {
                                last->next = mem_range;
                            }
                        }

                        mode = FETCH_DATA;
                    }
                    else
                    {
                        hiscore_free();
                        break;
                    }
                }
                else
                {
                    /* line is a game name */
                    if (mode == FETCH_DATA) break;
                }
        }   
    }

    timer = timer_alloc(machine,hiscore_periodic, NULL);
    timer_adjust_periodic(timer, machine->primary_screen->frame_period(), 0, machine->primary_screen->frame_period());

    machine->add_notifier(MACHINE_NOTIFY_EXIT, hiscore_close);
    
}

char *parse_hiscoredat(char *s, int n, int *const index)
{
	char *cur = s;

	/* loop while we have characters */
	while (n > 0)
	{
        if (*index == hiscoredat_length)
            break;
        
		int c = hiscoredat[(*index)++];

		/* if there's a CR, look for an LF afterwards */
		if (c == 0x0d)
		{
			int c2 = hiscoredat[(*index)++];
			if (c2 != 0x0a)
				(*index)--;
			*cur++ = 0x0d;
			n--;
			break;
		}

		/* if there's an LF, reinterp as a CR for consistency */
		else if (c == 0x0a)
		{
			*cur++ = 0x0d;
			n--;
			break;
		}

		/* otherwise, pop the character in and continue */
		*cur++ = c;
		n--;
	}

	/* if we put nothing in, return NULL */
	if (cur == s)
		return NULL;

	/* otherwise, terminate */
	if (n > 0)
		*cur++ = 0;
	return s;
}
