# Building the libcap/{cap.psx} Go packages, and examples.
#
# Note, we use symlinks to construct a GOPATH friendly src tree. The
# packages themselves are intended to be (ultimately) found via proxy
# as "kernel.org/pub/linux/libs/security/libcap/cap" and
# "kernel.org/pub/linux/libs/security/libcap/psx". However, to
# validate their use on these paths, we fake such a structure in the
# build tree with symlinks.

topdir=$(realpath ..)
include $(topdir)/Make.Rules

GOPATH=$(realpath .)
IMPORTDIR=kernel.org/pub/linux/libs/security/libcap
PKGDIR=pkg/$(GOOSARCH)/$(IMPORTDIR)
PSXGOPACKAGE=$(PKGDIR)/psx.a
CAPGOPACKAGE=$(PKGDIR)/cap.a

DEPS=../libcap/libcap.a ../libcap/libpsx.a

all: $(PSXGOPACKAGE) $(CAPGOPACKAGE) web compare-cap try-launching

$(DEPS):
	make -C ../libcap all

../progs/tcapsh-static:
	make -C ../progs tcapsh-static

src/$(IMPORTDIR)/psx:
	mkdir -p "src/$(IMPORTDIR)"
	ln -s $(topdir)/psx $@

src/$(IMPORTDIR)/cap:
	mkdir -p "src/$(IMPORTDIR)"
	ln -s $(topdir)/cap $@

$(topdir)/libcap/cap_names.h: $(DEPS)
	make -C $(topdir)/libcap all

good-names.go: $(topdir)/libcap/cap_names.h src/$(IMPORTDIR)/cap  mknames.go
	$(GO) run mknames.go --header=$< --textdir=$(topdir)/doc/values | gofmt > $@ || rm -f $@
	diff -u ../cap/names.go $@

$(PSXGOPACKAGE): src/$(IMPORTDIR)/psx ../psx/*.go $(DEPS)
	mkdir -p pkg
	GO111MODULE=off CGO_LDFLAGS_ALLOW="$(CGO_LDFLAGS_ALLOW)" GOPATH="$(GOPATH)" $(GO) install $(IMPORTDIR)/psx

$(CAPGOPACKAGE): src/$(IMPORTDIR)/cap ../cap/*.go good-names.go $(PSXGOPACKAGE)
	GO111MODULE=off CGO_LDFLAGS_ALLOW="$(CGO_LDFLAGS_ALLOW)" GOPATH=$(GOPATH) $(GO) install $(IMPORTDIR)/cap

# Compiles something with this package to compare it to libcap. This
# tests more when run under sudotest (see ../progs/quicktest.sh for that).
compare-cap: compare-cap.go $(CAPGOPACKAGE)
	GO111MODULE=off CGO_LDFLAGS_ALLOW="$(CGO_LDFLAGS_ALLOW)" CGO_CFLAGS="$(CGO_CFLAGS)" CGO_LDFLAGS="$(CGO_LDFLAGS)" GOPATH=$(GOPATH) $(GO) build $<

web: ../goapps/web/web.go $(CAPGOPACKAGE)
	GO111MODULE=off CGO_ENABLED="$(CGO_REQUIRED)" CGO_LDFLAGS_ALLOW="$(CGO_LDFLAGS_ALLOW)" GOPATH=$(GOPATH) $(GO) build -o $@ $(GOBUILDTAG) $<
ifeq ($(RAISE_GO_FILECAP),yes)
	make -C ../progs setcap
	sudo ../progs/setcap cap_setpcap,cap_net_bind_service=p web
	@echo "NOTE: RAISED cap_setpcap,cap_net_bind_service ON web binary"
endif

ok: ok.go
	GO111MODULE=off CGO_ENABLED=0 GOPATH=$(GOPATH) $(GO) build $<

try-launching: try-launching.go $(CAPGOPACKAGE) ok
	GO111MODULE=off CGO_ENABLED="$(CGO_REQUIRED)" CGO_LDFLAGS_ALLOW="$(CGO_LDFLAGS_ALLOW)" GOPATH=$(GOPATH) $(GO) build $(GOBUILDTAG) $<
ifeq ($(CGO_REQUIRED),0)
	GO111MODULE=off CGO_ENABLED="1" CGO_LDFLAGS_ALLOW="$(CGO_LDFLAGS_ALLOW)" GOPATH=$(GOPATH) $(GO) build -o $@-cgo $<
endif

test: all
	GO111MODULE=off CGO_LDFLAGS_ALLOW="$(CGO_LDFLAGS_ALLOW)" GOPATH="$(GOPATH)" $(GO) test $(IMPORTDIR)/psx
	GO111MODULE=off CGO_LDFLAGS_ALLOW="$(CGO_LDFLAGS_ALLOW)" GOPATH="$(GOPATH)" $(GO) test $(IMPORTDIR)/cap
	LD_LIBRARY_PATH=../libcap ./compare-cap

sudotest: test ../progs/tcapsh-static
	./try-launching
ifeq ($(CGO_REQUIRED),0)
	./try-launching-cgo
endif
	sudo ./try-launching
ifeq ($(CGO_REQUIRED),0)
	sudo ./try-launching-cgo
endif

install: all
	rm -rf $(FAKEROOT)$(GOPKGDIR)/$(IMPORTDIR)/psx
	mkdir -p $(FAKEROOT)$(GOPKGDIR)/$(IMPORTDIR)/psx/include/sys
	for x in src/$(IMPORTDIR)/psx/* ; do if [ -d $$x ]; then continue; fi; install -m 0644 $$x $(FAKEROOT)$(GOPKGDIR)/$(IMPORTDIR)/psx; done
	install -m 0644 src/$(IMPORTDIR)/psx/include/sys/psx_syscall.h $(FAKEROOT)$(GOPKGDIR)/$(IMPORTDIR)/psx/include/sys/psx_syscall.h
	mkdir -p $(FAKEROOT)$(GOPKGDIR)/$(IMPORTDIR)/cap
	rm -rf $(FAKEROOT)$(GOPKGDIR)/$(IMPORTDIR)/cap/*
	install -m 0644 src/$(IMPORTDIR)/cap/* $(FAKEROOT)$(GOPKGDIR)/$(IMPORTDIR)/cap

clean:
	rm -f *.o *.so *~ mknames web ok good-names.go
	rm -f compare-cap try-launching try-launching-cgo
	rm -f $(topdir)/cap/*~ $(topdir)/psx/*~
	rm -fr pkg src
