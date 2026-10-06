#!/bin/bash
# This is a buildpkg build.sh script
# build.sh helper functions
. ${BUILDPKG_SCRIPTS}/build.sh.functions
#
###########################################################
# Check the following 4 variables before running the script
topdir=joe
version=4.8
pkgver=1
source[0]=https://sourceforge.net/projects/joe-editor/files/JOE%20sources/joe-4.8/joe-4.8.tar.gz
# If there are no patches, simply comment this

# Source function library
. ${BUILDPKG_SCRIPTS}/buildpkg.functions


# solaris 2.5.1 does not have ftello
#configure_args+=(--disable-largefile)
LIBS="$LIBS -llsecompat"
CPPFLAGS="$CPPFLAGS -include $prefix/include/lse/wchar_compat.h"
#make_build_opts=( CPPFLAGS="$CPPFLAGS -include $prefix/include/lsecompat.h" )

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
