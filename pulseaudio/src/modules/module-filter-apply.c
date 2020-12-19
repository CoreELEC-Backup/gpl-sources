/***
  This file is part of PulseAudio.

  Copyright 2011 Colin Guthrie

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

#include <pulse/timeval.h>
#include <pulse/rtclock.h>
#include <pulse/xmalloc.h>

#include <pulsecore/core.h>
#include <pulsecore/core-util.h>
#include <pulsecore/i18n.h>
#include <pulsecore/macro.h>
#include <pulsecore/hashmap.h>
#include <pulsecore/hook-list.h>
#include <pulsecore/sink-input.h>
#include <pulsecore/modargs.h>
#include <pulsecore/proplist-util.h>

#define PA_PROP_FILTER_APPLY_PARAMETERS PA_PROP_FILTER_APPLY".%s.parameters"
#define PA_PROP_FILTER_APPLY_MOVING     "filter.apply.moving"
#define PA_PROP_FILTER_APPLY_SET_BY_MFA "filter.apply.set_by_mfa"
#define PA_PROP_MDM_AUTO_FILTERED       "module-device-manager.auto_filtered"

PA_MODULE_AUTHOR("Colin Guthrie");
PA_MODULE_DESCRIPTION("Load filter sinks automatically when needed");
PA_MODULE_VERSION(PACKAGE_VERSION);
PA_MODULE_LOAD_ONCE(true);
PA_MODULE_USAGE(_("autoclean=<automatically unload unused filters?>"));

static const char* const valid_modargs[] = {
    "autoclean",
    NULL
};

#define DEFAULT_AUTOCLEAN true
#define HOUSEKEEPING_INTERVAL (10 * PA_USEC_PER_SEC)

struct filter {
    char *name;
    char *parameters;
    uint32_t module_index;
    pa_sink *sink;
    pa_sink *sink_master;
    pa_source *source;
    pa_source *source_master;
};

struct userdata {
    pa_core *core;
    pa_hashmap *filters;
    /* Keep track of streams we're managing PA_PROP_MDM_AUTO_FILTERED on, we're
     * only maintaining membership, so key and value are just the
     * pa_sink_input/pa_source_output. */
    pa_hashmap *mdm_ignored_inputs, *mdm_ignored_outputs;
    bool autoclean;
    pa_time_event *housekeeping_time_event;
};

static unsigned filter_hash(const void *p) {
    const struct filter *f = p;

    if (f->sink_master && !f->source_master)
        return (unsigned) (f->sink_master->index + pa_idxset_string_hash_func(f->name));
    else if (!f->sink_master && f->source_master)
        return (unsigned) ((f->source_master->index << 16) + pa_idxset_string_hash_func(f->name));
    else
        return (unsigned) (f->sink_master->index + (f->source_master->index << 16) + pa_idxset_string_hash_func(f->name));
}

static int filter_compare(const void *a, const void *b) {
    const struct filter *fa = a, *fb = b;
    int r;

    if (fa->sink_master != fb->sink_master || fa->source_master != fb->source_master)
        return 1;
    if ((r = strcmp(fa->name, fb->name)))
        return r;

    return 0;
}

static struct filter *filter_new(const char *name, const char *parameters, pa_sink *sink, pa_source *source) {
    struct filter *f;

    pa_assert(sink || source);

    f = pa_xnew(struct filter, 1);
    f->name = pa_xstrdup(name);
    f->parameters = pa_xstrdup(parameters);
    f->sink_master = sink;
    f->source_master = source;
    f->module_index = PA_INVALID_INDEX;
    f->sink = NULL;
    f->source = NULL;

    return f;
}

static void filter_free(struct filter *f) {
    if (f) {
        pa_xfree(f->name);
        pa_xfree(f->parameters);
        pa_xfree(f);
    }
}

