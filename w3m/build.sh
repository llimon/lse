#!/bin/bash
# This is a buildpkg build.sh script
# build.sh helper functions
. ${BUILDPKG_SCRIPTS}/build.sh.functions
#
###########################################################
# Check the following 4 variables before running the script
topdir=w3m
version=0.5.3-git20210102-deb11u1
pkgver=2

source[0]=https://github.com/tats/w3m/archive/refs/tags/v0.5.3+git20210102+deb11u1.tar.gz
# If there are no patches, simply comment this
#patch[0]=

# Source function library
. ${BUILDPKG_SCRIPTS}/buildpkg.functions

# Global settings
LIBS="$LIBS -llsecompat -llsenet -lpthread -lthread"
topsrcdir=${topdir}-${version}
configure_args+=(--with-ssl --enable-image )
#--with-imagelib=imlib2)

reg prep
prep()
{
    generic_prep
    setdir source
    ${__gsed} -i 's|^#! /bin/sh|#/usr/local/lse/bin/bash|' configure
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
#    doc COPYHEADER COPYING CHANGES README AUTHORS
    doc NEWS TODO ChangeLog doc/README
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
