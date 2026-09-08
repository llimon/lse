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


# Global settings
export CPPFLAGS="$CPPFLAGS -include $prefix/include/compat/dlfcn_compat.h -include $prefix/include/compat/snprintf_compat.h"
export LDFLAGS="$LDFLAGS -lposix4 -lsnprintf"
ac_overrides="gl_cv_func_snprintf_directive_n=yes
gl_cv_func_vsnprintf_directive_n=yes
gl_cv_func_printf_directive_n=yes
"
# disabling generation of .so binaries; We don't need that stuff for sudo and makes it a little leaner for resource contrained workstations.
configure_args+=(--enable-static --disable-shared --enable-static-sudoers --disable-poll --disable-hardening --sysconfdir=/usr/tgcware/etc --with-man --with-all-insults)

export no_configure=1

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

run_configure()
{
    local my_ac_overrides="$platform_ac_overrides $ac_overrides"
    setdir ${srcdir}/${topsrcdir}/$1

    local acvar
    for acvar in $my_ac_overrides; do
      	export $acvar
    done
    echo $__configure "${configure_args[@]}"
    $__configure "${configure_args[@]}"

	 # Compatibillity with solaris older than 2.6
	 #if [ "`uname -r`" = "5.6" ]; then
	 #	${__gsed} -i 's|#define HAVE_SNPRINTF 1|/* #define HAVE_SNPRINTF 1 */|' config.h
	 #	${__gsed} -i 's|#define HAVE_VSNPRINTF 1|/* #define HAVE_VSNPRINTF 1 */|' config.h
	 #fi

	
}

reg build
build()
{
	 run_configure
    ${__make} ${make_build_opts} $(_upls $make_build_target)
    #generic_build
	 
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
    ${__rm} -f ${stagedir}${prefix}/etc/sudoers
    #${__rm} -f ${stagedir}${prefix}/relnotes/sudo*
#lprefix/relnotes/sudo-1.8.32-1/sudo.txt
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
