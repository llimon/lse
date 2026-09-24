#!/bin/bash
# This is a buildpkg build.sh script
# build.sh helper functions
. ${BUILDPKG_SCRIPTS}/build.sh.functions
#
###########################################################
# Check the following 4 variables before running the script
topdir=minicom
version=2.7.1
version_major="${version%.*}"

pkgver=1

# https://alioth-archive.debian.org/releases/minicom/Source/2.7.1/minicom-2.7.1.tar.gz
source[0]=https://alioth-archive.debian.org/releases/minicom/Source/${version}/${topdir}-${version}.tar.gz
# If there are no patches, simply comment this
patch[0]=build-getopt.patch

# Source function library
. ${BUILDPKG_SCRIPTS}/buildpkg.functions

# Global settings
export LDFLAGS="$LDFLAGS -lposix4 -llsecompat -lw"
topsrcdir=${topdir}-${version}

reg prep
prep()
{
    generic_prep
    setdir source
    ${__gsed} -i 's|^#! /bin/sh|#!/usr/local/lse/bin/bash|' configure

}

reg build
build()
{
    export CFLAGS="$CFLAGS -std=gnu89"
    ac_overrides="gl_cv_func_getopt_posix=no"
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
