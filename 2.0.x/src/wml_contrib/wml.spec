Summary:    Website META Language
Name:       wml
Version:    2.0.11
Release:    1
Copyright:  GPL
Group:      Applications/Publishing/HTML
Source:     http://www.engelschall.com/sw/wml/distrib/%{name}-%{version}.tar.gz
Url:        http://www.engelschall.com/sw/wml
Packager:   Christian W. Zuckschwerdt <zany@triq.net>
BuildRoot:  %{_tmppath}/%{name}-buildroot

%description
WML is a free and extensible Webdesigner's off-line HTML generation
toolkit for Unix, distributed under the GNU General Public License
(GPL v2). It is written in ANSI C and Perl 5, build via a GNU Autoconf
based source tree and runs out-of-the-box on all major Unix derivates.
It can be used free of charge both in educational and commercial
environments.

%prep
%setup

%build
# we don't use the configure macro because libdir is special
./configure --prefix=%{_prefix} \
            --bindir=%{_bindir} \
            --libdir=%{_libdir}/%{name} \
            --mandir=%{_mandir}
make

%install
rm -rf $RPM_BUILD_ROOT
make prefix=$RPM_BUILD_ROOT/%{_prefix} \
     bindir=$RPM_BUILD_ROOT/%{_bindir} \
     libdir=$RPM_BUILD_ROOT/%{_libdir}/%{name} \
     mandir=$RPM_BUILD_ROOT/%{_mandir} \
     install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%doc ANNOUNCE BUGREPORT ChangeLog
%doc COPYING COPYRIGHT COPYRIGHT.OTHER CREDITS
%doc NEWS README README.mp4h SUPPORT VERSION VERSION.HISTORY
%{_bindir}/*
%{_libdir}/%{name}/*
%{_mandir}/man?/*
%{_mandir}/cat?/*
