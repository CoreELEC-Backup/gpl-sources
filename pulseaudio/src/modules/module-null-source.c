/***
  This file is part of PulseAudio.

  Copyright 2004-2008 Lennart Poettering
  Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).

  PulseAudio is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published
  by the Free Software Foundation; either version 2 of the License,
  or (at your option) any later version.

  PulseAudio is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with PulseAudio; if not, see <http://www.gnu.org/licenses/>.
***/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include <pulse/rtclock.h>
#include <pulse/timeval.h>
#include <pulse/util.h>
#include <pulse/xmalloc.h>

#include <pulsecore/core-util.h>
#include <pulsecore/log.h>
#include <pulsecore/macro.h>
#include <pulsecore/modargs.h>
#include <pulsecore/module.h>
#include <pulsecore/rtpoll.h>
#include <pulsecore/source.h>
#include <pulsecore/thread-mq.h>
#include <pulsecore/thread.h>

PA_MODULE_AUTHOR("Lennart Poettering & Marc-Andre Lureau");
PA_MODULE_DESCRIPTION("Clocked NULL source");
PA_MODULE_VERSION(PACKAGE_VERSION);
PA_MODULE_LOAD_ONCE(false);
PA_MODULE_USAGE(
        "format=<sample format> "
        "channels=<number of channels> "
        "rate=<sample rate> "
        "source_name=<name of source> "
        "channel_map=<channel map> "
        "description=<description for the source> ");

#define DEFAULT_SOURCE_NAME "source.null"
#define MAX_LATENCY_USEC (PA_USEC_PER_SEC * 2)
#define MIN_LATENCY_USEC (500)

struct userdata {
    pa_core *core;
    pa_module *module;
    pa_source *source;

    pa_thread *thread;
    pa_thread_mq thread_mq;
    pa_rtpoll *rtpoll;

    size_t block_size;

    pa_usec_t block_usec;
    pa_usec_t timestamp;
};

static const char* const valid_modargs[] = {
    "rate",
    "format",
    "channels",
    "source_name",
    "channel_map",
    "description",
    NULL
};

static int source_process_msg(pa_msgobject *o, int code, void *data, int64_t offset, pa_memchunk *chunk) {
    struct userdata *u = PA_SOURCE(o)->userdata;

    switch (code) {
        case PA_SOURCE_MESSAGE_GET_LATENCY: {
            pa_usec_t now;

            now = pa_rtclock_now();
            *((int64_t*) data) = (int64_t)now - (int64_t)u->timestamp;

            return 0;
        }
    }

    return pa_source_process_msg(o, code, data, offset, chunk);
}

/* Called from the IO thread. */
static int source_set_state_in_io_thread_cb(pa_source *s, pa_source_state_t new_state, pa_suspend_cause_t new_suspend_cause) {
    struct userdata *u;

    pa_assert(s);
    pa_assert_se(u = s->userdata);

    if (s->thread_info.state == PA_SOURCE_SUSPENDED || s->thread_info.state == PA_SOURCE_INIT) {
        if (PA_SOURCE_IS_OPENED(new_state))
            u->timestamp = pa_rtclock_now();
    }

    return 0;
}

static void source_update_requested_latency_cb(pa_source *s) {
    struct userdata *u;

    pa_source_assert_ref(s);
    u = s->userdata;
    pa_assert(u);

    u->block_usec = pa_source_get_requested_latency_within_thread(s);
    if (u->block_usec == (pa_usec_t)-1)
        u->block_usec = u->source->thread_info.max_latency;
}

static void thread_func(void *userdata) {
    struct userdata *u = userdata;
    bool timer_elapsed = false;
    size_t max_block_size;

    pa_assert(u);

    pa_log_debug("Thread starting up");

    if (u->core->realtime_scheduling)
        pa_thread_make_realtime(u->core->realtime_priority);

    pa_thread_mq_install(&u->thread_mq);

    max_block_size = pa_frame_align(pa_mempool_block_size_max(u->core->mempool), &u->source->sample_spec);
    u->timestamp = pa_rtclock_now();

    for (;;) {
        int ret;

        /* Generate some null data */
        if (PA_SOURCE_IS_OPENED(u->source->thread_info.state)) {
            pa_usec_t now;
            pa_memchunk chunk;

            now = pa_rtclock_now();

            if (timer_elapsed && (chunk.length = pa_usec_to_bytes(now - u->timestamp, &u->source->sample_spec)) > 0) {

                chunk.length = PA_MIN(max_block_size, chunk.length);

                chunk.memblock = pa_memblock_new(u->core->mempool, chunk.length);
                chunk.index = 0;
                pa_silence_memchunk(&chunk, &u->source->sample_spec);
                pa_source_post(u->source, &chunk);
                pa_memblock_unref(chunk.memblock);

                u->timestamp += pa_bytes_to_usec(chunk.length, &u->source->sample_spec);
            }

            pa_rtpoll_set_timer_absolute(u->rtpoll, u->timestamp + u->block_usec);
        } else
            pa_rtpoll_set_timer_disabled(u->rtpoll);

        /* Hmm, nothing to do. Let's sleep */
        if ((ret = pa_rtpoll_run(u->rtpoll)) < 0)
            goto fail;

        timer_elapsed = pa_rtpoll_timer_elapsed(u->rtpoll);

        if (ret == 0)
            goto finish;
    }

fail:
    /* If this was no regular exit from the loop we have to continue
     * processing messages until we received PA_MESSAGE_SHUTDOWN */
    pa_asyncmsgq_post(u->thread_mq.outq, PA_MSGOBJECT(u->core), PA_CORE_MESSAGE_UNLOAD_MODULE, u->module, 0, NULL, NULL);
    pa_asyncmsgq_wait_for(u->thread_mq.inq, PA_MESSAGE_SHUTDOWN);

finish:
    pa_log_debug("Thread shutting down");
}

