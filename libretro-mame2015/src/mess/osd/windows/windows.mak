###########################################################################
#
#   windows.mak
#
#   MESS Windows-specific makefile
#
###########################################################################

MESS_WINSRC = src/mess/osd/windows
MESS_WINOBJ = $(OBJ)/mess/osd/windows

OBJDIRS += \
	$(MESSOBJ)/osd \
	$(MESSOBJ)/osd/windows

RESFILE = $(MESS_WINOBJ)/mess.res

$(LIBOSD): $(OSDOBJS)

$(LIBOCORE): $(OSDCOREOBJS)

$(LIBOCORE_NOMAIN): $(OSDCOREOBJS:$(WINOBJ)/main.o=)

#-------------------------------------------------
# generic rules for the resource compiler
#-------------------------------------------------

$(MESS_WINOBJ)/%.res: $(MESS_WINSRC)/%.rc
	@echo Compiling resources $<...
	$(RC) $(RCDEFS) $(RCFLAGS) --include-dir $(MESS_WINOBJ) -o $@ -i $<


#-------------------------------------------------
# rules for resource file
#-------------------------------------------------

$(RESFILE): $(MESS_WINSRC)/mess.rc $(MESS_WINOBJ)/messvers.rc

$(MESS_WINOBJ)/messvers.rc: $(SRC)/build/verinfo.py $(SRC)/version.c
	@echo Emitting $@...
	$(PYTHON) $(SRC)/build/verinfo.py -b mess -o $@ $(SRC)/version.c
