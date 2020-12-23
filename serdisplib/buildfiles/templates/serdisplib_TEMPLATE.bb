DESCRIPTION = "library to drive LC-displays"
SECTION = "libs"
DEPENDS += "libusb"
LICENSE = "GPL"
PRIORITY = "optional"
MAINTAINER ="maf; Wolfgang Astleitner <mrwastl@users.sf.net>"
DEFAULT_PREFERENCE = "1"

PREMIRRORS_prepend () {
        ${SOURCEFORGE_MIRROR}   http://mesh.dl.sourceforge.net/sourceforge
}
SRC_URI = "${SOURCEFORGE_MIRROR}/serdisplib/serdisplib-${PV}.tar.gz"

S = "${WORKDIR}/serdisplib-${PV}"

inherit autotools

HEADER_FILES = serdisp.h serdisp_control.h serdisp_connect.h serdisp_messages.h \
               serdisp_colour.h serdisp_tools.h serdisp_parport.h

EXTRA_OECONF = "--enable-libusb"

do_stage() {
	oe_libinstall -a -so lib/libserdisp ${STAGING_LIBDIR}

        install -d ${STAGING_BINDIR}
	install -m 755 src/testserdisp ${STAGING_BINDIR}
	if test -e tools/multidisplay; then
		install -m 755 tools/multidisplay ${STAGING_BINDIR}
	fi

        install -d ${STAGING_INCDIR}/serdisplib
        for X in ${HEADER_FILES}
        do
                install -m 0644 include/serdisplib/$X ${STAGING_INCDIR}/serdisplib/$X
        done
}


do_install () {
	oe_libinstall -a -so lib/libserdisp ${D}${libdir}

	install -d ${D}${bindir}
	install -m 755 src/testserdisp ${D}${bindir}
	if test -e tools/multidisplay; then
		install -m 755 tools/multidisplay ${D}${bindir}
	fi

	install -d ${D}${includedir}/serdisplib

	for X in ${HEADER_FILES}		 
	do
	    install -m 0644 include/serdisplib/$X ${D}${includedir}/serdisplib/$X
	done
}

