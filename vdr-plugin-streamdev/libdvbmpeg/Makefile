#
# Makefile for a Video Disk Recorder plugin
#
# $Id: Makefile,v 1.5 2010/07/30 10:49:28 schmirl Exp $

### The object files (add further files here):

OBJS = ctools.o remux.o ringbuffy.o transform.o

### Disable attribute warn_unused_result

DEFINES += -U_FORTIFY_SOURCE

### The main target:

.PHONY: clean
libdvbmpegtools.a: $(OBJS)
	ar -rcs libdvbmpegtools.a $(OBJS) 

### Implicit rules:

%.o: %.c 
	$(CC) $(CFLAGS) -c $(DEFINES) $(INCLUDES) -o $@ $<

### Dependencies:

MAKEDEP = $(CC) -MM -MG
DEPFILE = .dependencies

$(DEPFILE): Makefile
	@$(MAKEDEP) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.c) > $@

-include $(DEPFILE)

### Targets:

clean:
	@-rm -f $(OBJS) $(DEPFILE) *.a core* *~
