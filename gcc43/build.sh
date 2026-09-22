#!/bin/bash
# This is a buildpkg build.sh script
# build.sh helper functions
. ${BUILDPKG_SCRIPTS}/build.sh.functions
#
###########################################################
# Check the following 4 variables before running the script
topdir=gcc
version=4.3.6
pkgver=2
source[0]=ftp://ftp.sunet.se/pub/gnu/gcc/releases/$topdir-$version/$topdir-$version.tar.bz2
## If there are no patches, simply comment this
patch[0]=gcc-4.3.6-libffi-unwind.patch
patch[1]=sol-ld-fixes.patch

# Source function library
. ${BUILDPKG_SCRIPTS}/buildpkg.functions

# Common settings for gcc
. ${BUILDPKG_BASE}/gcc/build.sh.gcc.common

# This compiler is bootstrapped with gcc 4.2.4
export PATH=/usr/tgcware/gcc42/bin:$PATH

reg prep
prep()
{
    generic_prep
}

reg build
build()
{
    setup_tools
    ${__mkdir} -p ${srcdir}/$objdir
    export CONFIG_SHELL=/usr/bin/ksh

    echo "GCC_ARCH: $gcc_arch"
	 [[ "$gcc_arch" == "" ]] && exit

    # v7 requires explicit -lgcc_s
    if [[ "$gcc_arch" == *"v7"* ]]; then
        export LDFLAGS="$LDFLAGS -lgcc_s"
    fi

    # Append stage1/boot flags using the computed LDFLAGS
    export configure_args+=( 
       --with-stage1-ldflags="-static-libgcc $LDFLAGS" 
      --with-boot-ldflags="-static-libgcc $LDFLAGS"
       --enable-sjlj-exceptions
    )
    echo "--------------------------------"
    echo "Build Parameters to configure"
	 printf '<%s>\n' "${configure_args[@]}"
    echo "--------------------------------"

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

    # Turn all the hardlinks in bin into symlinks
    redo_bin

    # Place share/docs in the regular location
    prefix=$topinstalldir
    doc COPYING* MAINTAINERS NEWS
    # Make sure we got our goodies
    validate_gcc_installation
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
