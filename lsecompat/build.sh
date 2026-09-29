#!/bin/bash
# This is a buildpkg build.sh script
# build.sh helper functions
. ${BUILDPKG_SCRIPTS}/build.sh.functions
#
###########################################################
# Check the following 4 variables before running the script
topdir=lsecompat
version=1.0
pkgver=1
# If there are no patches, simply comment this

# Source function library
. ${BUILDPKG_SCRIPTS}/buildpkg.functions


workdir=`pwd`
no_configure=1

gnu_link tar

reg prep
prep()
{
    echo "no-op"
}

#reg distclean
#{
#    cd src || exit 1
#    rm *.o *.a *.so
#}

reg build
build()
{
    mkdir -p ${srcdir}/${topdir}-${version}
    cp ${workdir}/src/* ${srcdir}/${topdir}-${version}/
    generic_build

    #cd src || exit 1
    #make
}

reg check
check()
{
   echo "no-op"
}

reg install
install()
{
    
    cd src || exit 1
    make install prefix=${stagedir}${prefix}
#    generic_install DESTDIR
    #doc ChangeLog README NEWS COPYING
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
