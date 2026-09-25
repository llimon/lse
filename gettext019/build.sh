#!/bin/bash
# This is a buildpkg build.sh script
# build.sh helper functions
. ${BUILDPKG_SCRIPTS}/build.sh.functions
#
###########################################################
# Check the following 4 variables before running the script
topdir=gettext
version=0.19.8.1
pkgver=2
source[0]=https://mirrors.kernel.org/gnu/gettext/$topdir-$version.tar.lz
# If there are no patches, simply comment this
#patch[0]=

# Source function library
. ${BUILDPKG_SCRIPTS}/buildpkg.functions

export LIBS="-liconv -llsecompat  -lpthread"
configure_args+=(--with-libiconv-prefix=$prefix --disable-java --disable-native-java --disable-openmp)
make_build_opts=( \
    CXXFLAGS="-I/usr/local/lse/include -O2 -mcpu=v7 -include /usr/local/lse/include/lse/snprintf_compat.h" \
 )

gnu_link autopoint envsubst gettext gettext.sh gettextize msgattrib msgcat msgcmp msgcomm msgconv msgen msgexec msgfilter msgfmt msggrep msginit msgmerge msgunfmt msguniq ngettext recode-sr-latin xgettext

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
    doc NEWS README COPYING
    compat gettext 0.17 1 1
    compat gettext 0.18.2 1 1
    compat gettext 0.18.3.1 1 1
    compat gettext 0.19.4 1 1
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
