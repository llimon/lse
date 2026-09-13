#!/bin/bash
# This is a buildpkg build.sh script
# build.sh helper functions
. ${BUILDPKG_SCRIPTS}/build.sh.functions
#
###########################################################
# Check the following 4 variables before running the script
topdir=xterm
version=287
pkgver=1
source[0]=https://invisible-island.net/archives/${xterm}/${topdir}-${version}.tgz
# If there are no patches, simply comment this
#patch[0]=

# Source function library
. ${BUILDPKG_SCRIPTS}/buildpkg.functions



configure_args+=( --enable-ansi-color \
    --enable-sixel-graphics \
    --disable-fifo-lines \
    --disable-wide-chars \
    --disable-luit \
    --disable-freetype \
    --disable-desktop \
    --with-x \
    --disable-session-mgt \
    --x-includes=/usr/openwin/include \
    --x-libraries=/usr/openwin/lib
)

reg prep
prep()
{
    generic_prep
    setdir source
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
}


reg build
build()
{
    #export CFLAGS="$CFLAGS -std=gnu99" 
    #export LIBS="$LIBS -lsnprintf -lgcc_s"

#    ac_overrides="ac_cv_func_getprogname=yes \
#                  ac_cv_prog_cc_c11=no \
#                  gl_cv_compiler_c11_supported=no \
#                  ac_cv_func_vsnprintf=yes \
#                  ac_cv_func_snprintf=yes \
#                  gl_cv_func_vsnprintf_posix=yes \
#                  gl_cv_func_snprintf_posix=yes \
#                  gl_cv_func_vsnprintf_zerosize_bug=no"
#
    run_configure


    # Clean out the last appended lines from xtermcfg.h
    sed -i '/Solaris 2.5.1/,$d' xtermcfg.h
    
    # Append macro overrides
    cat << 'EOF' >> xtermcfg.h

/* Solaris 2.5.1 / OpenWin X11R5 Compatibility Overrides */
#ifndef sessionShellWidgetClass
#define sessionShellWidgetClass applicationShellWidgetClass
#endif

#ifndef IceConn
#define IceConn void*
#endif

#ifndef IceSetIOErrorHandler
#define IceSetIOErrorHandler(a) ((void)0)
#endif
EOF


    currdir = $PWD

    ${__make} CC="gcc -include /usr/tgcware/include/compat/snprintf_compat.h"
}




reg check
check()
{
    generic_check
}

reg install
install()
{
    #local stage_doc_dir="${stage_dir}${prefix}/share/doc/groff-1.22"

    # Pre-create staging directories so contrib/mom doesn't choke on missing paths
    #mkdir -p "${stage_doc_dir}/pdf"
    #mkdir -p "${stage_doc_dir}/examples/mom"
    #make_install_target="install pdfdocdir=\$(docdir)/examples/mom"

    # Empty MOM_PDFDOCFILES in Makefile.sub so it doesn't attempt to install/link PDFs
    setdir source
    sed -e 's/^MOM_PDFDOCFILES =.*/MOM_PDFDOCFILES =/' \
        contrib/mom/Makefile.sub > contrib/mom/Makefile.sub.tmp \
        && mv contrib/mom/Makefile.sub.tmp contrib/mom/Makefile.sub

    generic_install DESTDIR

    doc COPYING* NEWS README TODO LICENSES
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