static const char* get_filter_name(pa_object *o, bool is_sink_input) {
    const char *apply;
    pa_proplist *pl;

    if (is_sink_input)
        pl = PA_SINK_INPUT(o)->proplist;
    else
        pl = PA_SOURCE_OUTPUT(o)->proplist;

    /* If the stream doesn't want any filter, then let it be. */
    if ((apply = pa_proplist_gets(pl, PA_PROP_FILTER_APPLY)) && !pa_streq(apply, "")) {
        const char* suppress = pa_proplist_gets(pl, PA_PROP_FILTER_SUPPRESS);

        if (!suppress || !pa_streq(suppress, apply))
            return apply;
    }

    return NULL;
}

static const char* get_filter_parameters(pa_object *o, const char *want, bool is_sink_input) {
    const char *parameters;
    char *prop_parameters;
    pa_proplist *pl;

    if (is_sink_input)
        pl = PA_SINK_INPUT(o)->proplist;
    else
        pl = PA_SOURCE_OUTPUT(o)->proplist;

    prop_parameters = pa_sprintf_malloc(PA_PROP_FILTER_APPLY_PARAMETERS, want);
    parameters = pa_proplist_gets(pl, prop_parameters);
    pa_xfree(prop_parameters);

    return parameters;
}

/* This function is used to set or unset the filter related stream properties. This is necessary
 * if a stream does not have filter.apply set and is manually moved to a filter sink or source.
 * In this case, the properties must be temporarily set and removed when the stream is moved away
 * from the filter. */
static void set_filter_properties(pa_proplist *pl, struct filter *filter, bool set_properties) {
    char *prop_parameters;

    if (set_properties) {
        pa_assert(filter);

        pa_proplist_sets(pl, PA_PROP_FILTER_APPLY, filter->name);

        if (filter->parameters) {
            prop_parameters = pa_sprintf_malloc(PA_PROP_FILTER_APPLY_PARAMETERS, filter->name);
            pa_proplist_sets(pl, prop_parameters, filter->parameters);
            pa_xfree(prop_parameters);
        }

        pa_proplist_sets(pl, PA_PROP_FILTER_APPLY_SET_BY_MFA, "1");

    } else {
        const char *old_filter_name = NULL;

        if (filter)
            old_filter_name = filter->name;
        else
            old_filter_name = pa_proplist_gets(pl, PA_PROP_FILTER_APPLY);

        /* If the filter name cannot be determined, properties cannot be removed. */
        if (!old_filter_name)
            return;

        prop_parameters = pa_sprintf_malloc(PA_PROP_FILTER_APPLY_PARAMETERS, old_filter_name);
        pa_proplist_unset(pl, prop_parameters);
        pa_xfree(prop_parameters);

        pa_proplist_unset(pl, PA_PROP_FILTER_APPLY);
        pa_proplist_unset(pl, PA_PROP_FILTER_APPLY_SET_BY_MFA);
    }
}

static struct filter* get_filter_for_object(struct userdata *u, pa_object *o, bool is_sink_input) {
    pa_sink *sink = NULL;
    pa_source *source = NULL;
    struct filter *filter = NULL;
    void *state;

    if (is_sink_input)
        sink = PA_SINK_INPUT(o)->sink;
    else
        source = PA_SOURCE_OUTPUT(o)->source;

    PA_HASHMAP_FOREACH(filter, u->filters, state) {
        if ((is_sink_input && sink == filter->sink) || (!is_sink_input && source == filter->source)) {
            return filter;
        }
    }

    return NULL;
}

static bool should_group_filter(struct filter *filter) {
    return pa_streq(filter->name, "echo-cancel");
}

static char* get_group(pa_object *o, bool is_sink_input) {
    pa_proplist *pl;

    if (is_sink_input)
        pl = PA_SINK_INPUT(o)->proplist;
    else
        pl = PA_SOURCE_OUTPUT(o)->proplist;

    /* There's a bit of cleverness here -- the second argument ensures that we
     * only group streams that require the same filter */
    return pa_proplist_get_stream_group(pl, pa_proplist_gets(pl, PA_PROP_FILTER_APPLY), NULL);
}

