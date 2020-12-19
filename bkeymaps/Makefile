# this uses gentoo's portage-utils and the installed kbd keymaps to
# generate the binary maps.
#
# Your kbd needs to be patched with binary dump support:
# https://bugs.gentoo.org/show_bug.cgi?id=175179

PACKAGE=	bkeymaps
VERSION=	1.13

#SRCS=		$(shell qlist kbd | grep 'i386.*map.gz$$' | grep -v include ) 

BMAPDIR=	bkeymaps
OUTDIR=		$(PACKAGE)-$(VERSION)
DISTFILES=	Makefile
TARBALL=	$(PACKAGE)-$(VERSION).tar.gz

sharedir=	/usr/share


tar: $(TARBALL)

#generate:
#	rm -fr "$(OUTDIR)"
#	for map in $(SRCS) ; do 				\
#		subdir=`dirname $$map`;				\
#		subdir=`basename $$subdir`;			\
#		subdirs="$$subdirs $$subdir";			\
#		name=`basename $$map .map.gz`;			\
#		dest="$(OUTDIR)/$$subdir/$$name.bmap.bz2";	\
#		echo "$$dest";					\
#		mkdir -p "$(OUTDIR)/$$subdir";			\
#		loadkeys -b "$$map" > "$$dest" || exit 1;	\
#	done;							\
#	for dir in $$subdirs ; do				\
#		echo $$dir;					\
#	done | sort | uniq > subdirs

gen:	$(BMAPDIR)

$(BMAPDIR):
	rm -rf "$(BMAPDIR)";					\
	for map in $(MAPS) ; do					\
		s=`echo $$map | cut -d : -f 1`;			\
		d=`echo $$map | cut -d : -f 2`;			\
		src="$(MAPSDIR)/i386/$${s}.map.gz";		\
		dest="$(BMAPDIR)/$${d}.bmap";	\
		destdir=`dirname "$$dest"`;			\
		mkdir -p "$$destdir";				\
		echo "$$d";					\
		loadkeys -b "$$src" > "$$dest" || exit 1;	\
	done

$(TARBALL): $(BMAPDIR)
	rm -rf $(OUTDIR) &&  mkdir -p $(OUTDIR)
	cp -r $(DISTFILES) $(BMAPDIR) $(OUTDIR)
	tar -czf $@ $(OUTDIR)

clean:
	rm -fr $(OUTDIR) $(BMAPDIR) $(TARBALL)


install: $(BMAPDIR)
	mkdir -p "$(DESTDIR)/$(sharedir)"
	cp -r $(BMAPDIR) "$(DESTDIR)/$(sharedir)/"

.PHONY: clean tar gen

