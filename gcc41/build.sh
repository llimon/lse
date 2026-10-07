#!/bin/bash
# This is a buildpkg build.sh script
# build.sh helper functions
. ${BUILDPKG_SCRIPTS}/build.sh.functions
#
###########################################################
# Check the following 4 variables before running the script
topdir=gcc
version=4.1.2
pkgver=2
source[0]=ftp://ftp.sunet.se/pub/gnu/gcc/releases/$topdir-$version/$topdir-$version.tar.bz2
## If there are no patches, simply comment this
patch[0]=gcc-4.1.2-new-makeinfo.patch
patch[1]=gcc-4.1.2-new-gas.patch

# Source function library
. ${BUILDPKG_SCRIPTS}/buildpkg.functions

# Common settings for gcc
. ${BUILDPKG_BASE}/gcc/build.sh.gcc.common

# Global settings
# This compiler is bootstrapped with gcc 3.4.6
#export PATH=/opt/gnu/gnat/bin:/usr/tgcware/gcc34/bin:/usr/local/bin:$PATH
export CPPFLAGS="-I/usr/tgcware/include"
export LDFLAGS="-static-libgcc -L/usr/tgcware/lib -R/usr/local/lse"
#export LIBS="-lgcc_s"
unset LIBS
export SHELL=/usr/local/lse/bin/bash

# This compiler is bootstrapped with gcc 4.0.4
export PATH=/usr/tgcware/gcc40/bin:$PATH

#LD_LIBRARY_PATH=/usr/local/lse/lib

export LD=/usr/ccs/bin/ld
export DEFAULT_LINKER=/usr/ccs/bin/ld


configure_args+=(
   --with-ld=/usr/ccs/bin/ld
   --without-gnu-ld
   --disable-nls
   --with-gmp=/usr/local/lse
   --with-mpfr=/usr/local/lse
   --with-stage1-ldflags="-static-libgcc $LDFLAGS" 
   --with-boot-ldflags="-static-libgcc $LDFLAGS"
   --enable-sjlj-exceptions
   --disable-decimal-float
)
configure_args+=(--enable-obsolete)

reg prep
prep()
{
    generic_prep
    setdir source
    # Set bugurl and vendor version
    ${__gsed} -i "s|URL:[^>]*|URL:$gccbugurl|" gcc/version.c
    ${__gsed} -i "/VERSUFFIX/ s/\"\"/\" ($gccpkgversion)\"/" gcc/version.c
}

reg build
build()
{
    setup_tools
    ${__mkdir} -p ${srcdir}/$objdir
    generic_build ../$objdir
}

reg install
install()
{
    clean stage
    setdir ${srcdir}/${objdir}
    ${__make} DESTDIR=$stagedir install
    custom_install=1
    generic_install
    ${__find} ${stagedir} -name '*.la' -print | ${__xargs} ${__rm} -f

    # Rearrange libraries
    redo_libs

    # Remove obsolete gccbug script
    ${__rm} -f $stagedir$prefix/bin/gccbug

    # Turn all the hardlinks in bin into symlinks
    redo_bin

    # Place share/docs in the regular location
    prefix=$topinstalldir
    doc COPYING* BUGS FAQ MAINTAINERS NEWS
}

reg check
check()
{
    setdir source
    setdir ../$objdir
    ${__make} -k check
}

reg pack
pack()
{
    iprefix=${topdir}${abbrev_majorminor}
    generic_pack
}

reg distclean
distclean()
{
    META_CLEAN="$META_CLEAN compver.*"
    clean distclean
    ${__rm} -rf $srcdir/$objdir
}

###################################################
# No need to look below here
###################################################
build_sh $*