/* For filters that apply on a source-output/sink-input pair, this finds the
 * master sink if we know the master source, or vice versa. It does this by
 * looking up streams that belong to the same stream group as the original
 * object. The idea is that streams from the sam group are always routed
 * together. */
static bool find_paired_master(struct userdata *u, struct filter *filter, pa_object *o, bool is_sink_input) {
    char *group;

    if ((group = get_group(o, is_sink_input))) {
        uint32_t idx;
        char *g;
        char *module_name = pa_sprintf_malloc("module-%s", filter->name);

        if (is_sink_input) {
            pa_source_output *so;

            PA_IDXSET_FOREACH(so, u->core->source_outputs, idx) {
                g = get_group(PA_OBJECT(so), false);

                if (pa_streq(g, group)) {
                    if (pa_streq(module_name, so->source->module->name)) {
                        /* Make sure we are not routing to the monitor source
                         * of the same filter */
                        if (so->source->monitor_of) {
                            pa_xfree(g);
                            continue;
                        }
                        /* Make sure we're not routing to another instance of
                         * the same filter. */
                        filter->source_master = so->source->output_from_master->source;
                    } else {
                        filter->source_master = so->source;
                    }

                    pa_xfree(g);
                    break;
                }

                pa_xfree (g);
            }
        } else {
            pa_sink_input *si;

            PA_IDXSET_FOREACH(si, u->core->sink_inputs, idx) {
                g = get_group(PA_OBJECT(si), true);

                if (pa_streq(g, group)) {
                    if (pa_streq(module_name, si->sink->module->name)) {
                        /* Make sure we're not routing to another instance of
                         * the same filter. */
                        filter->sink_master = si->sink->input_to_master->sink;
                    } else {
                        filter->sink_master = si->sink;
                    }

                    pa_xfree(g);
                    break;
                }

                pa_xfree(g);
            }
        }

        pa_xfree(group);
        pa_xfree(module_name);

        if (!filter->sink_master || !filter->source_master)
            return false;
    }

    return true;
}

static bool nothing_attached(struct filter *f) {
    bool no_si = true, no_so = true;

    if (f->sink)
        no_si = pa_idxset_isempty(f->sink->inputs);
    if (f->source)
        no_so = pa_idxset_isempty(f->source->outputs);

    return no_si && no_so;
}

static void housekeeping_time_callback(pa_mainloop_api*a, pa_time_event* e, const struct timeval *t, void *userdata) {
    struct userdata *u = userdata;
    struct filter *filter;
    void *state;

    pa_assert(a);
    pa_assert(e);
    pa_assert(u);

    pa_assert(e == u->housekeeping_time_event);
    u->core->mainloop->time_free(u->housekeeping_time_event);
    u->housekeeping_time_event = NULL;

    PA_HASHMAP_FOREACH(filter, u->filters, state) {
        if (nothing_attached(filter)) {
            uint32_t idx;

            pa_log_debug("Detected filter %s as no longer used. Unloading.", filter->name);
            idx = filter->module_index;
            pa_hashmap_remove(u->filters, filter);
            filter_free(filter);
            pa_module_unload_request_by_index(u->core, idx, true);
        }
    }

    pa_log_info("Housekeeping Done.");
}

static void trigger_housekeeping(struct userdata *u) {
    pa_assert(u);

    if (!u->autoclean)
        return;

    if (u->housekeeping_time_event)
        return;

    u->housekeeping_time_event = pa_core_rttime_new(u->core, pa_rtclock_now() + HOUSEKEEPING_INTERVAL, housekeeping_time_callback, u);
}

