BUILD_DIR := $(shell pwd)/v4l
TMP ?= /tmp

ifeq ($(EDITOR),)
  ifeq ($(VISUAL),)
	EDITOR := vi
  else
	EDITOR := $(VISUAL) -w
  endif
endif

all:

install:
	$(MAKE) -C $(BUILD_DIR) install

# Hmm, .PHONY does not work with wildcard rules :-(
SPECS = media-specs

.PHONY: $(SPECS)

$(SPECS):
	$(MAKE) -C $(BUILD_DIR) $(MAKECMDGOALS)

%::
	$(MAKE) -C $(BUILD_DIR) $(MAKECMDGOALS)

download untar::
	$(MAKE) -C linux/ $(MAKECMDGOALS)

dir::
	$(MAKE) -C linux/ $(MAKECMDGOALS) DIR="../$(DIR)"

cleanall:
	$(MAKE) distclean
	$(MAKE) -C linux distclean
