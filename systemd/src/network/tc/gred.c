/* SPDX-License-Identifier: LGPL-2.1-or-later
 * Copyright © 2020 VMware, Inc. */

#include <linux/pkt_sched.h>

#include "alloc-util.h"
#include "conf-parser.h"
#include "netlink-util.h"
#include "parse-util.h"
#include "qdisc.h"
#include "string-util.h"

static int generic_random_early_detection_init(QDisc *qdisc) {
        GenericRandomEarlyDetection *gred;

        assert(qdisc);

        gred = GRED(qdisc);

        gred->grio = -1;

        return 0;
}

static int generic_random_early_detection_fill_message(Link *link, QDisc *qdisc, sd_netlink_message *req) {
        GenericRandomEarlyDetection *gred;
        struct tc_gred_sopt opt = {};
        int r;

        assert(link);
        assert(qdisc);
        assert(req);

        gred = GRED(qdisc);

        opt.DPs = gred->virtual_queues;
        opt.def_DP = gred->default_virtual_queue;

        if (gred->grio >= 0)
                opt.grio = gred->grio;

        r = sd_netlink_message_open_container_union(req, TCA_OPTIONS, "gred");
        if (r < 0)
                return log_link_error_errno(link, r, "Could not open container TCA_OPTIONS: %m");

        r = sd_netlink_message_append_data(req, TCA_GRED_DPS, &opt, sizeof(struct tc_gred_sopt));
        if (r < 0)
                return log_link_error_errno(link, r, "Could not append TCA_GRED_DPS attribute: %m");

        r = sd_netlink_message_close_container(req);
        if (r < 0)
                return log_link_error_errno(link, r, "Could not close container TCA_OPTIONS: %m");

        return 0;
}

static int generic_random_early_detection_verify(QDisc *qdisc) {
        GenericRandomEarlyDetection *gred = GRED(qdisc);

        if (gred->default_virtual_queue >= gred->virtual_queues)
                return log_warning_errno(SYNTHETIC_ERRNO(EINVAL),
                                         "%s: DefaultVirtualQueue= must be less than VirtualQueues=. "
                                         "Ignoring [GenericRandomEarlyDetection] section from line %u.",
                                         qdisc->section->filename, qdisc->section->line);

        return 0;
}

int config_parse_generic_random_early_detection_u32(
                const char *unit,
                const char *filename,
                unsigned line,
                const char *section,
                unsigned section_line,
                const char *lvalue,
                int ltype,
                const char *rvalue,
                void *data,
                void *userdata) {

        _cleanup_(qdisc_free_or_set_invalidp) QDisc *qdisc = NULL;
        GenericRandomEarlyDetection *gred;
        Network *network = data;
        uint32_t *p;
        uint32_t v;
        int r;

        assert(filename);
        assert(lvalue);
        assert(rvalue);
        assert(data);

        r = qdisc_new_static(QDISC_KIND_GRED, network, filename, section_line, &qdisc);
        if (r == -ENOMEM)
                return log_oom();
        if (r < 0) {
                log_syntax(unit, LOG_WARNING, filename, line, r,
                           "More than one kind of queueing discipline, ignoring assignment: %m");
                return 0;
        }

        gred = GRED(qdisc);

        if (streq(lvalue, "VirtualQueues"))
                p = &gred->virtual_queues;
        else if (streq(lvalue, "DefaultVirtualQueue"))
                p = &gred->default_virtual_queue;
        else
                assert_not_reached("Invalid lvalue.");

        if (isempty(rvalue)) {
                *p = 0;

                qdisc = NULL;
                return 0;
        }

        r = safe_atou32(rvalue, &v);
        if (r < 0) {
                log_syntax(unit, LOG_WARNING, filename, line, r,
                           "Failed to parse '%s=', ignoring assignment: %s",
                           lvalue, rvalue);
                return 0;
        }

        if (v > MAX_DPs)
                log_syntax(unit, LOG_WARNING, filename, line, 0,
                           "Invalid '%s=', ignoring assignment: %s",
                           lvalue, rvalue);

        *p = v;
        qdisc = NULL;

        return 0;
}
int config_parse_generic_random_early_detection_bool(
                const char *unit,
                const char *filename,
                unsigned line,
                const char *section,
                unsigned section_line,
                const char *lvalue,
                int ltype,
                const char *rvalue,
                void *data,
                void *userdata) {

        _cleanup_(qdisc_free_or_set_invalidp) QDisc *qdisc = NULL;
        GenericRandomEarlyDetection *gred;
        Network *network = data;
        int r;

        assert(filename);
        assert(lvalue);
        assert(rvalue);
        assert(data);

        r = qdisc_new_static(QDISC_KIND_GRED, network, filename, section_line, &qdisc);
        if (r == -ENOMEM)
                return log_oom();
        if (r < 0) {
                log_syntax(unit, LOG_WARNING, filename, line, r,
                           "More than one kind of queueing discipline, ignoring assignment: %m");
                return 0;
        }

        gred = GRED(qdisc);

        if (isempty(rvalue)) {
                gred->grio = -1;

                qdisc = NULL;
                return 0;
        }

        r = parse_boolean(rvalue);
        if (r < 0) {
                log_syntax(unit, LOG_WARNING, filename, line, r,
                           "Failed to parse '%s=', ignoring assignment: %s",
                           lvalue, rvalue);
                return 0;
        }

        gred->grio = r;
        qdisc = NULL;

        return 0;
}

const QDiscVTable gred_vtable = {
        .object_size = sizeof(GenericRandomEarlyDetection),
        .tca_kind = "gred",
        .init = generic_random_early_detection_init,
        .fill_message = generic_random_early_detection_fill_message,
        .verify = generic_random_early_detection_verify,
};