static int do_move(struct userdata *u, pa_object *obj, pa_object *parent, bool is_input) {
    /* Keep track of objects that we've marked for module-device-manager to ignore */
    pa_hashmap_put(is_input ? u->mdm_ignored_inputs : u->mdm_ignored_outputs, obj, obj);

    if (is_input) {
        pa_sink_input_set_property(PA_SINK_INPUT(obj), PA_PROP_MDM_AUTO_FILTERED, "1");
        return pa_sink_input_move_to(PA_SINK_INPUT(obj), PA_SINK(parent), false);
    } else {
        pa_source_output_set_property(PA_SOURCE_OUTPUT(obj), PA_PROP_MDM_AUTO_FILTERED, "1");
        return pa_source_output_move_to(PA_SOURCE_OUTPUT(obj), PA_SOURCE(parent), false);
    }
}

static void move_object_for_filter(struct userdata *u, pa_object *o, struct filter *filter, bool restore, bool is_sink_input) {
    pa_object *parent;
    pa_proplist *pl;
    const char *name;

    pa_assert(o);
    pa_assert(filter);

    if (is_sink_input) {
        pl = PA_SINK_INPUT(o)->proplist;
        parent = PA_OBJECT(restore ? filter->sink_master : filter->sink);
        if (!parent)
            return;
        name = PA_SINK(parent)->name;
    } else {
        pl = PA_SOURCE_OUTPUT(o)->proplist;
        parent = PA_OBJECT(restore ? filter->source_master : filter->source);
        if (!parent)
            return;
        name = PA_SOURCE(parent)->name;
    }

    pa_proplist_sets(pl, PA_PROP_FILTER_APPLY_MOVING, "1");

    if (do_move(u, o, parent, is_sink_input) < 0)
        pa_log_info("Failed to move %s for \"%s\" to <%s>.", is_sink_input ? "sink-input" : "source-output",
                    pa_strnull(pa_proplist_gets(pl, PA_PROP_APPLICATION_NAME)), name);
    else
        pa_log_info("Successfully moved %s for \"%s\" to <%s>.", is_sink_input ? "sink-input" : "source-output",
                    pa_strnull(pa_proplist_gets(pl, PA_PROP_APPLICATION_NAME)), name);

    pa_proplist_unset(pl, PA_PROP_FILTER_APPLY_MOVING);
}

static void move_objects_for_filter(struct userdata *u, pa_object *o, struct filter *filter, bool restore,
        bool is_sink_input) {

    if (!should_group_filter(filter))
        move_object_for_filter(u, o, filter, restore, is_sink_input);
    else {
        pa_source_output *so;
        pa_sink_input *si;
        char *g, *group;
        uint32_t idx;

        group = get_group(o, is_sink_input);

        PA_IDXSET_FOREACH(so, u->core->source_outputs, idx) {
            g = get_group(PA_OBJECT(so), false);

            if (pa_streq(g, group))
                move_object_for_filter(u, PA_OBJECT(so), filter, restore, false);

            pa_xfree(g);
        }

        PA_IDXSET_FOREACH(si, u->core->sink_inputs, idx) {
            g = get_group(PA_OBJECT(si), true);

            if (pa_streq(g, group))
                move_object_for_filter(u, PA_OBJECT(si), filter, restore, true);

            pa_xfree(g);
        }

        pa_xfree(group);
    }
}

/* Note that we assume a filter will provide at most one sink and at most one
 * source (and at least one of either). */
static void find_filters_for_module(struct userdata *u, pa_module *m, const char *name, const char *parameters) {
    uint32_t idx;
    pa_sink *sink;
    pa_source *source;
    struct filter *fltr = NULL;

    PA_IDXSET_FOREACH(sink, u->core->sinks, idx) {
        if (sink->module == m) {
            pa_assert(pa_sink_is_filter(sink));

            fltr = filter_new(name, parameters, sink->input_to_master->sink, NULL);
            fltr->module_index = m->index;
            fltr->sink = sink;

            break;
        }
    }

    PA_IDXSET_FOREACH(source, u->core->sources, idx) {
        if (source->module == m && !source->monitor_of) {
            pa_assert(pa_source_is_filter(source));

            if (!fltr) {
                fltr = filter_new(name, parameters, NULL, source->output_from_master->source);
                fltr->module_index = m->index;
                fltr->source = source;
            } else {
                fltr->source = source;
                fltr->source_master = source->output_from_master->source;
            }

            break;
        }
    }

    pa_hashmap_put(u->filters, fltr, fltr);
}

