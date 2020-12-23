# RPM package for Enca, an Extremely Naive Charset Analyser.
#
# This RPM package suports conditional builds. Components can be
# enabled/disabled in rpmbuild via --with/--without. Default is:
#
#   rpmbuild -ba --without htmldocs --with static
#
# Check http://www.rpm.org/wiki/PackagerDocs/ConditionalBuilds for more info.

# add --with htmldocs option, i.e. disable generation of HTML documentation by
# default
%bcond_with htmldocs

# add --without static, i.e. enable generation of static libraries by
# default
%bcond_without static

Summary:        Detect encoding of text files and convert to other encodings.
Name:           enca
Version:        1.19
Release:        1%{?dist}
License:        GPLv2
Group:          Applications/Text
Source:         http://dl.cihar.com/enca/enca-1.19.tar.gz
URL:            https://github.com/nijel/enca
Prefix:         %{_prefix}
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

%description
Enca is an Extremely Naive Charset Analyser.

It detects character set and encoding of text files and can also convert them
to other encodings using either a built-in converter or external libraries and
tools like GNU recode (librecode), UNIX98 iconv (libiconv), perl Unicode::Map
and cstocs.

Currently, it has support for Belarusian, Bulgarian, Croatian, Czech,
Estonian, Latvian, Lithuanian, Polish, Russian, Slovak, Slovene, Ukrainian,
Chinese, and some multibyte encodings (mostly variants of Unicode) independent
on the language.

This package contains enca, a command line frontend, as well as the shared
Enca libraries other programs can make use of.

Install enca if you need to cope with text files of dubious origin and unknown
encoding and convert them to some reasonable encoding.

%package        devel
Summary:        Header files and libraries for Enca development.
Group:          Development/Libraries
Requires:       %{name} = %{version}-%{release}
Requires:       pkgconfig

%description devel
The %{name}-devel package contains the static libraries, header files and
documentation for writing programs using Enca, the Extremely Naive Charset
Analyser.

Install %{name}-devel if you are going to create applications using the Enca
library.

%prep
%setup -q

%build
%configure \
    --disable-dependency-tracking \
    --without-librecode \
    %{?_without_htmldocs:--disable-gtk-doc} \
    %{?_without_static:--disable-static}

%__make %{?_smp_mflags}

%check
#make check

%install
[ "%{buildroot}" != "/" ] && %__rm -rf %{buildroot}
%make_install

%__mv %{buildroot}/%{_datadir}/gtk-doc/ gtk-doc

%if %{without static}
%__rm -f %{buildroot}/%{_libdir}/libenca.a
%__rm -f %{buildroot}/%{_libdir}/libenca.la
%endif

%clean
[ "%{buildroot}" != "/" ] && %__rm -rf %{buildroot}

%files
%defattr(755,root,root)
%{_bindir}/enca
%{_bindir}/enconv
%{_libdir}/libenca.so*

