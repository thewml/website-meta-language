dnl ##
dnl ##  configure.in -- GNU autoconf source for WML
dnl ##  Copyright (c) 1997 Ralf S. Engelschall, All Rights Reserved. 
dnl ##

AC_PREREQ(2.50)dnl
AC_REVISION($Revision: 1.31 $)

dnl ##  setup
AC_INIT(INSTALL)
AC_PREFIX_DEFAULT(/usr/local)
AC_CONFIG_AUX_DIR([etc])

shtool=./etc/shtool
TERM_BOLD=`$shtool echo -e %B 2>/dev/null`
TERM_NORM=`$shtool echo -e %b 2>/dev/null`

dnl ##  version id
WML_VERSION="`$shtool version -l txt -d long VERSION`"
AC_SUBST(WML_VERSION)
VERSION="`$shtool version -l txt VERSION`"
AC_SUBST(VERSION)
echo "${TERM_BOLD}Configuring for WML $WML_VERSION${TERM_NORM}"

dnl ##  remember the arguments
WML_CONFIG_ARGS="$*"
AC_SUBST(WML_CONFIG_ARGS)

dnl ##  look for --enable-maintainer-mode option
WML_MAINTAINER_MODE

dnl ##  built env
AC_CONFIGURE_PART(CHECK: Build Environment)
AC_BUILT_PARAMS

dnl ##  fix installation paths
test "x$prefix" = xNONE && prefix=$ac_default_prefix
eval "dir=$prefix"
case $dir in
    *wml* ) libsubdir= ;;
        * ) libsubdir="/wml" ;;
esac
AC_SUBST(libsubdir)

tmpdir=${TMPDIR-/tmp}
AC_SUBST(tmpdir)

dnl ##  replace "::" with "." on MS-Windows
DCOLON="::"
AC_CANONICAL_HOST
case $host in
  *-*-msdos* | *-*-go32* | *-*-mingw32* | *-*-cygwin* | *-*-windows*)
    DCOLON="."
    ;;
esac
AC_CANONICAL_TARGET
case $target in
  *-*-msdos* | *-*-go32* | *-*-mingw32* | *-*-cygwin* | *-*-windows*)
    DCOLON="."
    ;;
esac
AC_SUBST([DCOLON])
AC_EXEEXT

dnl ##  compile tools
AC_CONFIGURE_PART(CHECK: Build Utilities)
AC_ARG_WITH(cc,dnl
[  --with-cc=PATH          use a specific ANSI C compiler],[
CC=$with_cc
],[dnl
:
])dnl
AC_ARG_WITH(forced-cc,dnl
[  --with-forced-cc=PATH   force the global usage of a specific ANSI C compiler],[
CC=$with_forced_cc
export CC
],[dnl
:
])dnl
AC_PROG_CC
AC_PROG_CPP
AC_PROG_RANLIB
AC_PROG_MAKE_SET

dnl ##  perl interpreter
AC_CONFIGURE_PART(CHECK: Perl Language)
AC_CHECK_PERL_INTERPRETER
if test -f $PATH_PERL; then
    :
else
    AC_ERROR([required program ``perl'' not found])
fi
path_perl=$PATH_PERL
AC_ARG_WITH(perl_opts,dnl
[  --with-perl-opts=STRING options to pass to Makefiles when compiling Perl modules],[
PERL_OPTS=$with_perl_opts
],[dnl
PERL_OPTS=
])dnl
AC_SUBST(PERL_OPTS)

dnl ##  perl modules
AC_ARG_WITH(openworld,dnl
[  --with-openworld        try to use system-wide copies of Perl modules],
,[
with_openworld=no
])
if test ".$with_openworld" = .no; then
    have_getoptlong=0
    have_bitvector=0
    have_filepathconvert=0
    have_imagesize=0
    have_io=0
    have_termreadkey=0
    have_delimmatch=0
