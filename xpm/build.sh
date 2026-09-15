#!/bin/bash
# This is a buildpkg build.sh script
# build.sh helper functions
. ${BUILDPKG_SCRIPTS}/build.sh.functions
#
###########################################################
# Check the following 4 variables before running the script
topdir=xpm
version=3.4k
version_major="${version%.*}"

pkgver=1

# https://fossies.org/linux/misc/old/xpm-3.4k.tar.gz
source[0]=https://fossies.org/linux/misc/old/${topdir}-${version}.tar.gz
# If there are no patches, simply comment this
#patch[0]=

# Source function library
. ${BUILDPKG_SCRIPTS}/buildpkg.functions

# Global settings
export CPPFLAGS="$CPPFLAGS -I/usr/openwin/include -I. -Ilib -I../lib"
export LDFLAGS="-R/usr/openwin/lib -L/usr/openwin/lib"

topsrcdir=${topdir}-${version}
#configure_args+=()


reg prep
prep()
{	
    generic_prep
    
    setdir source

    ${__mkdir} lib/X11
    ${__cp} lib/*.h lib/X11

	 ${__imake} -I/usr/openwin/lib/config -DTOPDIR=. -DCURDIR=. || echo "error running imake " 
    ${__make} Makefiles

    cat << EOF >> lib/mapfile
libXpm.so.3.4 {
    global:
        Xpm*;
    local:
        *;
};
EOF

    	${__gsed} -i "s|\(PREPROCESSCMD[[:space:]]*=\).*|\1 gcc -E \$(STD_CPP_DEFINES) |" ${make_file}

}

reg build
build()
{
	 no_configure=1
    setdir source
    ${__make}  \
        CC=gcc PREPROCESSCMD="$CPPFLAGS" \
        SHLIBLDFLAGS="-G -z text" \
        CDEBUGFLAGS="-g" \
        EXTRA_LDOPTIONS="$LDFLAGS" \
        OPENWINHOME="/usr/openwin" \
        PICFLAGS="-fPIC" CCOPTIONS="-std=gnu99 ${GCC_MCPU}" 
}

reg check
check()
{
    generic_check
}

reg install
install()
{
	
	setdir source
   clear stage

   export PATH=/usr/openwin/bin:$PATH
   DESTDIR=${stagedir}/openwin
   BINDIR=/usr/openwin/bin
   mkdir -p $DESTDIR
	echo "destdir $DESTDIR"
   ${__make} install \
      DESTDIR="${stagedir}/${prefix}/openwin" \
      BINDIR="/usr/openwin/bin" \
      SHLIBLDFLAGS="-G -z text" \
      XPMBINDIR="/bin" \
      SHELL="/usr/tgcware/bin/bash"

   ${__make} install.man \
      DESTDIR="${stagedir}/${prefix}/openwin" \
      BINDIR="/usr/openwin/bin" \
      SHLIBLDFLAGS="-G -z text" \
      XPMBINDIR="${stagedir}/${prefix}/bin" \
      SHELL="/usr/tgcware/bin/bash"
   setdir stage
   grm -v -d openwin

}

reg pack
pack()
{
	 DESTDIR=${stagedir}
    #setdir stage
    generic_pack DESTDIR
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
