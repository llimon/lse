#!/bin/bash
# This is a buildpkg build.sh script
# build.sh helper functions
. ${BUILDPKG_SCRIPTS}/build.sh.functions
#
###########################################################
# Check the following 4 variables before running the script
topdir=ntp
version=4.2.6p5
release_version="${version%p*}"  # Strips .6p5 -> 4.2


pkgver=1

source[0]=https://downloads.nwtime.org/$topdir/${release_version}/${topdir}-${version}.tar.gz
# If there are no patches, simply comment this
patch[0]=rfc2553.patch

# Source function library
. ${BUILDPKG_SCRIPTS}/buildpkg.functions


# Global settings
export LIBS="-lsocket -lnsl"
ac_overrides="ac_cv_header_sys_timepps_h=no ac_cv_header_timepps_h=no"
#configure_args+=(--disable-ipv6 --without-crypto --disable-all-clocks)
configure_args+=(--disable-ipv6 --disable-all-clocks)


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
    clean stage
    setdir source
    ${__make} DESTDIR=$stagedir install

    ${__mkdir} -p ${stagedir}/${_sysconfdir}/init.d
    ${__mkdir} -p ${stagedir}/${_sysconfdir}/rc0.d
    ${__mkdir} -p ${stagedir}/${_sysconfdir}/rc1.d
    ${__mkdir} -p ${stagedir}/${_sysconfdir}/rc2.d
    ${__mkdir} -p ${stagedir}/${_sysconfdir}/rcS.d
    ${__mkdir} -p ${stagedir}${prefix}/${_sysconfdir}
   
    # Install initscript
    ${__gcp} $patchdir/ntpd.init ${stagedir}/${_sysconfdir}/init.d/lse_ntpd
    ${__gcp} $patchdir/ntp.conf ${stagedir}${prefix}/${_sysconfdir}/ntp.conf
    chmod 755 ${stagedir}/${_sysconfdir}/init.d/lse_ntpd

    # Create empty drift file for ntpd frequency tracking
    touch ${stagedir}${prefix}/${_sysconfdir}/ntp.drift
    chmod 644 ${stagedir}${prefix}/${_sysconfdir}/ntp.drift

    (setdir ${stagedir}/${_sysconfdir}/rc0.d; ${__ln} -sf ../init.d/lse_ntpd K02lse_ntpd)
    (setdir ${stagedir}/${_sysconfdir}/rc1.d; ${__ln} -sf ../init.d/lse_ntpd K02lse_ntpd)
    (setdir ${stagedir}/${_sysconfdir}/rcS.d; ${__ln} -sf ../init.d/lse_ntpd K02lse_ntpd)
    (setdir ${stagedir}/${_sysconfdir}/rc2.d; ${__ln} -sf ../init.d/tgcs_ntpd S98lse_ntpd)
    custom_install=1
    generic_install DESTDIR
    doc README COPYRIGHT
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