MAPSDIR=/usr/share/keymaps
MAPS =	azerty/azerty:fr/azerty\
	azerty/be-latin1:be/be-latin1\
	azerty/fr-latin1:fr/fr-latin1\
	azerty/fr-latin9:fr/fr-latin9\
	azerty/fr-pc:fr/fr-pc\
	azerty/fr:fr/fr\
	azerty/wangbe:wangbe/wangbe\
	azerty/wangbe2:wangbe/wangbe2\
	dvorak/ANSI-dvorak:dvorak/ANSI-dvorak\
	dvorak/dvorak-l:dvorak/dvorak-l\
	dvorak/dvorak-r:dvorak/dvorak-r\
	dvorak/dvorak:dvorak/dvorak\
	fgGIod/tr_f-latin5:tr/tr_f-latin5\
	fgGIod/trf:tr/trf\
	qwerty/bg-cp1251:bg/bg-cp1251\
	qwerty/bg-cp855:bg/bg-cp855\
	qwerty/bg_bds-cp1251:bg/bg_bds-cp1251\
	qwerty/bg_bds-utf8:bg/bg_bds-utf8\
	qwerty/bg_pho-cp1251:bg/bg_pho-cp1251\
	qwerty/bg_pho-utf8:bg/bg_pho-utf8\
	qwerty/br-abnt:br/br-abnt\
	qwerty/br-abnt2:br/br-abnt2\
	qwerty/br-latin1-abnt2:br/br-latin1-abnt2\
	qwerty/br-latin1-us:br/br-latin1-us\
	qwerty/by:by/by\
	qwerty/cf:cf/cf\
	qwerty/cz-cp1250:cz/cz-cp1250\
	qwerty/cz-lat2-prog:cz/cz-lat2-prog\
	qwerty/cz-lat2:cz/cz-lat2\
	qwerty/cz:cz/cz\
	qwerty/dk-latin1:dk/dk-latin1\
	qwerty/es-cp850:es/es-cp850\
	qwerty/es:es/es\
	qwerty/et-nodeadkeys:et/et-nodeadkeys\
	qwerty/et:et/et\
	qwerty/fi-latin1:fi/fi-latin1\
	qwerty/gr-pc:gr/gr-pc\
	qwerty/gr:gr/gr\
	qwerty/hu101:hu/hu101\
	qwerty/il-heb:il/il-heb\
	qwerty/il-phonetic:il/il-phonetic\
	qwerty/il:il/il\
	qwerty/is-latin1-us:is/is-latin1-us\
	qwerty/is-latin1:is/is-latin1\
	qwerty/it-ibm:it/it-ibm\
	qwerty/it:it/it\
	qwerty/it2:it/it2\
	qwerty/jp106:jp/jp106\
	qwerty/la-latin1:la/la-latin1\
	qwerty/lt.baltic:lt/lt.baltic\
	qwerty/lt.l4:lt/lt.l4\
	qwerty/lt:lt/lt\
	qwerty/mk-cp1251:mk/mk-cp1251\
	qwerty/mk-utf:mk/mk-utf\
	qwerty/mk:mk/mk\
	qwerty/mk0:mk/mk0\
	qwerty/nl:nl/nl\
	qwerty/nl2:nl/nl2\
	qwerty/no-latin1:no/no-latin1\
	qwerty/pc110:pc/pc110\
	qwerty/pl:pl/pl\
	qwerty/pl1:pl/pl1\
	qwerty/pl2:pl/pl2\
	qwerty/pl3:pl/pl3\
	qwerty/pl4:pl/pl4\
	qwerty/pt-latin1:pt/pt-latin1\
	qwerty/pt-latin9:pt/pt-latin9\
	qwerty/ro_win:ro/ro_win\
	qwerty/ru_win:ru/ru_win\
	qwerty/se-fi-ir209:se/se-fi-ir209\
	qwerty/se-fi-lat6:se/se-fi-lat6\
	qwerty/se-ir209:se/se-ir209\
	qwerty/se-lat6:se/se-lat6\
	qwerty/sk-prog-qwerty:sk/sk-prog-qwerty\
	qwerty/sk-qwerty:sk/sk-qwerty\
	qwerty/sr-cy:sr/sr-cy\
	qwerty/sv-latin1:sv/sv-latin1\
	qwerty/tr_q-latin5:tr/tr_q-latin5\
	qwerty/tralt:tr/tralt\
	qwerty/trq:tr/trq\
	qwerty/ua-utf-ws:ua/ua-utf-ws\
	qwerty/ua-utf:ua/ua-utf\
	qwerty/ua-ws:ua/ua-ws\
	qwerty/ua:ua/ua\
	qwerty/uk:uk/uk\
	qwerty/us-acentos:us/us-acentos\
	qwerty/us:us/us\
	qwertz/croat:croat/croat\
	qwertz/cz-us-qwertz:cz/cz-us-qwertz\
	qwertz/cz:cz/cz-qwertz\
	qwertz/de-latin1-nodeadkeys:de/de-latin1-nodeadkeys\
	qwertz/de-latin1:de/de-latin1\
	qwertz/de:de/de\
	qwertz/de_CH-latin1:de/de_CH-latin1\
	qwertz/fr_CH-latin1:fr/fr_CH-latin1\
	qwertz/fr_CH:fr/fr_CH\
	qwertz/hu:hu/hu\
	qwertz/sg-latin1-lk450:sg/sg-latin1-lk450\
	qwertz/sg-latin1:sg/sg-latin1\
	qwertz/sg:sg/sg\
	qwertz/sk-prog-qwertz:sk/sk-prog-qwertz\
	qwertz/sk-qwertz:sk/sk-qwertz\
	qwertz/slovene:slovene/slovene
