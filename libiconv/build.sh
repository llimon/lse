#!/bin/bash
# This is a buildpkg build.sh script
# build.sh helper functions
. ${BUILDPKG_SCRIPTS}/build.sh.functions
#
###########################################################
# Check the following 4 variables before running the script
topdir=libiconv
version=1.15
pkgver=2
source[0]=https://mirrors.kernel.org/gnu/$topdir/$topdir-$version.tar.gz
patch[0]=libiconv-getprogname.patch

# Source function library
. ${BUILDPKG_SCRIPTS}/buildpkg.functions
   make_build_opts=( CPPFLAGS="$CPPFLAGS -include $prefix/include/lsecompat.h" )

gnu_link iconv

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
    # Global settings
    export LDFLAGS="$LDFLAGS"
    LDFLAGS="$LDFLAGS -llsecompat" 
    # Should use bash for libtool
    export CONFIG_SHELL=/usr/local/lse/bin/bash
    configure_args+=(--enable-extra-encodings)
    gl_cv_func_getprogname_is_buggy=yes generic_build
}

reg install
install()
{
    generic_install DESTDIR
    doc NOTES ChangeLog DESIGN NEWS COPYING AUTHORS README COPYING.LIB
    ${__rm} -f ${stagedir}${prefix}/${_libdir}/charset.alias
    ${__rm} -f ${stagedir}${prefix}/share/doc/*.html

    # Sadly configure ignores --program-prefix, so we have to fix this manually
    ${__mv} ${stagedir}${prefix}/${_bindir}/iconv ${stagedir}${prefix}/${_bindir}/${gnu_prefix}iconv
    ${__mv} ${stagedir}${prefix}/${_mandir}/man1/iconv.1 ${stagedir}/${prefix}/${_mandir}/man1/giconv.1

    # Compatible with previous package
    compat libiconv 1.12 1 1
    compat libiconv 1.14 1 2
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
