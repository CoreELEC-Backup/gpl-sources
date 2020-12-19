#
# Makefile for libcap
#
topdir=$(shell pwd)
include Make.Rules

#
# flags
#

all install clean kdebug: %: %-here
	$(MAKE) -C libcap $@
ifneq ($(PAM_CAP),no)
	$(MAKE) -C pam_cap $@
endif
ifeq ($(GOLANG),yes)
	$(MAKE) -C go $@
	rm -f cap/go.sum
endif
	$(MAKE) -C tests $@
	$(MAKE) -C progs $@
	$(MAKE) -C doc $@
	$(MAKE) -C kdebug $@

all-here:

install-here:

clean-here:
	$(LOCALCLEAN)

distclean: clean
	$(DISTCLEAN)
	@echo "CONFIRM Go package cap has right version dependency on cap/psx:"
	for x in $$(find . -name go.mod); do grep -F -v "module" $$x | fgrep "kernel.org/pub/linux/libs/security/libcap" > /dev/null || continue ; grep -F "v$(GOMAJOR).$(VERSION).$(MINOR)" $$x  > /dev/null && continue ; echo "$$x is not updated to v$(GOMAJOR).$(VERSION).$(MINOR)" ; exit 1 ; done
	@echo "ALL go.mod files updated"
	@echo "Now validate that everything is checked in to a clean tree.."
	test -z "$$(git status -s)"
	@echo "All good!"

release: distclean
	cd .. && ln -s libcap libcap-$(VERSION).$(MINOR) && tar cvf libcap-$(VERSION).$(MINOR).tar --exclude patches libcap-$(VERSION).$(MINOR)/* && rm libcap-$(VERSION).$(MINOR)

test: all
	make -C libcap $@
	make -C tests $@
ifneq ($(PAM_CAP),no)
	$(MAKE) -C pam_cap $@
endif
ifeq ($(GOLANG),yes)
	make -C go $@
endif
	make -C progs $@

sudotest: all
	make -C tests $@
ifneq ($(PAM_CAP),no)
	$(MAKE) -C pam_cap $@
endif
ifeq ($(GOLANG),yes)
	make -C go $@
endif
	make -C progs $@

distcheck:
	./distcheck.sh
	make DYNAMIC=yes clean all test sudotest
	make CC=/usr/local/musl/bin/musl-gcc clean all test sudotest
	make clean all test sudotest
	make distclean

morgangodoc:
	@echo "Now the release is made, you want to remember to run:"
	@echo
	@echo "GOPROXY=https://proxy.golang.org GO111MODULE=on go get kernel.org/pub/linux/libs/security/libcap/cap@v$(GOMAJOR).$(VERSION).$(MINOR)"
	@echo
	@echo "This will cause a go.dev documentation update."

morganrelease: distcheck
	@echo "sign the main library tag twice: older DSA key; and newer RSA (kernel.org) key"
	git tag -u D41A6DF2 -s libcap-$(VERSION).$(MINOR) -m "This is libcap-$(VERSION).$(MINOR)"
	git tag -u E2CCF3F4 -s libcap-korg-$(VERSION).$(MINOR) -m "This is libcap-$(VERSION).$(MINOR)"
	@echo "The following are for the Go module tracking."
	git tag -u D41A6DF2 -s v$(GOMAJOR).$(VERSION).$(MINOR) -m "This is the version tag for the 'libcap' Go base directory associated with libcap-$(VERSION).$(MINOR)."
	git tag -u D41A6DF2 -s psx/v$(GOMAJOR).$(VERSION).$(MINOR) -m "This is the version tag for the 'psx' Go package associated with libcap-$(VERSION).$(MINOR)."
	git tag -u D41A6DF2 -s cap/v$(GOMAJOR).$(VERSION).$(MINOR) -m "This is the version tag for the 'cap' Go package associated with libcap-$(VERSION).$(MINOR)."
	make release
	@echo "sign the tar file using korg key"
	cd .. && gpg -sba -u E2CCF3F4 libcap-$(VERSION).$(MINOR).tar
	make morgangodoc
