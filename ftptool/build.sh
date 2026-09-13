#!/bin/bash
# This is a buildpkg build.sh script
# build.sh helper functions
. ${BUILDPKG_SCRIPTS}/build.sh.functions
#
###########################################################
# Check the following 4 variables before running the script
topdir=Ftptool
version=4.6
version_major="${version%.*}"

pkgver=1

source[0]=https://www.ibiblio.org/pub/X11/contrib/utilities/${topdir}${version}.tar.gz
# If there are no patches, simply comment this
#patch[0]=

# Source function library
. ${BUILDPKG_SCRIPTS}/buildpkg.functions


# Global settings
topsrcdir=${topdir}-${version}
#configure_args+=()

topsrcdir="${topdir}${version}"

reg prep
prep()
{	
    generic_prep
    
    setdir source


    ${__gsed} -i 's|# DEFINES= -DSYSV -DSVR4|DEFINES= -DSYSV -DSVR4 -DOWTOOLKIT_WARNING_DISABLED|' Makefile
    #${__gsed} -i 's|# LIBSUNOS5= -L${OPENWINHOME}/lib -lsocket -lnsl -lm|LIBSUNOS5= -L${OPENWINHOME}/lib -R${OPENWINHOME}/lib -lsocket -lnsl -lm|' Makefile
    ${__gsed} -i 's|# LIBSUNOS5= -L${OPENWINHOME}/lib -lsocket -lnsl -lm|LIBSUNOS5=-L/usr/openwin/lib -R/usr/openwin/lib -lsocket -lnsl -lm |' Makefile
    ${__gsed} -i 's|# CC=gcc -g|CC=gcc|' Makefile
    ${__gsed} -i "s|CDEBUGFLAGS = -O -xF|CDEBUGFLAGS = $CFLAGS -g -mno-unaligned-doubles -fno-pack-struct -I/usr/openwin/include |" Makefile
    ${__gsed} -i 's|CCOPTIONS = -DSYSV -DSVR4 -xF -Wa,-cg92|CCOPTIONS = -DSUNOS41 -DXVIEW3 -DSYSV -DSVR4 -std=gnu99|' Makefile
    ${__gsed} -i 's|# XVIEW= -DXVIEW3|XVIEW= -DXVIEW3|' Makefile


    ${__gsed} -i "s|^# OPENWINHOME[[:space:]]*=.*|OPENWINHOME= ${prefix}/openwin|" Makefile
    ${__gsed} -i "s|\(BINDIR[[:space:]]*=\).*|\1 \$(OPENWINHOME)/bin|" Makefile
    ${__gsed} -i "s|\(LIBDIR[[:space:]]*=\).*|\1 \$(OPENWINHOME)/lib|" Makefile
    ${__gsed} -i "s|\(MANDIR[[:space:]]*=\).*|\1 /share/man/man1|" Makefile
    ${__gsed} -i "s|\(HELPDIR[[:space:]]*=\).*|\1 \$(OPENWINHOME)/help|" Makefile
    ${__gsed} -i "s|\(DESTDIR[[:space:]]*=\).*|\1 ${stagedir}/usr/tgcware/openwin|" Makefile
    ${__gsed} -i "s|\(MKDIRHIER[[:space:]]*=\).*|\1 /bin/sh /usr/openwin/bin/mkdirhier|" Makefile
    ${__gsed} -i 's|$(INSTALL) -c $(INSTMANFLAGS) ftptool.info $(HELPDIR)/ftptool.info|#$(INSTALL) -c $(INSTMANFLAGS) ftptool.info $(HELPDIR)/ftptool.info|' Makefile
}

reg build
build()
{
    no_configure=1
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
    # Note: I'm doing a manual install, cause the old Makefile cannot easily have a stage area
    DESTDIR=${stagedir}
    mkdir -p $DESTDIR
    generic_install DESTDIR
    cd ${srcdir}/${topsrcdir}
    make install.man


    #doc README TODO README.FIRST WISHLIST BUGS LEGAL_NOTICE
}

reg pack
pack()
{
    DESTDIR=${stagedir}
    generic_pack DESTTDIR
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
