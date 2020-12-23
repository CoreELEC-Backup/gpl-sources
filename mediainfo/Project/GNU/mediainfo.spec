%define mediainfo_version           18.05
%define libmediainfo_version        18.05
%define libzen_version              0.4.37

%if 0%{?fedora_version} || 0%{?centos_version} >= 600 || 0%{?rhel_version} >= 600
%define libmediainfo_name libmediainfo
%define libzen_name libzen
%else
%define libmediainfo_name libmediainfo0
%define libzen_name libzen0
%endif

Name:           mediainfo
Version:        %{mediainfo_version}
Release:        1
Summary:        Most relevant technical and tag data for video and audio files (CLI)

Group:          Applications/Multimedia
License:        BSD-2-Clause
URL:            http://MediaArea.net/MediaInfo
Packager:       MediaArea.net SARL <info@mediaarea.net>
Source0:        %{name}_%{version}.tar.gz

Requires:       %{libzen_name}%{?_isa} >= %{libzen_version}
Requires:       %{libmediainfo_name}%{?_isa} >= %{libmediainfo_version}

Prefix:         %{_prefix}
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root
BuildRequires:  gcc-c++
BuildRequires:  pkgconfig
BuildRequires:  libmediainfo-devel >= %{libmediainfo_version}
BuildRequires:  libzen-devel >= %{libzen_version}
BuildRequires:  pkgconfig
BuildRequires:  zlib-devel
BuildRequires:  libtool
BuildRequires:  automake
BuildRequires:  autoconf

%if 0%{?fedora_version} == 99
BuildRequires: gnu-free-sans-fonts
%endif

%if 0%{?mageia}
BuildRequires:  sane-backends-iscan
BuildRequires:  libuuid-devel
%endif

# wxWidgets package name
%if 0%{?suse_version} && 0%{?suse_version} >= 1140
BuildRequires:  wxWidgets-devel
%else
%if 0%{?mageia}
BuildRequires:  wxgtk2.8-devel
%else
BuildRequires:  wxGTK-devel
%endif
%endif

%if 0%{?suse_version}
BuildRequires:  update-desktop-files
%endif
%if 0%{?fedora_version}
BuildRequires:  desktop-file-utils
%endif

%description
MediaInfo is a convenient unified display of the most relevant technical
and tag data for video and audio files.

What information can I get from MediaInfo?
* General: title, author, director, album, track number, date, duration...
* Video: codec, aspect, fps, bitrate...
* Audio: codec, sample rate, channels, language, bitrate...
* Text: language of subtitle
* Chapters: number of chapters, list of chapters

DivX, XviD, H263, H.263, H264, x264, ASP, AVC, iTunes, MPEG-1,
MPEG1, MPEG-2, MPEG2, MPEG-4, MPEG4, MP4, M4A, M4V, QuickTime,
RealVideo, RealAudio, RA, RM, MSMPEG4v1, MSMPEG4v2, MSMPEG4v3,
VOB, DVD, WMA, VMW, ASF, 3GP, 3GPP, 3GP2

What format (container) does MediaInfo support?
* Video: MKV, OGM, AVI, DivX, WMV, QuickTime, Real, MPEG-1,
  MPEG-2, MPEG-4, DVD (VOB) (Codecs: DivX, XviD, MSMPEG4, ASP,
  H.264, AVC...)
* Audio: OGG, MP3, WAV, RA, AC3, DTS, AAC, M4A, AU, AIFF
* Subtitles: SRT, SSA, ASS, SAMI

This package includes the command line interface.

%package gui
Summary:    Supplies technical and tag information about a video or audio file (GUI)
Group:      Applications/Multimedia
Requires:   %{libzen_name}%{?_isa} >= %{libzen_version}
Requires:   %{libmediainfo_name}%{?_isa} >= %{libmediainfo_version}

# wxWidgets package name
%if 0%{?suse_version} && 0%{?suse_version} >= 1140
Requires:   wxWidgets
%else
%if 0%{?mageia}
Requires:  wxgtk2.8
%else
Requires:   wxGTK
%endif
%endif

%description gui
MediaInfo is a convenient unified display of the most relevant technical
and tag data for video and audio files.

What information can I get from MediaInfo?
* General: title, author, director, album, track number, date, duration...
* Video: codec, aspect, fps, bitrate...
* Audio: codec, sample rate, channels, language, bitrate...
* Text: language of subtitle
* Chapters: number of chapters, list of chapters

DivX, XviD, H263, H.263, H264, x264, ASP, AVC, iTunes, MPEG-1,
MPEG1, MPEG-2, MPEG2, MPEG-4, MPEG4, MP4, M4A, M4V, QuickTime,
RealVideo, RealAudio, RA, RM, MSMPEG4v1, MSMPEG4v2, MSMPEG4v3,
VOB, DVD, WMA, VMW, ASF, 3GP, 3GPP, 3GP2

