sbin_PROGRAMS = eventlircd
eventlircd_SOURCES = main.c monitor.c monitor.h input.c input.h lircd.c lircd.h
eventlircd_CFLAGS = $(AM_CFLAGS) $(LIBUDEV_CFLAGS) -DLIRCD_SOCKET=\"$(LIRCD_SOCKET)\" -DEVMAP_DIR=\"$(EVMAP_DIR)\"
eventlircd_LDFLAGS = $(AM_CFLAGS) $(LIBUDEV_LIBS)

nodist_eventlircd_SOURCES = event_name_to_code.h evkey_code_to_name.h evkey_type.h

event_name_to_code.h: event_name_to_code.h.sh $(ABSOLUTE_LINUX_INPUT_H)
	sh event_name_to_code.h.sh

evkey_code_to_name.h: evkey_code_to_name.h.sh $(ABSOLUTE_LINUX_INPUT_H)
	sh evkey_code_to_name.h.sh

evkey_type.h: evkey_type.h.sh $(ABSOLUTE_LINUX_INPUT_H)
	sh evkey_type.h.sh

input.c: event_name_to_code.h evkey_code_to_name.h evkey_type.h
