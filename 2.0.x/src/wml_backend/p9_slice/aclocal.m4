dnl #
dnl #  aclocal.m4 -- Local M4 functions for GNU autoconf 2.x
dnl #  Copyright (c) Ralf S. Engelschall, <rse@en.engelschall.com>
dnl #
dnl #  NOTICE:
dnl #      all defined macros are named AC_*
dnl #      all defined and used variables are named acl-*
dnl #
dnl
dnl ##########################################################
dnl ##
dnl ##  check for Perl 5 module
dnl ##
dnl ##########################################################
dnl
define(AC_CHECK_PERL_MODULE,[dnl
AC_MSG_CHECKING([for Perl module $1 ($2)])
if test ".$have_$3" = .0; then
    AC_MSG_RESULT([forced: closed world])
else
    rc=`$PATH_PERL -e 'eval "use $1 ifelse($2, ANY, , $2)"; print "ok" unless [$]@'`; 
    if test ".$rc" = .ok; then
        have_$3=1
        AC_MSG_RESULT([found])
    else
        have_$3=0
        AC_MSG_RESULT([not found])
    fi
fi
AC_SUBST(have_$3)
])dnl
dnl
dnl
dnl ##########################################################
dnl ##
dnl ##  check for latest Perl interpreter
dnl ##
dnl ##########################################################
dnl
dnl
define(AC_CHECK_PERL_INTERPRETER,[dnl
AC_MSG_CHECKING([for Perl interpreter])
AC_ARG_WITH(perl,dnl
[  --with-perl=PATH        force the usage of a specific Perl 5 interpreter],[
dnl [[
perlprog=$with_perl
perlvers=`$perlprog -e 'printf "%.3f",$]'`
dnl ]
],[
perlvers=
for dir in `echo $PATH | sed -e 's/:/ /g'`; do
    for perl in perl5 perl miniperl; do
         if test -f "$dir/$perl"; then
             if test -x "$dir/$perl"; then
                 perlprog="$dir/$perl"
                 if $perlprog -e 'require 5.003'; then
dnl [[
                     perlvers=`$perlprog -e 'printf "%.3f",$]'`
dnl ]
                     break 2
                 fi
             fi
         fi
    done
done
])dnl
AC_MSG_RESULT([$perlprog v$perlvers])
if test -f $perlprog; then
    :
else
    AC_ERROR([required program ``perl'' not found])
fi
AC_SUBST(perlprog)
AC_SUBST(perlvers)
PATH_PERL=$perlprog
AC_SUBST(PATH_PERL)
])dnl
dnl
dnl ##########################################################
dnl ##
dnl ##  check for MakeMaker install paths
dnl ##
dnl ##########################################################
dnl
define(AC_CHECK_PERL_MM_PATHS,[dnl
AC_MSG_CHECKING([for MakeMaker's private install paths])
MYTMPDIR=${TMPDIR-/tmp}/ac.$$
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
( cd $MYTMPDIR; eval "$PATH_PERL Makefile.PL PREFIX=$dir/perl LIB=$dir/perl/lib >/dev/null 2>/dev/null" )
for line in `make -f $MYTMPDIR/Makefile dummy | grep '^INSTALL'`; do
    eval "$line"
done
rm -rf $MYTMPDIR 2>/dev/null
AC_MSG_RESULT([ok])
AC_SUBST(INSTALLPRIVLIB)
AC_SUBST(INSTALLARCHLIB)
])dnl
dnl
