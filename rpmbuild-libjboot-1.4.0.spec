#################################
# Spec file for JBoot framework #
#################################

Summary: JBoot library to build application
Name: libjboot
Version: 1.4.0
Release: 1
License: Freeware
URL: http://www.sven-lukas.de
Group: System
Packager: Sven Lukas
BuildRoot: ./rpmbuild/

%description
JBoot is a framework to build enterprise applications on top of ESL.

%prep
#echo "BUILDROOT = $RPM_BUILD_ROOT"

mkdir -p $RPM_BUILD_ROOT/usr/lib64
cp -a ../../build/jboot/1.4.0/default/architecture/linux-gcc/link-dynamic/libjboot.so $RPM_BUILD_ROOT/usr/lib64/libjboot.so.1.4.0
ln -s libjboot.so.1.4.0 $RPM_BUILD_ROOT/usr/lib64/libjboot.so

exit

%files
%attr(0755, root, root) /usr/lib64/libjboot.so.1.4.0
%attr(0777, root, root) /usr/lib64/libjboot.so

%pre

%post

%postun

%clean

%changelog
* Sun Apr 23 2022 Sven Lukas <sven.lukas@gmail.com>
  - First prebuild RPM