static bool can_unload_module(struct userdata *u, uint32_t idx) {
    void *state;
    struct filter *filter;

    /* Check if any other struct filters point to the same module */
    PA_HASHMAP_FOREACH(filter, u->filters, state) {
        if (filter->module_index == idx && !nothing_attached(filter))
            return false;
    }

    return true;
}

static pa_hook_result_t process(struct userdata *u, pa_object *o, bool is_sink_input, bool is_property_change) {
    const char *want;
    const char *parameters;
    bool done_something = false;
    pa_sink *sink = NULL;
    pa_source *source = NULL;
    pa_module *module = NULL;
    char *module_name = NULL;
    struct filter *fltr = NULL, *filter = NULL;
    pa_proplist *pl;

    if (is_sink_input) {
        if ((sink = PA_SINK_INPUT(o)->sink))
            module = sink->module;
        pl = PA_SINK_INPUT(o)->proplist;
    } else {
        if ((source = PA_SOURCE_OUTPUT(o)->source))
            module = source->module;
        pl = PA_SOURCE_OUTPUT(o)->proplist;
    }

    /* If there is no sink/source yet, we can't do much */
    if ((is_sink_input && !sink) || (!is_sink_input && !source))
        goto done;

    /* If the stream doesn't want any filter, then let it be. */
    if ((want = get_filter_name(o, is_sink_input))) {
        /* We need to ensure the SI is playing on a sink of this type
         * attached to the sink it's "officially" playing on */

        if (!module)
            goto done;

        module_name = pa_sprintf_malloc("module-%s", want);
        if (pa_streq(module->name, module_name)) {
            pa_log_debug("Stream appears to be playing on an appropriate sink already. Ignoring.");
            goto done;
        }

        /* If the stream originally did not have the filter.apply property set and is
         * manually moved away from the filter, remove the filter properties from the
         * stream */
        if (pa_proplist_gets(pl, PA_PROP_FILTER_APPLY_SET_BY_MFA)) {

            set_filter_properties(pl, NULL, false);

            /* If the new sink/source is also a filter, the stream has been moved from
             * one filter to another, so add the properties for the new filter. */
            if ((filter = get_filter_for_object(u, o, is_sink_input)))
                set_filter_properties(pl, filter, true);

            done_something = true;
            goto done;
        }

        /* The stream needs be moved to a filter. */

        /* Some filter modules might require parameters by default.
         * (e.g 'plugin', 'label', 'control' of module-ladspa-sink) */
        parameters = get_filter_parameters(o, want, is_sink_input);

        fltr = filter_new(want, parameters, sink, source);

        if (should_group_filter(fltr) && !find_paired_master(u, fltr, o, is_sink_input)) {
            pa_log_debug("Want group filtering but don't have enough streams.");
            goto done;
        }

        if (!(filter = pa_hashmap_get(u->filters, fltr))) {
            char *args;
            pa_module *m;

            args = pa_sprintf_malloc("autoloaded=1 %s%s %s%s %s",
                    fltr->sink_master ? "sink_master=" : "",
                    fltr->sink_master ? fltr->sink_master->name : "",
                    fltr->source_master ? "source_master=" : "",
                    fltr->source_master ? fltr->source_master->name : "",
                    fltr->parameters ? fltr->parameters : "");

            pa_log_debug("Loading %s with arguments '%s'", module_name, args);

            if (pa_module_load(&m, u->core, module_name, args) >= 0) {
                find_filters_for_module(u, m, want, parameters);
                filter = pa_hashmap_get(u->filters, fltr);
                done_something = true;
            }
            pa_xfree(args);
        }

        if (!filter) {
            pa_log("Unable to load %s", module_name);
            goto done;
        }

        /* We can move the stream now as we know the destination. If this
         * isn't true, we will do it later when the sink appears. */
        if ((is_sink_input && filter->sink) || (!is_sink_input && filter->source)) {
            move_objects_for_filter(u, o, filter, false, is_sink_input);
            done_something = true;
        }
    } else {
        /* The filter.apply property is not set. If the stream is nevertheless using a
         * filter sink/source, it either has been moved to the filter manually or the
         * user just removed the filter.apply property. */

        if ((filter = get_filter_for_object(u, o, is_sink_input))) {
            if (is_property_change) {
                /* 'filter.apply' has been manually unset. Do restore. */
                move_objects_for_filter(u, o, filter, true, is_sink_input);
                set_filter_properties(pl, filter, false);
                done_something = true;
            } else {
                /* Stream has been manually moved to a filter sink/source
                 * without 'filter.apply' set. Leave sink as it is. */
                set_filter_properties(pl, filter, true);
            }
        }
    }

done:
    if (done_something)
        trigger_housekeeping(u);

    pa_xfree(module_name);
    filter_free(fltr);

    return PA_HOOK_OK;
}

