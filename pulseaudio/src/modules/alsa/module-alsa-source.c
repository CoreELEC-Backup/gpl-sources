/***
  This file is part of PulseAudio.

  Copyright 2004-2008 Lennart Poettering
  Copyright 2006 Pierre Ossman <ossman@cendio.se> for Cendio AB

  PulseAudio is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published
  by the Free Software Foundation; either version 2.1 of the License,
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

#include <stdio.h>

#include <alsa/asoundlib.h>

#ifdef HAVE_VALGRIND_MEMCHECK_H
#include <valgrind/memcheck.h>
#endif

#include <pulsecore/module.h>
#include <pulsecore/modargs.h>
#include <pulsecore/log.h>
#include <pulsecore/macro.h>

#include "alsa-util.h"
#include "alsa-source.h"

PA_MODULE_AUTHOR("Lennart Poettering");
PA_MODULE_DESCRIPTION("ALSA Source");
PA_MODULE_VERSION(PACKAGE_VERSION);
PA_MODULE_LOAD_ONCE(false);
PA_MODULE_USAGE(
        "name=<name for the source, to be prefixed> "
        "source_name=<name for the source> "
        "source_properties=<properties for the source> "
        "namereg_fail=<when false attempt to synthesise new source_name if it is already taken> "
        "device=<ALSA device> "
        "device_id=<ALSA card index> "
        "format=<sample format> "
        "rate=<sample rate> "
        "alternate_rate=<alternate sample rate> "
        "channels=<number of channels> "
        "channel_map=<channel map> "
        "fragments=<number of fragments> "
        "fragment_size=<fragment size> "
        "mmap=<enable memory mapping?> "
        "tsched=<enable system timer based scheduling mode?> "
        "tsched_buffer_size=<buffer size when using timer based scheduling> "
        "tsched_buffer_watermark=<upper fill watermark> "
        "ignore_dB=<ignore dB information from the device?> "
        "control=<name of mixer control>"
        "deferred_volume=<Synchronize software and hardware volume changes to avoid momentary jumps?> "
        "deferred_volume_safety_margin=<usec adjustment depending on volume direction> "
        "deferred_volume_extra_delay=<usec adjustment to HW volume changes> "
        "fixed_latency_range=<disable latency range changes on overrun?>");

static const char* const valid_modargs[] = {
    "name",
    "source_name",
    "source_properties",
    "namereg_fail",
    "device",
    "device_id",
    "format",
    "rate",
    "alternate_rate",
    "channels",
    "channel_map",
    "fragments",
    "fragment_size",
    "mmap",
    "tsched",
    "tsched_buffer_size",
    "tsched_buffer_watermark",
    "ignore_dB",
    "control",
    "deferred_volume",
    "deferred_volume_safety_margin",
    "deferred_volume_extra_delay",
    "fixed_latency_range",
    NULL
};

int pa__init(pa_module*m) {
    pa_modargs *ma = NULL;

    pa_assert(m);

    pa_alsa_refcnt_inc();

    if (!(ma = pa_modargs_new(m->argument, valid_modargs))) {
        pa_log("Failed to parse module arguments");
        goto fail;
    }

    if (!(m->userdata = pa_alsa_source_new(m, ma, __FILE__, NULL, NULL)))
        goto fail;

    pa_modargs_free(ma);

    return 0;

fail:

    if (ma)
        pa_modargs_free(ma);

    pa__done(m);

    return -1;
}

int pa__get_n_used(pa_module *m) {
    pa_source *source;

    pa_assert(m);
    pa_assert_se(source = m->userdata);

    return pa_source_linked_by(source);
}

void pa__done(pa_module*m) {
    pa_source *source;

    pa_assert(m);

    if ((source = m->userdata))
        pa_alsa_source_free(source);

    pa_alsa_refcnt_dec();
}
