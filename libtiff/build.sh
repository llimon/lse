#!/bin/bash
# This is a buildpkg build.sh script
# build.sh helper functions
. ${BUILDPKG_SCRIPTS}/build.sh.functions
#
###########################################################
# Check the following 4 variables before running the script
topdir=tiff
version=4.0.8
version_major="${version%.*}"

pkgver=1

# https://download.osgeo.org/libtiff/tiff-4.0.8.tar.gz
source[0]=https://download.osgeo.org/libtiff/${topdir}-${version}.tar.gz
# If there are no patches, simply comment this
#patch[0]=

# Source function library
. ${BUILDPKG_SCRIPTS}/buildpkg.functions


# Global settings
topsrcdir=${topdir}-${version}
configure_args+=(
  --disable-shared
)
make_build_opts=( CC="${CC:-gcc}  -include ${prefix}/include/lsecompat.h" )


##
## Override or extend globals
export CFLAGS="$CFLAGS -fPIC"
export CXXFLAGS="$CXXFLAGS -fPIC"
export LIBS="$LIBS -llsecompat"

reg prep
prep()
{
    generic_prep
    setdir source
    ${__gsed} -i 's|^#! /bin/sh|#!/usr/local/lse/bin/bash|' configure
    #${__gsed} -i 's|print -r --|printf "%s\\n"|g' libtool
    

}

reg build
build()
{
    SHELL=/usr/local/lse/bin/bash generic_build
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
    doc COPYRIGHT README VERSION TODO
    find ${stagedir} \( -name '*.so' -o -name '*.so.*' -o -name '*.la' \) -exec rm {} \;
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
