#!/bin/bash
# This is a buildpkg build.sh script
# build.sh helper functions
. ${BUILDPKG_SCRIPTS}/build.sh.functions
#
###########################################################
# Check the following 4 variables before running the script
topdir=lzip
version=1.20
pkgver=2
source[0]=http://download.savannah.gnu.org/releases/lzip/$topdir-$version.tar.gz
# If there are no patches, simply comment this
#patch[0]=lzip-1.20-no-stdint_h.patch
#patch[0]=snprintf.patch
patch[0]=libs-build-patch

# Source function library
. ${BUILDPKG_SCRIPTS}/buildpkg.functions

# Global settings
#CXXFLAGS="-D_GLIBCXX_USE_C99 -D_GLIBCXX_USE_C99_CHECK -I/usr/local/lse/include/c++/4.1.2"
export CXXFLAGS="-I$prefix/include/c++/4.1.2 -include $prefix/include/compat/snprintf_compat.h"
export LIBS="$LIBS -llsecompat -lpthread"
configure_args+=(CXXFLAGS="$CXXFLAGS" LDFLAGS="$LDFLAGS" LIBS="$LIBS")

reg prep
prep()
{
    generic_prep
}

reg build
build()
{
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
    doc NEWS README AUTHORS COPYING
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