else
    AC_CHECK_PERL_MODULE(Getopt::Long, 2.16, getoptlong)
    AC_CHECK_PERL_MODULE(Bit::Vector, 5.2, bitvector)
    AC_CHECK_PERL_MODULE(File::PathConvert, ANY, filepathconvert)
    AC_CHECK_PERL_MODULE(Image::Size, 2.6, imagesize)
    AC_CHECK_PERL_MODULE(IO::File, 1.07, io)
    AC_CHECK_PERL_MODULE(Term::ReadKey, 2.11, termreadkey)
fi

dnl ##  makemaker paths
AC_MSG_CHECKING([for MakeMaker's private install paths])
MYTMPDIR=$tmpdir/ac.$$
rm -rf $MYTMPDIR 2>/dev/null
mkdir $MYTMPDIR
cat >$MYTMPDIR/Makefile.PL <<'EOT'
use ExtUtils::MakeMaker;
WriteMakefile(
    'NAME'      => 'Dummy::Dummy',
    'VERSION'   => '0.0'
);
sub MY::postamble {
  q{
abs2prefix = sed -e "s|^$(PREFIX)|'\\\\$$(libdir)'/perl|"

dummy:
	@echo INSTALLPRIVLIB=`echo $(INSTALLPRIVLIB) | $(abs2prefix)`
	@echo INSTALLARCHLIB=`echo $(INSTALLARCHLIB) | $(abs2prefix)`
};
}
EOT
test "x$prefix" = xNONE && prefix=$ac_default_prefix
test "x$exec_prefix" = xNONE && exec_prefix='${prefix}'
eval "dir=$libdir$libsubdir"
( cd $MYTMPDIR; eval "$path_perl Makefile.PL PREFIX=$dir/perl LIB=$dir/perl/lib >/dev/null 2>/dev/null" )
for line in `make -f $MYTMPDIR/Makefile dummy | grep '^INSTALL'`; do
    eval "$line"
done
rm -rf $MYTMPDIR 2>/dev/null
AC_MSG_RESULT([ok])
AC_SUBST(INSTALLPRIVLIB)
AC_SUBST(INSTALLARCHLIB)

dnl ##  subdir for preformatted manpages
AC_ARG_WITH(catdir,dnl
[  --with-catdir=STRING    subdir prefix for preformatted manpages (default: cat)]
,[
catdir=$with_catdir
],[
catdir=cat
])
AC_SUBST(catdir)

dnl ##   Add flags when calling configure at wml_backend/p2_mp4h:
dnl ##     - process documentation by wml_aux/tidy/tidy
dnl ##     - suppress support for loadable modules
ac_configure_args="$ac_configure_args --with-tidy=`pwd`/wml_aux/tidy/tidy"

dnl ## more GNU autoconf scripts can be found there:
AC_CONFIG_SUBDIRS(dnl
wml_backend/p2_mp4h  dnl
wml_backend/p3_eperl dnl
wml_backend/p4_gm4 dnl
wml_aux/iselect dnl
wml_common/gd dnl
)

dnl ## output generation
AC_CONFIGURE_PART(RESULT: Substituting Files)
AC_OUTPUT(dnl
Makefile dnl
wml_frontend/Makefile dnl
wml_backend/Makefile dnl
wml_backend/p1_ipp/Makefile dnl
wml_backend/p5_divert/Makefile dnl
wml_backend/p6_asubst/Makefile dnl
wml_backend/p7_htmlfix/Makefile dnl
wml_backend/p8_htmlstrip/Makefile dnl
wml_backend/p9_slice/Makefile dnl
wml_common/Makefile dnl
wml_docs/Makefile dnl
wml_aux/Makefile dnl
wml_aux/htmlinfo/Makefile dnl
wml_aux/htmlclean/Makefile dnl
wml_aux/linklint/Makefile dnl
wml_aux/map2html/Makefile dnl
wml_aux/txt2html/Makefile dnl
wml_aux/tidy/Makefile dnl
wml_aux/weblint/Makefile dnl
wml_aux/freetable/Makefile dnl
wml_misc/Makefile dnl
wml_include/Makefile dnl
wml_test/Makefile dnl
,dnl
)dnl

echo ""
echo "Now please type '${TERM_BOLD}make${TERM_NORM}' to compile. Good luck."
echo ""

dnl ##EOF##
