# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (C) 2009-2013 Stephan Raue (stephan@openelec.tv)
# Copyright (C) 2013 Lutz Fiebach (lufie@openelec.tv)
# Copyright (C) 2018-present Team CoreELEC (https://coreelec.org)

ADDON_NAME=service.coreelec.settings

SHELL=/bin/bash
BUILDDIR=build
DATADIR=/usr/share/kodi
ADDONDIR=$(DATADIR)/addons
BUTTON_FOCUS_COLOR=FFC40300

################################################################################

all: $(BUILDDIR)/$(ADDON_NAME)

addon: $(BUILDDIR)/$(ADDON_NAME)-$(ADDON_VERSION).zip

install: $(BUILDDIR)/$(ADDON_NAME)
	mkdir -p $(DESTDIR)$(ADDONDIR)
	cp -R $(BUILDDIR)/$(ADDON_NAME) $(DESTDIR)$(ADDONDIR)

clean:
	rm -rf $(BUILDDIR)

uninstall:
	rm -rf $(DESTDIR)$(ADDONDIR)/$(ADDON_NAME)

$(BUILDDIR)/$(ADDON_NAME): $(BUILDDIR)/$(ADDON_NAME)/resources
	mkdir -p $(BUILDDIR)/$(ADDON_NAME)
	cp -R src/*.py $(BUILDDIR)/$(ADDON_NAME)
	cp addon.xml $(BUILDDIR)/$(ADDON_NAME)
	sed -e "s,@ADDONNAME@,$(ADDON_NAME),g" \
	    -e "s,@ADDONVERSION@,$(ADDON_VERSION),g" \
	    -e "s,@DISTRONAME@,$(DISTRONAME),g" \
	    -i $(BUILDDIR)/$(ADDON_NAME)/addon.xml
	cp changelog.txt $(BUILDDIR)/$(ADDON_NAME)

$(BUILDDIR)/$(ADDON_NAME)/resources: $(BUILDDIR)/$(ADDON_NAME)/resources/skins \
                                     $(BUILDDIR)/$(ADDON_NAME)/resources/language
	mkdir -p $(BUILDDIR)/$(ADDON_NAME)/resources
	cp -R src/resources/* $(BUILDDIR)/$(ADDON_NAME)/resources

$(BUILDDIR)/$(ADDON_NAME)/resources/skins: $(BUILDDIR)/$(ADDON_NAME)/resources/skins/Default/media
	mkdir -p $(BUILDDIR)/$(ADDON_NAME)/resources/skins/Default
	cp -R skins/Default/* $(BUILDDIR)/$(ADDON_NAME)/resources/skins/Default
	sed -e "s,button_focus,$(BUTTON_FOCUS_COLOR),g" -i $(BUILDDIR)/$(ADDON_NAME)/resources/skins/Default/1080i/*
	mkdir -p $(BUILDDIR)/$(ADDON_NAME)/resources/skins/skin.estuary/xml
	cp -R skins/Default/1080i/* $(BUILDDIR)/$(ADDON_NAME)/resources/skins/skin.estuary/xml

$(BUILDDIR)/$(ADDON_NAME)/resources/skins/Default/media:
	mkdir -p $(BUILDDIR)/$(ADDON_NAME)/resources/skins/Default/media
	cp textures/$(DISTRONAME)/*.{png,jpg} $(BUILDDIR)/$(ADDON_NAME)/resources/skins/Default/media
	mkdir -p $(BUILDDIR)/$(ADDON_NAME)/resources/skins/skin.estuary/media
	cp textures/$(DISTRONAME)/*.{png,jpg} $(BUILDDIR)/$(ADDON_NAME)/resources/skins/skin.estuary/media

$(BUILDDIR)/$(ADDON_NAME)/resources/language:
	mkdir -p $(BUILDDIR)/$(ADDON_NAME)/resources/language
	cp -R language/* $(BUILDDIR)/$(ADDON_NAME)/resources/language
	sed -e "s,@DISTRONAME@,$(DISTRONAME),g" \
	    -e "s,@ROOT_PASSWORD@,$(ROOT_PASSWORD),g" \
	    -i $(BUILDDIR)/$(ADDON_NAME)/resources/language/*/*.po

$(BUILDDIR)/$(ADDON_NAME)-$(ADDON_VERSION).zip: $(BUILDDIR)/$(ADDON_NAME)
	cd $(BUILDDIR); zip -r $(ADDON_NAME)-$(ADDON_VERSION).zip $(ADDON_NAME)
