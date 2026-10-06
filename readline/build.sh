#!/bin/bash
# This is a buildpkg build.sh script
# build.sh helper functions
. ${BUILDPKG_SCRIPTS}/build.sh.functions
#
###########################################################
# Check the following 4 variables before running the script
topdir=readline
version=5.2
pkgver=1
source[0]=ftp://ftp.sunet.se/pub/gnu/readline/$topdir-$version.tar.gz
# If there are no patches, simply comment this
#patch[0]=mbrtowc.patch

# Source function library
. ${BUILDPKG_SCRIPTS}/buildpkg.functions

export LDFLAGS="$LDFLAGS -lw -llsecompat"
export LIBS="$LIBS -lw"
#configure_args+=(--disable-static)

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
    ${__rm} -f ${stagedir}${prefix}/${_docdir}/readline/*
    ${__rmdir} ${stagedir}${prefix}/${_docdir}/readline
    ${__rmdir} ${stagedir}${prefix}/${_bindir}
    doc NEWS CHANGES COPYING
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
