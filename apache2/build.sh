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
#    generic_install DESTDIR
#    doc README 
     sed -i \
        -e 's|^Group #-1|Group nogroup|' \
        -e 's|^#ServerName www.example.com:80|ServerName 127.0.0.1:80|' \
        -e 's|^ServerName .*|ServerName 127.0.0.1:80|' \
        ${stagedir}${prefix}/apache2/conf/httpd.conf
    for f in ${stagedir}${prefix}/apache2/conf/httpd.conf; do
        echo "" >> "$f"
        echo "# Custom SVR4 Accept Mutex Configuration" >> "$f"
        echo "AcceptMutex fcntl" >> "$f"
        echo "LockFile ${PREFIX}/logs/accept.lock" >> "$f"
    done

    ${__mkdir} -p ${stagedir}/${_sysconfdir}/init.d
    ${__mkdir} -p ${stagedir}/${_sysconfdir}/rc0.d
    ${__mkdir} -p ${stagedir}/${_sysconfdir}/rc1.d
    ${__mkdir} -p ${stagedir}/${_sysconfdir}/rcS.d
    ${__mkdir} -p ${stagedir}/${_sysconfdir}/rc2.d
    ${__cp}  ${metadir}/httpd.init ${stagedir}/${_sysconfdir}/lse_httpd

    chmod 755 ${metadir}/httpd.init ${stagedir}/${_sysconfdir}/lse_httpd
    (setdir ${stagedir}/${_sysconfdir}/rc0.d; ${__ln} -sf ../init.d/lse_httpd K02lse_httpd)
    (setdir ${stagedir}/${_sysconfdir}/rc1.d; ${__ln} -sf ../init.d/lse_httpd K02lse_httpd)
    (setdir ${stagedir}/${_sysconfdir}/rcS.d; ${__ln} -sf ../init.d/lse_httpd K02lse_httpd)
    (setdir ${stagedir}/${_sysconfdir}/rc2.d; ${__ln} -sf ../init.d/tgcs_httpd S98lse_httpd)
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
