%define _unpackaged_files_terminate_build 1

Name: libnemofolderlistmodel
Version: 0.1.0
Release: alt1

Summary: Library that provides remote and local directory and file models for Qt applications.
License: BSD-3-Clause and LGPLv3+ and GPLv3+
Group: Development/C++
Url: https://github.com/august-alt/filemanager-app

BuildRequires: cmake rpm-build
BuildRequires: extra-cmake-modules qt5-declarative-devel qt5-tools-devel libsmbclient-devel

Source0: %name-%version.tar

%description 
Library that provides remote and local directory and file
models for Qt applications.

%package -n %name-devel
Summary: Development files for lib%name.
Group: Development/C++

%description -n %name-devel
Headers for nemodirmodel library.

%prep
%setup -q

%build
%cmake
%cmake_build

%install
cd BUILD
mkdir -p %buildroot/%_libdir/
install -m 644 libnemofolderlistmodel.so %buildroot/%_libdir/

mkdir -p %buildroot/%_includedir/%name
cd %{_builddir}/%name-%version
rm -rf BUILD
find . -name '*.h' -print | grep -v test | grep -v qsambaclient |cpio -pavd %buildroot/%_includedir/%name/

mkdir -p %buildroot/%_pkgconfigdir
cat >"%buildroot/%_pkgconfigdir/%name.pc" <<-__EOF
includedir=%_includedir
libdir=%_libdir

Name: %name
Description: Library that provides remote and local directory and file models for Qt applications.
Version: %version-%release
Cflags: -I%_includedir/%name
Libs: -L%_libdir -l%name
__EOF

%files
%_libdir/libnemofolderlistmodel.so

%files -n %name-devel
%_includedir/%name/*.h
%_includedir/%name/disk/*.h
%_includedir/%name/net/*.h
%_includedir/%name/smb/*.h
%_includedir/%name/trash/*.h
%_pkgconfigdir/%name.pc

%changelog
* Tue Aug 24 2021 Vladimir Rubanov <august@altlinux.org> 0.1.0-alt1
- Initial build