#!/bin/bash
# This is a buildpkg build.sh script
# build.sh helper functions
. ${BUILDPKG_SCRIPTS}/build.sh.functions
#
###########################################################
# Check the following 4 variables before running the script
topdir=tar
version=1.30
pkgver=1
source[0]=ftp://ftp.sunet.se/pub/gnu/tar/$topdir-$version.tar.bz2
# If there are no patches, simply comment this
#patch[0]=getprogname.patch
#patch[1]=ftello-fix.patch

# Source function library
. ${BUILDPKG_SCRIPTS}/buildpkg.functions


# solaris 2.5.1 does not have ftello
#configure_args+=(--disable-largefile)
LIBS="$LIBS -llsecompat"
make_build_opts=( _pls "CPPFLAGS=\"\$CPPFLAGS -include $prefix/include/lsecompat.h\" ${make_build_opts}" )

gnu_link tar

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
    doc ChangeLog README NEWS COPYING
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
