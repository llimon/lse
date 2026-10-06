#!/bin/bash
# This is a buildpkg build.sh script
# build.sh helper functions
. ${BUILDPKG_SCRIPTS}/build.sh.functions
#
###########################################################
# Check the following 4 variables before running the script
topdir=sudo
version=1.8.32
pkgver=1
source[0]=https://www.sudo.ws/sudo/dist/$topdir-$version.tar.gz
# If there are no patches, simply comment this
# patch lib/util/getentropy.c
patch[0]=getentropy.c.patch
#patch[1]=RTLD_LOCAL.patch

# Source function library
. ${BUILDPKG_SCRIPTS}/buildpkg.functions

export LD_RUN_PATH="/usr/local/lse/lib"

# Global settings
export LIBS="$LIBS -lposix4 -llsecompat"

# disabling generation of .so binaries; We don't need that stuff for sudo and makes it a little leaner for resource contrained workstations.
configure_args+=(
	--enable-static 
	--disable-shared 
	--enable-static-sudoers 
	--disable-poll 
	--disable-hardening 
	--sysconfdir=/usr/local/lse/etc 
	--with-man 
	--with-all-insults
        CPPFLAGS="$CPPFLAGS -I/usr/local/lse/include -include /usr/local/lse/include/lse/snprintf_compat.h"
        CFLAGS="$CFLAGS"
        LDFLAGS="$LDFLAGS -lposix4 -llsecompat"
        SUDO_LDFLAGS="$LDFLAGS -lposix4 -llsecompat"
        LIBS="-lposix4 -llsecompat"
)
#make_build_opts=( CPPFLAGS="$CPPFLAGS -include $prefix/include/lsecompat.h" )
#make_build_opts=( CPPFLAGS="$CPPFLAGS -include $prefix/include/lse/snprintf_compat.h" )

reg prep
prep()
{
    generic_prep
    setdir source
    ${__gsed} -i "/^install_uid/ s/0/$(id -u)/" Makefile.in
    ${__gsed} -i "/^install_gid/ s/0/$(id -u)/" Makefile.in
    # Configure adds closefrom_fallback into the linker export map as a global
    # but that function is static void meaning it cannot possibly be a global
    # symbol. Later gcc/linker combos on Solaris seems not to care but gcc with
    # the Solaris 7 linker will fail with a symbol reference error when parsing
    # the mapfile.
    ${__gsed} -i 's/closefrom_fallback//' configure

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
    ${__mv} ${stagedir}${prefix}/share/doc/sudo ${stagedir}${prefix}/${_vdocdir}
    #${__rm} -f ${stagedir}${prefix}/etc/sudoers
    #${__rm} -f ${stagedir}${prefix}/relnotes/sudo*
#lprefix/relnotes/sudo-1.8.32-1/sudo.txt
    validate_staged_files ${stagedir} "/usr/local/lse/bin/sudo"
}

reg pack
pack()
{
    lprefix=${prefix#/*}
    topinstalldir=/
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