# external converters
%{_libexecdir}/%{name}/extconv/*
%dir %{_libexecdir}/%{name}/extconv
%dir %{_libexecdir}/%{name}

# docs and man pages
%defattr(644,root,root)
%doc AUTHORS ChangeLog COPYING FAQ README THANKS TODO
%doc %{_mandir}/man1/enca.1*
%doc %{_mandir}/man1/enconv.1*

%files devel
%defattr(-,root,root)
%{_includedir}/enca.h
%{_libdir}/pkgconfig/enca.pc

# static libs
%if %{with static}
%{_libdir}/libenca.a
%{_libdir}/libenca.la
%endif

# README and HTML docs
%doc DEVELOP.md
%if %{with htmldocs}
%doc gtk-doc/html
%endif

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%changelog
* Sat Jan 10 2015 Victor Foitzik (vifo) <vifo@cpan.org>
- updated spec to match information on Github and
  align with recent packaging guidelines from Fedora
- added conditional builds for HTML documentation and
  static libraries
* Sun Dec 18 2005 David Necas (Yeti) <yeti@physics.muni.cz>
- fixed 0644 permissions on doc directory
* Mon May 17 2004 David Necas (Yeti) <yeti@physics.muni.cz>
- doubled percents in changelog
* Mon Dec 22 2003 David Necas (Yeti) <yeti@physics.muni.cz>
- moved wrappers to libexec
* Thu Nov  6 2003 David Necas (Yeti) <yeti@physics.muni.cz>
- added b-piconv
- fixed HTML doc install paths
* Tue Oct 14 2003 David Necas (Yeti) <yeti@physics.muni.cz>
- testing whether $RPM_BUILD_ROOT is not /
- updated for new HTML doc location
- changed make -> %%__make, rm -> %%__rm
* Sat Aug  2 2003 David Necas (Yeti) <yeti@physics.muni.cz>
- cleaning $RPM_BUILD_ROOT in %%install
* Sat Jun 28 2003 David Necas (Yeti) <yeti@physics.muni.cz>
- removed --disable-gtk-doc, no longer needed
* Fri Jun 20 2003 David Necas (Yeti) <yeti@physics.muni.cz>
- added enca.pc to devel package
* Sat Jun 14 2003 David Necas (Yeti) <yeti@physics.muni.cz>
- updated description
- added --disable-gtk-doc
* Mon Dec 23 2002 David Necas (Yeti) <yeti@physics.muni.cz>
- added libenca.so
* Fri Dec 20 2002 David Necas (Yeti) <yeti@physics.muni.cz>
- fixed URL and Source to trific.ath.cx
* Mon Oct 21 2002 David Necas (Yeti) <yeti@physics.muni.cz>
- added FAQ to docs
* Thu Oct 10 2002 David Necas (Yeti) <yeti@physics.muni.cz>
- removed twice-listed %%{docdir}/html
* Sat Sep 21 2002 David Necas (Yeti) <yeti@physics.muni.cz>
- added b-umap
* Sun Sep 15 2002 David Necas (Yeti) <yeti@physics.muni.cz>
- added enconv
* Thu Aug 29 2002 David Necas (Yeti) <yeti@physics.muni.cz>
- removed bzip2-devel buildprereq
* Sat Aug 24 2002 David Necas (Yeti) <yeti@physics.muni.cz>
- added postinstall and postuninstall scriptlets
* Wed Aug 21 2002 David Necas (Yeti) <yeti@physics.muni.cz>
- updated to enca-0.10.0-pre2
- added libenca
- split into enca and enca-devel
- removed cache
- fixed HTML_DIR
* Tue Jul 10 2001 David Necas (Yeti) <yeti@physics.muni.cz>
- changed rpm macros in Source and URL to autoconf macros to ease debian/
  stuff generation
* Sun May 20 2001 David Necas (Yeti) <yeti@physics.muni.cz>
- added BuildPrereq: bzip2-devel
* Wed May  2 2001 David Necas (Yeti) <yeti@physics.muni.cz>
- changed group to standard (but much less appropriate) Applications/Text
- rpm macros are used instead of autoconf macros (after the first definition)
* Sun Mar 11 2001 David Necas (Yeti) <yeti@physics.muni.cz>
- added defattr, doc attributes
- uses global configure cache
- heavy use of predefined directories
- configure moved to build section as is usual
* Sun Feb 25 2001 David Necas (Yeti) <yeti@physics.muni.cz>
- updated to enca-0.9.0pre4 (including files and descriptions)
- added sed dependency
* Sun Oct 25 2000 David Necas (Yeti) <yeti@physics.muni.cz>
- updated to enca-0.7.5
* Sun Oct 11 2000 David Necas (Yeti) <yeti@physics.muni.cz>
- removed redundant Provides: enca
* Sun Oct  1 2000 David Necas (Yeti) <yeti@physics.muni.cz>
- updated to enca-0.7.1
- man page forced to be intstalled to ${prefix}/share/man
* Tue Sep 26 2000 David Necas (Yeti) <yeti@physics.muni.cz>
- updated to enca-0.7.0
- spec autogenerated by configure
* Tue Sep 19 2000 David Necas (Yeti) <yeti@physics.muni.cz>
- fixed not installing bcstocs
* Wed Sep 13 2000 David Necas (Yeti) <yeti@physics.muni.cz>
- first packaged (0.6.2)
