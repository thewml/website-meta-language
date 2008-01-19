AC_DEFUN(WML_MAINTAINER_MODE,[dnl
AC_MSG_CHECKING([whether to enable maintainer-specific portions of Makefiles])
  dnl maintainer-mode is disabled by default
  AC_ARG_ENABLE(maintainer-mode,
[  --enable-maintainer-mode   enable make rules and dependencies not useful
                            (and sometimes confusing) to the casual installer],
      USE_MAINTAINER_MODE=$enableval,
      USE_MAINTAINER_MODE=no)
  AC_MSG_RESULT($USE_MAINTAINER_MODE)
  if test ".$USE_MAINTAINER_MODE" = .yes; then
    MAINT='#M#'
  else
    MAINT=
  fi
  AC_SUBST(MAINT)dnl
])dnl

AC_DEFUN(AC_CONFIGURE_PART,[dnl
AC_MSG_RESULT()
AC_MSG_RESULT(${TERM_BOLD}$1${TERM_NORM})
])dnl

AC_DEFUN(AC_CHECK_PERL_INTERPRETER,[dnl
AC_MSG_CHECKING([for Perl language])
AC_ARG_WITH(perl,dnl
[  --with-perl=PATH        force the usage of a specific Perl 5 interpreter],[
dnl [[
perlprog=$with_perl
perlvers=`$perlprog -e 'printf "%.3f",$]'`
dnl ]
],[
perlvers=
for dir in `echo $PATH | sed -e 's/:/ /g'` $tmpdir; do
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
PATH_PERL=$perlprog
AC_MSG_RESULT([$perlprog v$perlvers])
if $perlprog -e 'require 5.003'; then
    :
else
    echo ""
    echo "Latest Perl found on your system is $perlvers,"
    echo "but at least Perl version 5.003 is required."
    echo "In case the newer one is not in PATH, just use"
    echo "the option --with-perl=/path/to/bin/perl to"
    echo "provide the correct executable."
    echo ""
    AC_ERROR([Perl version too old]) 
fi
AC_SUBST(PATH_PERL)
AC_SUBST(perlprog)
AC_SUBST(perlvers)
])dnl

AC_DEFUN(AC_CHECK_PERL_MODULE,[dnl
AC_MSG_CHECKING([for Perl module $1 ($2)])
rc=`$path_perl -e 'eval {require $1 ifelse($2, ANY, , $2)}; print "ok" unless [$]@'`; 
if test ".$rc" = .ok; then
    have_$3=1
    AC_MSG_RESULT([found])
else
    have_$3=0
    AC_MSG_RESULT([not found])
fi
AC_SUBST(have_$3)
])dnl

AC_DEFUN(AC_BUILT_PARAMS,[dnl

built_system=`$shtool guessos`
AC_SUBST(built_system)

AC_MSG_CHECKING(for built user)
built_user="`$shtool echo -e -n %u@%h%d`"
AC_SUBST(built_user)
AC_MSG_RESULT($built_user)

AC_MSG_CHECKING(for built date)
built_date="`$shtool echo -e -n '%D-%m-%Y'`"
AC_MSG_RESULT($built_date)
AC_SUBST(built_date)

])dnl
dnl

