#!/bin/bash
# This is a buildpkg build.sh script
# build.sh helper functions
. ${BUILDPKG_SCRIPTS}/build.sh.functions
#
###########################################################
# Check the following 4 variables before running the script
topdir=texinfo
version=4.13
pkgver=1
source[0]=ftp://ftp.sunet.se/pub/gnu/texinfo/$topdir-$version.tar.gz
# If there are no patches, simply comment this
#patch[0]=

# Source function library
. ${BUILDPKG_SCRIPTS}/buildpkg.functions


reg prep
prep()
{
    # has an "a"
    version=4.13a
    generic_prep
}

reg build
build()
{
#    export CPPFLAGS="$CPPFLAGS -include $prefix/include/lse/snprintf_compat.h"
    export LIBS="$LIBS -lw -llsecompat -llsew"

    ac_overides="ac_cv_func_iswalnum=yes \
  ac_cv_func_iswalpha=yes \
  ac_cv_func_iswcntrl=yes \
  ac_cv_func_iswdigit=yes \
  ac_cv_func_iswgraph=yes \
  ac_cv_func_iswlower=yes \
  ac_cv_func_iswprint=yes \
  ac_cv_func_iswpunct=yes \
  ac_cv_func_iswspace=yes \
  ac_cv_func_iswupper=yes \
  ac_cv_func_iswxdigit=yes
  "
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
    doc NEWS ChangeLog AUTHORS TODO COPYING
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
