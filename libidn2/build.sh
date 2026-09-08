#!/bin/bash
# This is a buildpkg build.sh script
# build.sh helper functions
. ${BUILDPKG_SCRIPTS}/build.sh.functions
#
###########################################################
# Check the following 4 variables before running the script
topdir=libidn2
version=2.3.7
pkgver=1
source[0]=http://www.mirrorservice.org/sites/ftp.gnu.org/gnu/libidn/$topdir-$version.tar.gz
# If there are no patches, simply comment this
#patch[0]=

# Source function library
. ${BUILDPKG_SCRIPTS}/buildpkg.functions

# Global settings
configure_args+=(--disable-static --with-libiconv-prefix=$prefix --with-libintl-prefix=$prefix)

reg prep
prep()
{
    generic_prep
    setdir source
    # Do not build examples
    ${__gsed} -i 's/examples//' Makefile.in
}

# `reg build
# `build()
# `{
# `    #export CPPFLAGS="$CPPFLAGS -include $prefix/include/compat/getprogname_compat.h"
# `    ac_overrides="ac_cv_func_getprogname=yes \
# `                  ac_cv_prog_cc_c11=no \
# `                  gl_cv_compiler_c11_supported=no"
# `
# `
# `    #make_build_opts="$make_build_opts MAKE=\"$MAKE\" CPPFLAGS=\"$CPPFLAGS -include $prefix/include/compat/getprogname_compat.h\""
# `    make_build_opts="$mke_build_opts CPPFLAGS=\"$CPPFLAGS -include $prefix/include/compat/getprogname_compat.h\""
# `    echo make_build_opts="$make_build_opts"
# `exit
# `    generic_build
# `}


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
    export CFLAGS="$CFLAGS -std=gnu99" 
    export LIBS="$LIBS -lgcc_s"
    ac_overrides="ac_cv_func_getprogname=yes \
                  ac_cv_prog_cc_c11=no \
                  gl_cv_compiler_c11_supported=no"

    run_configure
    #${__make} CPPFLAGS="$CPPFLAGS -include config.h -include ${prefix}/include/compat/getprogname_compat.h" 
    ${__make} CC="${CC:-gcc} -include config.h -include ${prefix}/include/compat/getprogname_compat.h" ${make_build_opts}


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
    doc AUTHORS COPYING* NEWS README.md
    compat libidn2 0.11 1 1
    compat libidn2 2.0.2 1 1
    compat libidn2 2.0.3 1 1
    compat libidn2 2.0.4 1 1
    compat libidn2 2.1.1a 1 1
    compat libidn2 2.3.0 1 1
    compat libidn2 2.3.2 1 1
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
