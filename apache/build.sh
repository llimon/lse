#!/bin/bash
# This is a buildpkg build.sh script
# build.sh helper functions
. ${BUILDPKG_SCRIPTS}/build.sh.functions
#
###########################################################
# Check the following 4 variables before running the script
topdir=httpd
version=2.0.65
pkgver=1
source[0]=https://archive.apache.org/dist/httpd/httpd-2.0.65.tar.gz
# If there are no patches, simply comment this

# Source function library
. ${BUILDPKG_SCRIPTS}/buildpkg.functions


# solaris 2.5.1 does not have ftello
make_build_opts=( CC="${CC:-gcc}  -include ${prefix}/include/lsecompat.h" )
CPPFLAGS="$CPPFLAGS -std=gnu99 -DHAVE_MEMMOVE"
LIBS="$LIBS -llsecompat -llsenet"
configure_args=(
   --prefix=/usr/local/lse/apache2
   --with-mpm=prefork
   --disable-ssl 
   --enable-so
   --enable-cgi
   --enable-rewrite
   --disable-ipv6
)

gnu_link tar

reg prep
prep()
{
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
    generic_install DESTDIR
    doc ChangeLog README NEWS COPYING
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
