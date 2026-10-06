#!/bin/bash
# This is a buildpkg build.sh script
# build.sh helper functions
. ${BUILDPKG_SCRIPTS}/build.sh.functions
#
###########################################################
# Check the following 4 variables before running the script
topdir=mc
version=4.6.1
version_major="${version%.*}"

pkgver=1

# http://ftp.midnight-commander.org/mc-4.6.1.tar.gz
source[0]=http://ftp.midnight-commander.org/${topdir}-${version}.tar.gz
# If there are no patches, simply comment this
#patch[0]=

# Source function library
. ${BUILDPKG_SCRIPTS}/buildpkg.functions

# Global settings
export LIBS="$LIBS -llsecompat -llsenet"
export CPPFLAGS="$CPPFLAGS -I${prefix}/include/ncurses"
make_build_opts=( CC="${CC:-gcc}  -include ${prefix}/include/lsecompat.h" )

#topsrcdir=${topdir}-${version}
configure_args+=(
   --with-screen=ncurses
   --without-libiconv-prefix 
   --without-libintl-prefix
)

#topsrcdir="${topdir}${version}"

reg prep
prep()
{	
    generic_prep
    setdir source
    sed -i '/#include "x11conn.h"/a \
#include <X11/Xlib.h>' src/x11conn.c
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
	generic_install	
}

reg pack
pack()
{
    generic_pack DESTDIR
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