What format (container) does MediaInfo support?
* Video: MKV, OGM, AVI, DivX, WMV, QuickTime, Real, MPEG-1,
  MPEG-2, MPEG-4, DVD (VOB) (Codecs: DivX, XviD, MSMPEG4, ASP,
  H.264, AVC...)
* Audio: OGG, MP3, WAV, RA, AC3, DTS, AAC, M4A, AU, AIFF
* Subtitles: SRT, SSA, ASS, SAMI

This package includes the graphical user interface.

%prep
%setup -q -n MediaInfo
sed -i 's/.$//' *.txt *.html Release/*.txt

find . -type f -exec chmod 644 {} ';'

pushd Project/GNU/CLI
    autoreconf -i
popd

pushd Project/GNU/GUI
    autoreconf -i
popd

%build
export CFLAGS="%{optflags}"
export CXXFLAGS="%{optflags}"

# build CLI
pushd Project/GNU/CLI
    %if 0%{?mageia} > 5
        %configure --disable-dependency-tracking
    %else
        %configure
    %endif
    make %{?_smp_mflags}
popd

# now build GUI
pushd Project/GNU/GUI
    %if 0%{?mageia} > 5
        %configure --disable-dependency-tracking
    %else
        %configure
    %endif
    make %{?_smp_mflags}
popd


%install
pushd Project/GNU/CLI
    make install DESTDIR=%{buildroot}
popd

pushd Project/GNU/GUI
    make install DESTDIR=%{buildroot}
popd

%if %{undefined fedora_version} || 0%{?fedora_version} < 26
rm -fr %{buildroot}%{_datadir}/metainfo
install -dm 755 %{buildroot}%{_datadir}/appdata/
install -m 644 Project/GNU/GUI/mediainfo-gui.metainfo.xml %{buildroot}%{_datadir}/appdata/mediainfo-gui.appdata.xml
%endif

%if 0%{?suse_version}
  %suse_update_desktop_file -n mediainfo-gui AudioVideo AudioVideoEditing
  %suse_update_desktop_file -n %{buildroot}%{_datadir}/apps/konqueror/servicemenus/mediainfo-gui.desktop AudioVideo AudioVideoEditing
  %suse_update_desktop_file -n %{buildroot}%{_datadir}/kde4/services/ServiceMenus/mediainfo-gui.desktop AudioVideo AudioVideoEditing
%endif

%files
%defattr(-,root,root,-)
%doc Release/ReadMe_CLI_Linux.txt History_CLI.txt
%if 0%{?fedora_version} || 0%{?centos_version} >= 700 || 0%{?rhel_version} >= 700
%license License.html
%else
%doc License.html
%endif
%{_bindir}/mediainfo

%files gui
%defattr(-,root,root,-)
%doc Release/ReadMe_GUI_Linux.txt History_GUI.txt
%if 0%{?fedora_version} || 0%{?centos_version} >= 700 || 0%{?rhel_version} >= 700
%license License.html
%else
%doc License.html
%endif
%{_bindir}/mediainfo-gui
%{_datadir}/applications/*.desktop
%{_datadir}/pixmaps/*.xpm
%dir %{_datadir}/icons/hicolor
%dir %{_datadir}/icons/hicolor/256x256
%dir %{_datadir}/icons/hicolor/256x256/apps
%{_datadir}/icons/hicolor/256x256/apps/*.png
%dir %{_datadir}/icons/hicolor
%dir %{_datadir}/icons/hicolor/scalable
%dir %{_datadir}/icons/hicolor/scalable/apps
%{_datadir}/icons/hicolor/scalable/apps/*.svg

%dir %{_datadir}/apps
%dir %{_datadir}/apps/konqueror
%dir %{_datadir}/apps/konqueror/servicemenus
%{_datadir}/apps/konqueror/servicemenus/*.desktop
%dir %{_datadir}/kde4
%dir %{_datadir}/kde4/services
%dir %{_datadir}/kde4/services/ServiceMenus
%{_datadir}/kde4/services/ServiceMenus/*.desktop
%dir %{_datadir}/kservices5
%dir %{_datadir}/kservices5/ServiceMenus
%{_datadir}/kservices5/ServiceMenus/*.desktop
%if (%{undefined rhel_version} || 0%{?rhel_version} >= 600) && (%{undefined centos_version} || 0%{?centos_version} >= 600)
%if 0%{?fedora_version} && 0%{?fedora_version} >= 26
%dir %{_datadir}/metainfo
%{_datadir}/metainfo/*.xml
%else
%dir %{_datadir}/appdata
%{_datadir}/appdata/*.xml
%endif
%else
%define _unpackaged_files_terminate_build 0
%endif


%changelog
* Thu Jan 01 2009 MediaArea.net SARL <info@mediaarea.net> - %{mediainfo_version}
- See History.txt for more info and real dates
- Previous packages made by Toni Graffy <toni@links2linux.de>
- Fedora style made by Vasiliy N. Glazov <vascom2@gmail.com>
