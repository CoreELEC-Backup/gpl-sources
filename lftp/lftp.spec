%define version 4.8.4
%define release 1
%define use_modules 0

Summary: Sophisticated CLI file transfer program
Name: lftp
Version: %{version}
Release: %{release}
URL: http://lftp.yar.ru/
Source: http://lftp.yar.ru/ftp/lftp-%{version}.tar.gz
Group: Applications/Internet
BuildRoot: %{_tmppath}/%{name}-buildroot
BuildRequires: readline-devel zlib-devel ncurses-devel expat-devel gnutls-devel
License: GNU GPL
#Packager: Manoj Kasichainula <manojk+rpm@io.com>

%description
lftp is CLI file transfer program. It supports FTP, HTTP, FISH and
SFTP protocols, has lots of features including mirror. It was designed
with reliability in mind. GNU Readline library is used for input.
There is also support for secure variants of FTP and HTTP.

%prep
%setup

%build

# Make sure that all message catalogs are built
if [ "$LINGUAS" ]; then
    unset LINGUAS
fi

%if %use_modules
    %configure --with-modules
%else
    %configure
%endif
make DESTDIR=%{buildroot}

%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}
rm -f %{buildroot}%{_libdir}/*.{so,la,a}

%clean
rm -rf %{buildroot}

%files
%defattr(644 root root 755)
%doc ABOUT-NLS BUGS COPYING FAQ FEATURES NEWS README* THANKS TODO
%config /etc/lftp.conf
%attr(755 root root) %{_bindir}/*
%if %use_modules
%{_libdir}/lftp/*/*.so
%{_libdir}/*.so.*
%endif
%{_mandir}/man*/*
%attr(755 root root) %{_datadir}/lftp/*
%{_datadir}/locale/*/*/*
%{_datadir}/applications/*
%{_datadir}/icons/*/*/*/*
