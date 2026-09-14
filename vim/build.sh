#!/bin/bash
# This is a buildpkg build.sh script
# build.sh helper functions
. ${BUILDPKG_SCRIPTS}/build.sh.functions
#
###########################################################
patchlevel=4804
# repo_pl is always 4 digits
repo_pl=$patchlevel
basever=8.2
# Check the following 4 variables before running the script
topdir=vim
version=${basever}.${patchlevel}
pkgver=2
source[0]="https://github.com/vim/vim/archive/v${basever}.${repo_pl}.tar.gz#/${topdir}-${version}.tar.gz"
# If there are no patches, simply comment this
#patch[0]=vim-8.2.4804-socklen_t.patch
patch[0]=sol2.5.1.patch

# Source function library
. ${BUILDPKG_SCRIPTS}/buildpkg.functions

# Global settings
export CPPFLAGS="$CPPFLAGS -I$prefix/include/ncurses -include $prefix/include/compat/snprintf_compat.h -include $prefix/include/compat/vterm_compat.h -include $prefix/include/compat/mbstowcs_compat.h -include $prefix/include/compat/wcsstr_compat.h -include $prefix/include/compat/socket_compat.h"
export CFLAGS="$CFLAGS -Wno-implicit-int"
export LIBS="$LIBS -liconv -lgcc_s -lsnprintf -lw -lsocket"
export SHELL=$prefix/bin/bash
export CONFIG_SHELL=$prefix/bin/bash

## Features = normal, we are building for very old hardware. Need to preserver memory!
basic_args=(--prefix=$prefix --without-local-dir)
basic_args+=(--disable-multibyte --disable-perlinterp --disable-pythoninterp)
basic_args+=(--disable-tclinterp --disable-netbeans --disable-cscope)
basic_args+=(--with-compiledby="Luis E Limon")
basic_args+=(--with-modified-by="Luis E Limon")
# Do not let scripts add a dependency on perl
ignore_deps="LSEperl"
# We need to override this
topsrcdir=vim-${basever}.${repo_pl}

reg prep
prep()
{
    generic_prep
}

reg build
build()
{

	setdir source
	${__rm} src/gvim src/vim src/vim-small

	 # Build Motif GUI version (Normal features)
    configure_args=("${basic_args[@]}" --with-features=normal --enable-gui=motif)
    generic_build
    setdir source
    ${__cp} src/vim src/gvim

    # Patch pty.c once (Idempotent: safe to run anytime before or after configure)
    ${__gsed} -i '/defined(UNIX)/!s/#if defined(FEAT_GUI) || defined(FEAT_JOB_CHANNEL)/#if defined(FEAT_GUI) || defined(FEAT_JOB_CHANNEL) || defined(UNIX)/' src/pty.c

    # Build small console version with +spell
    setdir source
    ${__make} clean

    # Force preprocessor to define FEAT_SPELL for small build
    ${__rm} -f src/auto/config.cache  # Wipe cached CPPFLAGS so configure won't abort

    configure_args=("${basic_args[@]}" --with-features=small --enable-gui=no --with-x=no)
    
    $__configure "${configure_args[@]}"
    ${__make}
    
    ${__cp} src/vim src/vim-small

    # Build normal console version
    setdir source
    ${__make} clean
    ${__rm} -f src/auto/config.cache  # Wipe cached CPPFLAGS again

    configure_args=("${basic_args[@]}" --with-features=normal --enable-gui=no --with-x=no)
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
    configure_args=("${basic_args[@]}" --enable-gui=motif)
    generic_install DESTDIR
    setdir source
    ${__cp} src/gvim ${stagedir}${prefix}/${_bindir}
    ${__cp} src/vim-small ${stagedir}${prefix}/${_bindir}

    # Verify all 3 binaries exist inside STAGING bindir
    setdir ${stagedir}${prefix}/${_bindir}

    ${__ln} -s gvim gvimdiff
    ${__ln} -s gvim gview

    # Validate we got our goods
    validate_staged_files  ${stagedir} \
       "${prefix}/${_bindir}/vim" \
       "${prefix}/${_bindir}/gvim" \
       "${prefix}/${_bindir}/vim-small" \
       "${prefix}/${_mandir}/man1/vim.1"


    # Create Man simlinks
    setdir ${stagedir}${prefix}/${_mandir}/man1
    ${__ln} -s vim.1 vim-small.1
    ${__ln} -s vim.1 gvim.1
    ${__ln} -s vim.1 gview.1
    ${__ln} -s vimdiff.1 gvimdiff.1

    custom_install=1
    generic_install DESTDIR
    doc README.txt
    ${__rm} -rf ${stagedir}${prefix}/${_mandir}/{fr,it,pl,ru,ja,da,de,tr}*
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
