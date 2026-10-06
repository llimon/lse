#!/bin/bash
# This is a buildpkg build.sh script
# build.sh helper functions
. ${BUILDPKG_SCRIPTS}/build.sh.functions
#
###########################################################
# Check the following 4 variables before running the script
topdir=openssl
version=1.0.2u
pkgver=8
source[0]=https://openssl.org/source/$topdir-$version.tar.gz
# If there are no patches, simply comment this
patch[0]=static-plugins.patch
patch[1]=openssl-1.0.2u-cve-2020-1971.patch
patch[2]=openssl-1.0.2u-cve-2021-23840.patch
patch[3]=openssl-1.0.2u-cve-2021-23841.patch
patch[4]=openssl-1.0.2u-cve-2021-3712.patch
patch[5]=openssl-1.0.2u-cve-2022-0778.patch

# Source function library
. ${BUILDPKG_SCRIPTS}/buildpkg.functions

# For cpu settings
. ${BUILDPKG_BASE}/gcc/build.sh.gcc.cpu

# Global settings
sover=1.0.0
shortver=102
pname=openssl${shortver}
make_check_target="test"
__configure="./Configure"

prefix=${prefix}/${pname}
configure_args=(
   --prefix=$prefix/
   zlib shared 
   no-dynamic-engine
)
if [ "$arch" = "sparc" ]; then
    configure_args+=(solaris-sparc${gcc_arch}-gcc)
else
    configure_args+=(386 solaris-x86-gcc)
fi

# Buildsystem is non-standard so we take the easy way out
export LD_OPTIONS="-Wl,-rpath,$prefix/lib"

reg prep
prep()
{
    generic_prep

    setdir source
    ${__gsed} -i '/^SHELL/s/sh/ksh/' Makefile.org
    ${__gsed} -i "s;@LIBDIR@;${prefix}/lib;g" Makefile.org

    if [ "$arch" = "i386" ]; then
	# Override the default gcc march
	${__gsed} -i "/solaris-x86-gcc/ s;-march=pentium;-march=$gcc_arch;" Configure
	# With GNU as there is no need to disable inline assembler
	${__gsed} -i "/solaris-x86-gcc/ s; -DOPENSSL_NO_INLINE_ASM;;" Configure
    fi
    # The -mv8 alias is not supported with newer gcc
    ${__gsed} -i 's/mv8/mcpu=v8/g' Configure

    ${__gsed} -i "/^CFLAG=/s;CFLAG=;CFLAG=-I${prefix}/include;" Makefile
    ${__gsed} -i "/EX_LIBS/s;-lz;-L${prefix}/lib -Wl,-rpath,${prefix}/lib -lz -llsecompat;" Makefile

}

reg build
build()
{
    setdir source

    echo $__configure "${configure_args[@]}"
    $__configure "${configure_args[@]}"

    # 1. Patch crypto/Makefile to prevent command-line max length overflow breaking ar
    ${__gsed} -i '/^\$(LIB): \$(LIBOBJ)/!b;n;c\\trm -f $(LIB)\n\tfind . -name "*.o" | xargs -n 50 $(AR) $(LIB)\n\ttest -z "$(FIPSLIBDIR)" || $(AR) $(LIB) $(FIPSLIBDIR)fipscanister.o\n\t$(RANLIB) $(LIB) || echo Never mind.' crypto/Makefile

    # 2. Fix single-pass Solaris ld failures in apps/ and test/ by enforcing a 2-pass archive scan
    ${__gsed} -i 's|LIBDEPS=" $$LIBRARIES $(EX_LIBS)"|LIBDEPS="$(EX_LIBS) -L.. -lssl -L.. -lcrypto -L.. -lssl -L.. -lcrypto"|g' apps/Makefile
    ${__gsed} -i 's|LIBDEPS=" $$LIBRARIES $(EX_LIBS)"|LIBDEPS="$(EX_LIBS) -L.. -lssl -L.. -lcrypto -L.. -lssl -L.. -lcrypto"|g' test/Makefile

    ${__make} SHARED_LDFLAGS="-shared -Wl,-rpath,${prefix}/${_libdir}" depend
    ${__make} SHARED_LDFLAGS="-shared -Wl,-rpath,${prefix}/${_libdir}"
}

reg check
check()
{
    generic_check
}

reg install
install()
{
    clean stage
    setdir source
    ${__make} INSTALL_PREFIX=$stagedir  MANDIR=$stagedir/${_mandir}  install

    doc README CHANGES FAQ INSTALL LICENSE NEWS
    
    custom_install=1
    generic_install INSTALL_PREFIX
 

    # Compatible with previous releases
    compat openssl 1.0.2j 1 1
    compat openssl 1.0.2k 1 2
    compat openssl 1.0.2o 1 3
    compat openssl 1.0.2p 1 4
    compat openssl 1.0.2r 1 5
    compat openssl 1.0.2u 1 6
    compat openssl 1.0.2u 1 7
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
