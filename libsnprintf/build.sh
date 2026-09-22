#!/bin/bash
# This is a buildpkg build.sh script
# build.sh helper functions
. ${BUILDPKG_SCRIPTS}/build.sh.functions
# ###########################################################
# Check the following 4 variables before running the script
topdir=libsnprintf
version=2.2.1
version_major="${version%.*}"

pkgver=1

source[0]=$patchdir/${topdir}-${version}.tar.gz
# If there are no patches, simply comment this

# Source function library
. ${BUILDPKG_SCRIPTS}/buildpkg.functions

# Global settings
export COMPATIBILITY="-DSNPRINTF_LONGLONG_SUPPORT -DSOLARIS_COMPATIBLE -DSOLARIS_BUG_COMPATIBLE"


export CFLAGS="$CFLAGS -fPIC $COMPATIBILITY"

#configure_args+=()

reg prep
prep()
{
         #get_files
    generic_prep
}

reg build
build()
{
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
    clean stage
    setdir source
    
    # Run the default install into the staging directory
    ${__make} DESTDIR=${stagedir} install


    custom_install=1
    generic_install DESTDIR

    set stage
    ${__rm} -v $prefix/lib/*.so
    
    doc ChangeLog NEWS README INSTALL LICENSE.txt
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