int pa__init(pa_module*m) {
    struct userdata *u = NULL;
    pa_sample_spec ss;
    pa_channel_map map;
    pa_modargs *ma = NULL;
    pa_source_new_data data;

    pa_assert(m);

    if (!(ma = pa_modargs_new(m->argument, valid_modargs))) {
        pa_log("Failed to parse module arguments.");
        goto fail;
    }

    ss = m->core->default_sample_spec;
    map = m->core->default_channel_map;
    if (pa_modargs_get_sample_spec_and_channel_map(ma, &ss, &map, PA_CHANNEL_MAP_DEFAULT) < 0) {
        pa_log("Invalid sample format specification or channel map");
        goto fail;
    }

    m->userdata = u = pa_xnew0(struct userdata, 1);
    u->core = m->core;
    u->module = m;
    u->rtpoll = pa_rtpoll_new();

    if (pa_thread_mq_init(&u->thread_mq, m->core->mainloop, u->rtpoll) < 0) {
        pa_log("pa_thread_mq_init() failed.");
        goto fail;
    }

    pa_source_new_data_init(&data);
    data.driver = __FILE__;
    data.module = m;
    pa_source_new_data_set_name(&data, pa_modargs_get_value(ma, "source_name", DEFAULT_SOURCE_NAME));
    pa_source_new_data_set_sample_spec(&data, &ss);
    pa_source_new_data_set_channel_map(&data, &map);
    pa_proplist_sets(data.proplist, PA_PROP_DEVICE_DESCRIPTION, pa_modargs_get_value(ma, "description", "Null Input"));
    pa_proplist_sets(data.proplist, PA_PROP_DEVICE_CLASS, "abstract");

    u->source = pa_source_new(m->core, &data, PA_SOURCE_LATENCY | PA_SOURCE_DYNAMIC_LATENCY);
    pa_source_new_data_done(&data);

    if (!u->source) {
        pa_log("Failed to create source object.");
        goto fail;
    }

    u->source->parent.process_msg = source_process_msg;
    u->source->set_state_in_io_thread = source_set_state_in_io_thread_cb;
    u->source->update_requested_latency = source_update_requested_latency_cb;
    u->source->userdata = u;

    pa_source_set_asyncmsgq(u->source, u->thread_mq.inq);
    pa_source_set_rtpoll(u->source, u->rtpoll);

    pa_source_set_latency_range(u->source, MIN_LATENCY_USEC, MAX_LATENCY_USEC);
    u->block_usec = u->source->thread_info.max_latency;

    u->source->thread_info.max_rewind =
        pa_usec_to_bytes(u->block_usec, &u->source->sample_spec);

    if (!(u->thread = pa_thread_new("null-source", thread_func, u))) {
        pa_log("Failed to create thread.");
        goto fail;
    }

    pa_source_put(u->source);

    pa_modargs_free(ma);

    return 0;

fail:
    if (ma)
        pa_modargs_free(ma);

    pa__done(m);

    return -1;
}

void pa__done(pa_module*m) {
    struct userdata *u;

    pa_assert(m);

    if (!(u = m->userdata))
        return;

    if (u->source)
        pa_source_unlink(u->source);

    if (u->thread) {
        pa_asyncmsgq_send(u->thread_mq.inq, NULL, PA_MESSAGE_SHUTDOWN, NULL, 0, NULL);
        pa_thread_free(u->thread);
    }

    pa_thread_mq_done(&u->thread_mq);

    if (u->source)
        pa_source_unref(u->source);

    if (u->rtpoll)
        pa_rtpoll_free(u->rtpoll);

    pa_xfree(u);
}
