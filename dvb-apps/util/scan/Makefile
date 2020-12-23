# Makefile for linuxtv.org dvb-apps/util/scan

objects  = atsc_psip_section.o \
           diseqc.o            \
           dump-vdr.o          \
           dump-zap.o          \
           lnb.o               \
           scan.o              \
           section.o

binaries = scan

inst_bin = $(binaries)

removing = atsc_psip_section.c atsc_psip_section.h

CPPFLAGS += -Wno-packed-bitfield-compat -D__KERNEL_STRICT_NAMES

.PHONY: all

all: $(binaries)

$(binaries): $(objects)

$(objects): atsc_psip_section.c atsc_psip_section.h

atsc_psip_section.c atsc_psip_section.h:
	perl section_generate.pl atsc_psip_section.pl

include ../../Make.rules
