###########################################################################
#
#   tiny.mak
#
#   Small driver-specific example makefile
#   Use make SUBTARGET=tiny to build
#
#   Copyright Nicola Salmoria and the MAME Team.
#   Visit  http://mamedev.org for licensing and usage restrictions.
#
###########################################################################

MAMESRC = $(SRC)/mame
MAMEOBJ = $(OBJ)/mame

AUDIO = $(MAMEOBJ)/audio
DRIVERS = $(MAMEOBJ)/drivers
LAYOUT = $(MAMEOBJ)/layout
MACHINE = $(MAMEOBJ)/machine
VIDEO = $(MAMEOBJ)/video

OBJDIRS += \
	$(AUDIO) \
	$(DRIVERS) \
	$(LAYOUT) \
	$(MACHINE) \
	$(VIDEO) \



#-------------------------------------------------
# Specify all the CPU cores necessary for the
# drivers referenced in tiny.c.
#-------------------------------------------------

CPUS += Z80
CPUS += M6502
CPUS += MCS48
CPUS += MCS51
CPUS += M6800
CPUS += M6809
CPUS += M680X0
CPUS += TMS9900
CPUS += COP400
CPUS += UPD7810
CPUS += M6805
CPUS += I386
CPUS += POWERPC
CPUS += DSP16A
#-------------------------------------------------
# Specify all the sound cores necessary for the
# drivers referenced in tiny.c.
#-------------------------------------------------

SOUNDS += SAMPLES
SOUNDS += DAC
SOUNDS += DISCRETE
SOUNDS += AY8910
SOUNDS += YM2151
SOUNDS += YM2203
SOUNDS += YM2608
SOUNDS += YM2610
SOUNDS += YM2610B
SOUNDS += ASTROCADE
SOUNDS += TMS5220
SOUNDS += OKIM6295
SOUNDS += HC55516
SOUNDS += YM3812
SOUNDS += CEM3394
SOUNDS += VOTRAX
SOUNDS += BEEP
SOUNDS += SPEAKER
SOUNDS += CDDA
SOUNDS += QSOUND
SOUNDS += MSM5205
#-------------------------------------------------
# specify available video cores
#-------------------------------------------------

#-------------------------------------------------
# specify available machine cores
#-------------------------------------------------

MACHINES += 6821PIA
MACHINES += TTL74148
MACHINES += TTL74153
MACHINES += TTL7474
MACHINES += RIOT6532
MACHINES += PIT8253
MACHINES += Z80CTC
MACHINES += 68681
MACHINES += BANKDEV
MACHINES += Z80DART
MACHINES += Z80PIO
MACHINES += EEPROMDEV
MACHINES += E05A03
MACHINES += E05A30
MACHINES += STEPPERS
MACHINES += UPD1990A
MACHINES += TIMEKPR
MACHINES += I8255
#-------------------------------------------------
# specify available bus cores
#-------------------------------------------------
BUSES += CENTRONICS
BUSES += SCSI
BUSES += NEOGEO
#-------------------------------------------------
# This is the list of files that are necessary
# for building all of the drivers referenced
# in tiny.c
#-------------------------------------------------

DRVLIBS = \
	$(MACHINE)/ticket.o \
	$(DRIVERS)/carpolo.o $(MACHINE)/carpolo.o $(VIDEO)/carpolo.o \
	$(DRIVERS)/circus.o $(AUDIO)/circus.o $(VIDEO)/circus.o \
	$(DRIVERS)/exidy.o $(AUDIO)/exidy.o $(VIDEO)/exidy.o \
	$(AUDIO)/exidy440.o \
	$(DRIVERS)/starfire.o $(VIDEO)/starfire.o \
	$(DRIVERS)/vertigo.o $(MACHINE)/vertigo.o $(VIDEO)/vertigo.o \
	$(DRIVERS)/victory.o $(VIDEO)/victory.o \
	$(AUDIO)/targ.o \
	$(DRIVERS)/astrocde.o $(VIDEO)/astrocde.o \
	$(DRIVERS)/gridlee.o $(AUDIO)/gridlee.o $(VIDEO)/gridlee.o \
	$(DRIVERS)/williams.o $(MACHINE)/williams.o $(AUDIO)/williams.o $(VIDEO)/williams.o \
	$(AUDIO)/gorf.o \
	$(AUDIO)/wow.o \
	$(DRIVERS)/gaelco.o $(VIDEO)/gaelco.o $(MACHINE)/gaelcrpt.o \
	$(DRIVERS)/wrally.o $(MACHINE)/wrally.o $(VIDEO)/wrally.o \
	$(DRIVERS)/looping.o \
	$(DRIVERS)/supertnk.o \
	$(DRIVERS)/neogeo.o $(VIDEO)/neogeo.o \
	$(DRIVERS)/neogeo_noslot.o \
	$(VIDEO)/neogeo_spr.o \
	$(MACHINE)/neoboot.o \
	$(MACHINE)/neocrypt.o \
	$(MACHINE)/neoprot.o \
	$(MACHINE)/ng_memcard.o \
	$(DRIVERS)/cps1.o $(VIDEO)/cps1.o \
	$(DRIVERS)/cps2.o $(MACHINE)/cps2crpt.o \
	$(DRIVERS)/fcrash.o \
	$(DRIVERS)/kenseim.o \
	$(MACHINE)/kabuki.o \

#-------------------------------------------------
# layout dependencies
#-------------------------------------------------

#$(DRIVERS)/astrocde.o:  $(LAYOUT)/gorf.lh \
						$(LAYOUT)/seawolf2.lh \
						$(LAYOUT)/spacezap.lh \
						$(LAYOUT)/tenpindx.lh
#$(DRIVERS)/circus.o:    $(LAYOUT)/circus.lh \
						$(LAYOUT)/crash.lh
