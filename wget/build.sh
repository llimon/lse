#!/bin/bash
# This is a buildpkg build.sh script
# build.sh helper functions
. ${BUILDPKG_SCRIPTS}/build.sh.functions
#
###########################################################
# Check the following 4 variables before running the script
topdir=wget
version=1.20.3
pkgver=2
source[0]=https://mirrors.kernel.org/gnu/wget/$topdir-$version.tar.lz
# If there are no patches, simply comment this
patch[0]=getprogname.patch

# Source function library
. ${BUILDPKG_SCRIPTS}/buildpkg.functions

# Global settings extention / overrides
#export LIBS="$LIBS -llsecompat -llsenet"
export LIBS="$LIBS -llsecompat"

# Supply library search paths and headers (RPATH is inherited from LDFLAGS)
export LDFLAGS="$LDFLAGS -L${ssl_prefix}/lib"
export CFLAGS="${CFLAGS} -std=gnu99 -I${ssl_prefix}/include"
export CXXFLAGS="${CXXFLAGS} -fpermissive -I${ssl_prefix}/include"
configure_args+=(
   --with-ssl=openssl 
   --with-libssl-prefix=$prefix/openssl102 
   --disable-nls
   --disable-iri
)

reg prep
prep()
{
    generic_prep
}

reg build
build()
{

    ac_overrides="ac_cv_func_getprogname=yes \
                  ac_cv_prog_cc_c11=no \
                  gl_cv_compiler_c11_supported=no \
                  ac_cv_func_vsnprintf=yes \
                  ac_cv_func_snprintf=yes \
                  gl_cv_func_vsnprintf_posix=yes \
                  gl_cv_func_snprintf_posix=yes \
                  gl_cv_func_vsnprintf_zerosize_bug=no"

    currdir = $PWD
    #make_build_opts=( CC="${CC:-gcc} -include $prefix/include/lsecompat.h" )

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
    doc AUTHORS COPYING NEWS README MAILING-LIST
    echo "ca_certificate = /usr/local/lse/etc/curl-ca-bundle.pem" | tee -a ${stagedir}${prefix}/etc/wgetrc
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
