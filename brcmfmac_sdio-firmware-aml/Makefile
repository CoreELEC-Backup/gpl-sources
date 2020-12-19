CC ?= gcc
CFLAGS ?= -Wall -g

CFLAGS  += $(DEPS_CFLAGS)
LDFLAGS += $(DEPS_LDFLAGS)

SRCS = brcm_patchram_plus.c
PROG = brcm_patchram_plus

all: $(PROG)

$(PROG): $(SRCS)
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

install:
	mkdir -p $(DESTDIR)/usr/bin
	cp -P $(PROG) $(DESTDIR)/usr/bin
	mkdir -p $(FWDIR)/lib/firmware
	cp -PR firmware/brcm $(FWDIR)/lib/firmware/

clean:
	rm -f $(PROG)
