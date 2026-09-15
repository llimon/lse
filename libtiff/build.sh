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
#configure_args+=()

##
## Override or extend globals
export LIBS="$LIBS -lsnprintf -lgcc_s"

reg prep
prep()
{
    generic_prep
    setdir source
    ${__gsed} -i 's|^#! /bin/sh|#!/bin/bash|' configure

}

run_configure()
{
    local my_ac_overrides="$platform_ac_overrides $ac_overrides"
    setdir ${srcdir}/${topsrcdir}/$1

    local acvar
    for acvar in $my_ac_overrides; do
        export $acvar
    done
    echo $__configure "${configure_args[@]}"
    $__configure "${configure_args[@]}"
}

reg build
build()
{
    run_configure
    ${__make} CC="${CC:-gcc}  -include ${prefix}/include/compat/snprintf_compat.h" ${make_build_opts}
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