static pa_hook_result_t sink_input_put_cb(pa_core *core, pa_sink_input *i, struct userdata *u) {
    pa_core_assert_ref(core);
    pa_sink_input_assert_ref(i);

    return process(u, PA_OBJECT(i), true, false);
}

static pa_hook_result_t sink_input_move_finish_cb(pa_core *core, pa_sink_input *i, struct userdata *u) {
    pa_core_assert_ref(core);
    pa_sink_input_assert_ref(i);

    if (pa_proplist_gets(i->proplist, PA_PROP_FILTER_APPLY_MOVING))
        return PA_HOOK_OK;

    /* If we're managing m-d-m.auto_filtered on this, remove and re-add if we're continuing to manage it */
    pa_hashmap_remove(u->mdm_ignored_inputs, i);

    return process(u, PA_OBJECT(i), true, false);
}

static pa_hook_result_t sink_input_proplist_cb(pa_core *core, pa_sink_input *i, struct userdata *u) {
    pa_core_assert_ref(core);
    pa_sink_input_assert_ref(i);

    /* Eliminate nested and redundant hook event that is triggered by
       pa_sink_input_set_property() in do_move(). */
    if (pa_proplist_gets(i->proplist, PA_PROP_FILTER_APPLY_MOVING))
        return PA_HOOK_OK;

    return process(u, PA_OBJECT(i), true, true);
}

static pa_hook_result_t sink_input_unlink_cb(pa_core *core, pa_sink_input *i, struct userdata *u) {
    pa_core_assert_ref(core);
    pa_sink_input_assert_ref(i);

    pa_assert(u);

    if (pa_hashmap_size(u->filters) > 0)
        trigger_housekeeping(u);

    pa_hashmap_remove(u->mdm_ignored_inputs, i);

    return PA_HOOK_OK;
}

static pa_hook_result_t sink_unlink_cb(pa_core *core, pa_sink *sink, struct userdata *u) {
    void *state;
    struct filter *filter = NULL;

    pa_core_assert_ref(core);
    pa_sink_assert_ref(sink);
    pa_assert(u);

    /* If either the parent or the sink we've loaded disappears,
     * we should remove it from our hashmap */
    PA_HASHMAP_FOREACH(filter, u->filters, state) {
        if (filter->sink_master == sink || filter->sink == sink) {
            uint32_t idx;

            /* Attempt to rescue any streams to the parent sink as this is likely
             * the best course of action (as opposed to a generic rescue via
             * module-rescue-streams */
            if (filter->sink == sink) {
                pa_sink_input *i;

                PA_IDXSET_FOREACH(i, sink->inputs, idx)
                    move_objects_for_filter(u, PA_OBJECT(i), filter, true, true);
            }

            idx = filter->module_index;
            pa_hashmap_remove(u->filters, filter);
            filter_free(filter);

            if (can_unload_module(u, idx))
                pa_module_unload_request_by_index(u->core, idx, true);
        }
    }

    return PA_HOOK_OK;
}

