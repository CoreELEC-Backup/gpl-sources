NAME=libaio
SPECFILE=$(NAME).spec
VERSION=$(shell awk '/Version:/ { print $$2 }' $(SPECFILE))
TAG = $(NAME)-$(VERSION)
RPMBUILD=$(shell `which rpmbuild >&/dev/null` && echo "rpmbuild" || echo "rpm")

prefix=/usr
includedir=$(prefix)/include
libdir=$(prefix)/lib

default: all

all:
	@$(MAKE) -C src

install:
	@$(MAKE) -C src install prefix=$(DESTDIR)$(prefix) includedir=$(DESTDIR)$(includedir) libdir=$(DESTDIR)$(libdir)

check:
	@$(MAKE) -C harness check

partcheck: all
	@$(MAKE) -C harness partcheck

clean:
	@$(MAKE) -C src clean
	@$(MAKE) -C harness clean

tag-archive:
	@git tag $(TAG)

create-archive:
	@git archive --prefix=$(NAME)-$(VERSION)/ -o $(NAME)-$(VERSION).tar.gz $(TAG)
	@echo "The final archive is ./$(NAME)-$(VERSION).tar.gz."

archive: clean tag-archive create-archive

srpm: create-archive
	$(RPMBUILD) --define "_sourcedir `pwd`" --define "_srcrpmdir `pwd`" --nodeps -bs $(SPECFILE)
