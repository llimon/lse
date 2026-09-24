#!/bin/bash
# This is a buildpkg build.sh script
# build.sh helper functions
. ${BUILDPKG_SCRIPTS}/build.sh.functions
#
###########################################################
# Check the following 4 variables before running the script
topdir=libpng
version=1.2.59
version_major="${version%.*}"

pkgver=1

# https://ftp-osl.osuosl.org/pub/libpng/src/libpng12/libpng-1.2.59.tar.xz
source[0]=https://ftp-osl.osuosl.org/pub/libpng/src/libpng12/${topdir}-${version}.tar.xz
# If there are no patches, simply comment this
#patch[0]=

# Source function library
. ${BUILDPKG_SCRIPTS}/buildpkg.functions

# Redefin package global attributes
pkgedby="Luis E Limon"
pkgdirdesig="tgcsware"
pkgprefix="TGCS"
email="9660709+llimon@users.noreply.github.com"

# Global settings
export CPPFLAGS="-I$prefix/include"
export LDFLAGS="-L$prefix/lib -R$prefix/lib"
topsrcdir=${topdir}-${version}
#configure_args+=()

reg prep
prep()
{
    generic_prep
    setdir source
    ${__gsed} -i 's|^#! /bin/sh|#!/usr/local/lse/bin/bash|' configure
    # Build with libidn2 instead of libidn
    #${__gsed} -i 's/idna.h/idn2.h/' configure WWW/Library/Implementation/HTParse.c
    #${__gsed} -i '/idn-free.h/d' configure WWW/Library/Implementation/HTParse.c
    #${__gsed} -i 's/-lidn/-lidn2/' configure

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
    doc COPYING README AUTHORS
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