static pa_hook_result_t source_output_put_cb(pa_core *core, pa_source_output *o, struct userdata *u) {
    pa_core_assert_ref(core);
    pa_source_output_assert_ref(o);

    return process(u, PA_OBJECT(o), false, false);
}

static pa_hook_result_t source_output_move_finish_cb(pa_core *core, pa_source_output *o, struct userdata *u) {
    pa_core_assert_ref(core);
    pa_source_output_assert_ref(o);

    if (pa_proplist_gets(o->proplist, PA_PROP_FILTER_APPLY_MOVING))
        return PA_HOOK_OK;

    /* If we're managing m-d-m.auto_filtered on this, remove and re-add if we're continuing to manage it */
    pa_hashmap_remove(u->mdm_ignored_outputs, o);

    return process(u, PA_OBJECT(o), false, false);
}

static pa_hook_result_t source_output_proplist_cb(pa_core *core, pa_source_output *o, struct userdata *u) {
    pa_core_assert_ref(core);
    pa_source_output_assert_ref(o);

    /* Eliminate nested and redundant hook event that is triggered by
       pa_source_output_set_property() in do_move(). */
    if (pa_proplist_gets(o->proplist, PA_PROP_FILTER_APPLY_MOVING))
        return PA_HOOK_OK;

    return process(u, PA_OBJECT(o), false, true);
}

static pa_hook_result_t source_output_unlink_cb(pa_core *core, pa_source_output *o, struct userdata *u) {
    pa_core_assert_ref(core);
    pa_source_output_assert_ref(o);

    pa_assert(u);

    if (pa_hashmap_size(u->filters) > 0)
        trigger_housekeeping(u);

    pa_hashmap_remove(u->mdm_ignored_outputs, o);

    return PA_HOOK_OK;
}

static pa_hook_result_t source_unlink_cb(pa_core *core, pa_source *source, struct userdata *u) {
    void *state;
    struct filter *filter = NULL;

    pa_core_assert_ref(core);
    pa_source_assert_ref(source);
    pa_assert(u);

    /* If either the parent or the source we've loaded disappears,
     * we should remove it from our hashmap */
    PA_HASHMAP_FOREACH(filter, u->filters, state) {
        if (filter->source_master == source || filter->source == source) {
            uint32_t idx;

            /* Attempt to rescue any streams to the parent source as this is likely
             * the best course of action (as opposed to a generic rescue via
             * module-rescue-streams */
            if (filter->source == source) {
                pa_source_output *o;

                PA_IDXSET_FOREACH(o, source->outputs, idx)
                    move_objects_for_filter(u, PA_OBJECT(o), filter, true, false);
            }

            idx = filter->module_index;
            pa_hashmap_remove(u->filters, filter);
            filter_free(filter);

            if (can_unload_module(u, idx))
                pa_module_unload_request_by_index(u->core, idx, true);
        }
    }

    return PA_HOOK_OK;
}

static void unset_mdm_ignore_input(pa_sink_input *i)
{
    pa_sink_input_set_property(i, PA_PROP_MDM_AUTO_FILTERED, NULL);
}

static void unset_mdm_ignore_output(pa_source_output *o)
{
    pa_source_output_set_property(o, PA_PROP_MDM_AUTO_FILTERED, NULL);
}

