#!/bin/bash
# This is a buildpkg build.sh script
# build.sh helper functions
. ${BUILDPKG_SCRIPTS}/build.sh.functions
#
###########################################################
# Check the following 4 variables before running the script
topdir=findutils
version=4.4.2
pkgver=2
source[0]=ftp://ftp.sunet.se/pub/gnu/findutils/$topdir-$version.tar.gz
# If there are no patches, simply comment this
#patch[0]=

# Source function library
. ${BUILDPKG_SCRIPTS}/buildpkg.functions

# Global settings
export CPPFLAGS="-I$prefix/include -Dwint_t=int"
export LIBS="-lposix4 -llsecompat -lw"
ac_overrides="am_cv_func_mbstate_t=no 
ac_cv_header_wctype_h=no 
ac_cv_header_wchar_h=no 
gl_cv_func_btowc=no 
gl_cv_func_mbrtowc=no 
l_cv_func_mbsrtowcs=no 
gl_cv_func_wcrtomb=no 
gl_cv_func_wctob=
no gl_cv_func_mbsinit=no"

gnu_link find locate oldfind updatedb xargs

make_build_opts=( CPPFLAGS="$CPPFLAGS -include $prefix/include/lsecompat.h -include $prefix/include/lse/mbstowcs_compat.h -i $prefix/include/lse/wcsstr_compat.h" )

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
    ${__rmdir} ${stagedir}${prefix}/var
    ${__rmdir} ${stagedir}${prefix}/${_libdir}
    doc AUTHORS COPYING NEWS README THANKS TODO
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
