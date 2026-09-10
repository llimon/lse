#!/bin/bash
# This is a buildpkg build.sh script
# build.sh helper functions
. ${BUILDPKG_SCRIPTS}/build.sh.functions
#
###########################################################
# Check the following 4 variables before running the script
topdir=cmake
version=2.8.12.2
pkgver=1
source[0]=https://cmake.org/files/v2.8/$topdir-$version.tar.gz
# If there are no patches, simply comment this
#patch[0]=

# Source function library
. ${BUILDPKG_SCRIPTS}/buildpkg.functions

# Global settings
LD_OPTIONS="$LDFLAGS -lposix4 -lw"
#CXXFLAGS="$CXXFLAGS -fpermissive -D__EXTENSIONS__ -I/usr/tgcware/include -include $prefix/include/compat/usleep_compat.h -DHAVE_WCSLEN=1 -DHAVE_WCSCPY=1 -DHAVE_WCHAR_H=1"
CXXFLAGS="$CXXFLAGS -fpermissive -D__EXTENSIONS__ -I/usr/tgcware/include -include $prefix/include/compat/usleep_compat.h" 
CFLAGS="$CFLAGS -DKWSYS_SHARED_FORWARD_LDPATH=\\\"LD_LIBRARY_PATH\\\""
CXXFLAGS="$CXXFLAGS -DKWSYS_SHARED_FORWARD_LDPATH=\\\"LD_LIBRARY_PATH\\\""
echo "CFLAGS=$CFLAGS"
echo "CXXFLAGS=$CXXFLAGS"
export CFLAGS CXXFLAGS
CC=gcc
CXX="g++"
export LD_OPTIONS LDFLAGS CFLAGS CXXFLAGS CC CXX
configure_args=(--prefix=$prefix --docdir=$_docdir/${topdir}-2.8 --mandir=share)
configure_args+=(--system-curl --system-expat --system-zlib --system-bzip2)
configure_args+=(--parallel=2)
make_check_target=test

reg prep
prep()
{
    generic_prep
}

reg build
build()
{
    setdir source
#    ${__gsed} -i 's/#ifndef HAVE_WCSLEN/#if 0 \/* Solaris 2.5.1 patch *\/ /g' Utilities/cmlibarchive/libarchive/archive_entry.c
#    ${__gsed} -i 's/#ifndef HAVE_WCSCPY/#if 0 \/* Solaris 2.5.1 patch *\/ /g' Utilities/cmlibarchive/libarchive/archive_entry.c
    generic_build
}

reg check
check()
{
    generic_check
}

reg install
install()
{
    generic_install DESTDIR
}

reg pack
pack()
{
    generic_pack
}

reg distclean
distclean()
{
    clean distclean
}

###################################################
# No need to look below here
###################################################
build_sh $*
