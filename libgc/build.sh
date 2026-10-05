#!/bin/bash
# This is a buildpkg build.sh script
# build.sh helper functions
. ${BUILDPKG_SCRIPTS}/build.sh.functions
#
###########################################################
# Check the following 4 variables before running the script
topdir=gc
version=7.2
pkgver=1
source[0]=https://www.hboehm.info/gc/gc_source/${topdir}-${version}d.tar.gz
# If there are no patches, simply comment this

# Source function library
. ${BUILDPKG_SCRIPTS}/buildpkg.functions

# ./configure --prefix=/usr/local/lse --mandir=/usr/local/lse/share/man --infodir=/usr/local/lse/share/info --build=sparc-sun-solaris2.5 --enable-shared --enable-threads=posix CC=gcc   LIBS="-lpthread -lthread -lposix4 -llsecompat" ac_cv_lib_rt_clock_gettime=no ac_cv_lib_rt_sem_init=no
# make
# sed -i 's/-lrt//g' Makefile
# sed -i 's/-lrt//g' libtool
# 
# Configure globals
#LIBS="$LIBS -lpthread -lthread -lposix4"
#CPPFLAGS="$CPPFLAGS -std=gnu99"
configure_args+=(
  --build=sparc-sun-solaris2.5 \
  --enable-shared \
  --enable-threads=posix \
  CC="gcc" \ 
  LIBS="-lpthread -lthread -lposix4 -llsecompat"
)
ac_overrides="ac_cv_lib_rt_clock_gettime=no ac_cv_lib_rt_sem_init=no"

reg prep
prep()
{
    generic_prep
}

reg build
build()
{
    no_configure=1
    generic_run_configure

    sed -i 's/-lrt//g' Makefile
    sed -i 's/-lrt//g' libtool
    
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