int pa__init(pa_module *m) {
    pa_modargs *ma = NULL;
    struct userdata *u;

    pa_assert(m);

    if (!(ma = pa_modargs_new(m->argument, valid_modargs))) {
        pa_log("Failed to parse module arguments");
        goto fail;
    }

    m->userdata = u = pa_xnew0(struct userdata, 1);

    u->core = m->core;

    u->autoclean = DEFAULT_AUTOCLEAN;
    if (pa_modargs_get_value_boolean(ma, "autoclean", &u->autoclean) < 0) {
        pa_log("Failed to parse autoclean value");
        goto fail;
    }

    u->filters = pa_hashmap_new(filter_hash, filter_compare);
    u->mdm_ignored_inputs = pa_hashmap_new_full(NULL, NULL, (pa_free_cb_t) unset_mdm_ignore_input, NULL);
    u->mdm_ignored_outputs = pa_hashmap_new_full(NULL, NULL, (pa_free_cb_t) unset_mdm_ignore_output, NULL);

    pa_module_hook_connect(m, &m->core->hooks[PA_CORE_HOOK_SINK_INPUT_PUT], PA_HOOK_LATE, (pa_hook_cb_t) sink_input_put_cb, u);
    pa_module_hook_connect(m, &m->core->hooks[PA_CORE_HOOK_SINK_INPUT_MOVE_FINISH], PA_HOOK_LATE, (pa_hook_cb_t) sink_input_move_finish_cb, u);
    pa_module_hook_connect(m, &m->core->hooks[PA_CORE_HOOK_SINK_INPUT_PROPLIST_CHANGED], PA_HOOK_LATE, (pa_hook_cb_t) sink_input_proplist_cb, u);
    pa_module_hook_connect(m, &m->core->hooks[PA_CORE_HOOK_SINK_INPUT_UNLINK], PA_HOOK_LATE, (pa_hook_cb_t) sink_input_unlink_cb, u);
    pa_module_hook_connect(m, &m->core->hooks[PA_CORE_HOOK_SINK_UNLINK], PA_HOOK_LATE-1, (pa_hook_cb_t) sink_unlink_cb, u);
    pa_module_hook_connect(m, &m->core->hooks[PA_CORE_HOOK_SOURCE_OUTPUT_PUT], PA_HOOK_LATE, (pa_hook_cb_t) source_output_put_cb, u);
    pa_module_hook_connect(m, &m->core->hooks[PA_CORE_HOOK_SOURCE_OUTPUT_MOVE_FINISH], PA_HOOK_LATE, (pa_hook_cb_t) source_output_move_finish_cb, u);
    pa_module_hook_connect(m, &m->core->hooks[PA_CORE_HOOK_SOURCE_OUTPUT_PROPLIST_CHANGED], PA_HOOK_LATE, (pa_hook_cb_t) source_output_proplist_cb, u);
    pa_module_hook_connect(m, &m->core->hooks[PA_CORE_HOOK_SOURCE_OUTPUT_UNLINK], PA_HOOK_LATE, (pa_hook_cb_t) source_output_unlink_cb, u);
    pa_module_hook_connect(m, &m->core->hooks[PA_CORE_HOOK_SOURCE_UNLINK], PA_HOOK_LATE-1, (pa_hook_cb_t) source_unlink_cb, u);

    pa_modargs_free(ma);

    return 0;

fail:
    pa__done(m);

    if (ma)
        pa_modargs_free(ma);

    return -1;
}

void pa__done(pa_module *m) {
    struct userdata* u;

    pa_assert(m);

    if (!(u = m->userdata))
        return;

    if (u->housekeeping_time_event)
        u->core->mainloop->time_free(u->housekeeping_time_event);

    if (u->filters) {
        struct filter *f;

        while ((f = pa_hashmap_steal_first(u->filters))) {
            pa_module_unload_request_by_index(u->core, f->module_index, true);
            filter_free(f);
        }

        pa_hashmap_free(u->filters);
    }

    if (u->mdm_ignored_inputs)
        pa_hashmap_free(u->mdm_ignored_inputs);

    if (u->mdm_ignored_outputs)
        pa_hashmap_free(u->mdm_ignored_outputs);

    pa_xfree(u);
}
