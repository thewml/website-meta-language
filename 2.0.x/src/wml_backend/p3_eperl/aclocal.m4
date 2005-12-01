dnl #
dnl #  aclocal.m4 -- Local M4 functions for GNU autoconf
dnl #  Copyright (c) Ralf S. Engelschall, <rse@engelschall.com>
dnl #
dnl #  NOTICE:
dnl #      all defined macros are named AC_*
dnl #      all defined and used variables are named acl-*
dnl #
dnl
define(AC_CONFIGURE_PART,[dnl
AC_MSG_RESULT()
AC_MSG_RESULT(${TERM_BOLD}$1${TERM_NORM})
])dnl
dnl
dnl ##########################################################
dnl ##
dnl ##  Built Environment
dnl ##
dnl ##########################################################
dnl
define(AC_BUILD_USER,[dnl
AC_MSG_CHECKING(for build user)
build_user="`$shtool echo -n -e %u@%h%d`"
AC_SUBST(build_user)
AC_MSG_RESULT($build_user)
])dnl
define(AC_BUILD_TIME,[dnl
AC_MSG_CHECKING(for build time)
build_time_ctime="`date | sed -e 's/\n$//'`"
build_time_iso="`$shtool echo -n -e '%D-%m-%Y'`"
AC_MSG_RESULT($build_time_iso)
AC_SUBST(build_time_ctime)
AC_SUBST(build_time_iso)
])dnl
dnl
