dnl  As this file is called configure.ac, Autoconf 2.50 is required
AC_INIT([mp4h], [1.3.0])
AC_PREREQ([2.50])

PACKAGE_DATE="06-Jul-2002"

AM_INIT_AUTOMAKE([$PACKAGE_NAME], [$PACKAGE_VERSION])
AM_MAINTAINER_MODE
AM_C_PROTOTYPES
ALL_LINGUAS=""
AM_GNU_GETTEXT
IT_PROG_INTLTOOL

dnl AC_CONFIG_SRCDIR([src/mp4h.c])
dnl AM_CONFIG_HEADER([config.h])
dnl 
dnl dnl   Now $srcdir is defined and can be used
dnl shtool="$srcdir/shtool"
dnl 
dnl AC_SUBST([PACKAGE_DATE])
dnl AC_DEFINE_UNQUOTED([PACKAGE_DATE], ["$PACKAGE_DATE"], [Release date])
dnl $shtool echo -e "Configuring for %B$PACKAGE_NAME $PACKAGE_VERSION ($PACKAGE_DATE)%b"
dnl 
dnl 
dnl if test x$datadir = 'x${prefix}/share'; then
dnl     if test x$prefix = xNONE; then
dnl         MP4HDATADIR="$ac_default_prefix/share/mp4h"
dnl     else
dnl         MP4HDATADIR="$prefix/share/mp4h"
dnl     fi
dnl else
dnl     MP4HDATADIR="$datadir/mp4h"
dnl fi
dnl if test x$libdir = 'x${exec_prefix}/lib'; then
dnl     if test x$prefix = xNONE; then
dnl         MP4HLIBDIR="$ac_default_prefix/lib/mp4h"
dnl     else
dnl         MP4HLIBDIR="$prefix/lib/mp4h"
dnl     fi
dnl else
dnl     MP4HLIBDIR="$libdir/mp4h"
dnl fi
dnl AC_DEFINE_UNQUOTED([MP4HLIBDIR], ["$MP4HLIBDIR/$PACKAGE_VERSION:$MP4HDATADIR"],
dnl         [Default location for mp4h modules]) 
dnl 
dnl AC_PROG_CC
dnl AC_PROG_CPP
dnl AC_PROG_LIBTOOL
dnl AC_PROG_MAKE_SET
dnl AC_PROG_INSTALL 
dnl 
dnl echo ''
dnl $shtool echo -e "Checking for %Bheaders and libraries%b"
dnl AC_ISC_POSIX
dnl 
dnl AC_HEADER_STDC
dnl 
dnl AC_TYPE_SIZE_T
dnl AC_C_CONST
dnl 
dnl AC_CHECK_HEADERS([limits.h locale.h memory.h stdarg.h string.h unistd.h time.h])
dnl AC_CHECK_HEADERS([pwd.h grp.h dirent.h sys/param.h sys/stat.h sys/types.h sys/times.h])
dnl 
dnl AC_CHECK_FUNCS([mkstemp strchr strerror tmpfile getpwuid getgrgid bcopy memmove])
dnl AC_FUNC_ALLOCA
dnl AC_FUNC_VPRINTF
dnl AC_REPLACE_FUNCS(strtol obstack xmalloc xstrdup)
dnl AC_CHECK_FUNC(getopt_long, , [
dnl    AC_LIBOBJ([getopt1])
dnl    AC_LIBOBJ([getopt])
dnl ])
dnl AC_MSG_CHECKING([for HTML postprocessing])
dnl AC_ARG_WITH(tidy,dnl
dnl [  --with-tidy=PATH        post-process HTML documentation with tidy],[dnl
dnl TIDY=$with_tidy
dnl test "$TIDY" = yes && TIDY=tidy
dnl AC_MSG_RESULT([$TIDY])
dnl ],[
dnl TIDY="@:"
dnl AC_MSG_RESULT([no])
dnl ])
dnl AC_SUBST([TIDY])
dnl 
dnl ALL_LINGUAS=
dnl 
dnl 
dnl if test ".$USE_INCLUDED_LIBINTL" = .yes; then
dnl     INTLINCL='-I$(top_srcdir)/intl'
dnl     AC_SUBST([INTLINCL])
dnl fi
dnl 
dnl MP4H_LOADABLE_MODULES
dnl 
dnl dnl    Must be called after MP4H_LOADABLE_MODULES
dnl AM_WITH_DMALLOC
dnl 
dnl module_list='intl:gettext system'
dnl 
dnl for m in $module_list
dnl do
dnl     eval "_do_module_`echo $m | sed -e 's/[[^a-z]]/_/g'`=0"
dnl done
dnl 
dnl test ".$with_modules" = .yes && with_modules=`echo $module_list | sed -e 's/  */,/g'`
dnl 
dnl if test ".$with_modules" != .no; then
dnl     echo ''
dnl     $shtool echo -e "Checking for %Bloadable modules%b support"
dnl     AC_LIBTOOL_WIN32_DLL
dnl     AC_LIBLTDL_CONVENIENCE
dnl     AC_ENABLE_SHARED
dnl     AC_DISABLE_STATIC
dnl     AC_LIBTOOL_DLOPEN
dnl     AC_LIBTOOL_SETUP
dnl     AC_PROG_LIBTOOL
dnl 
dnl     case ,$with_modules, in
dnl       *,intl:gettext,* )
dnl         if test ".$nls_cv_use_gnu_gettext" = .yes; then
dnl             _do_module_intl_gettext=1
dnl         else
dnl             AC_CHECK_HEADER([libintl.h], [_do_module_intl_gettext=1])
dnl         fi
dnl         ;;
dnl     esac
dnl     case ,$with_modules, in
dnl       *,javascript:mozjs,* )
dnl         AC_CHECK_HEADER([js/jsapi.h],
dnl             [AC_CHECK_LIB([js], [JS_NewRuntime],
dnl                           [_do_module_javascript_mozjs=1])])
dnl         ;;
dnl     esac
dnl     case ,$with_modules, in
dnl       *,javascript:njs,* )
dnl         AC_CHECK_HEADER([js.h],
dnl             [AC_CHECK_LIB([js], [js_free],
dnl                           [_do_module_javascript_njs=1])])
dnl         ;;
dnl     esac
dnl     case ,$with_modules, in
dnl       *,system,* ) _do_module_system=1 ;;
dnl     esac
dnl     case ,$with_modules, in
dnl       *,test,* ) _do_module_test=1 ;;
dnl     esac
dnl     case ,$with_modules, in
dnl       *,wml,* ) _do_module_wml=1 ;;
dnl     esac
dnl 
dnl     for m in $module_list
dnl     do
dnl         eval "test .\$_do_module_`echo $m | sed -e 's/[[^a-z]]/_/g'` = .1" || with_modules=`echo ,$with_modules, | sed -e "s/,$m,/,/" -e 's/^,//' -e 's/,$//'`
dnl     done
dnl     with_modules=`echo $with_modules | sed -e 's/,/ /g'`
dnl else
dnl     with_modules=
dnl fi
dnl 
dnl test ".$with_modules" = . && with_modules=none
dnl 
dnl AC_LIB_LTDL
dnl AC_SUBST(INCLTDL)
dnl AC_SUBST(LIBLTDL)
dnl 
dnl AC_MSG_CHECKING([which modules to compile])
dnl AC_MSG_RESULT([$with_modules])
dnl 
dnl AM_CONDITIONAL([LOADABLE_MODULES], [test ".$with_modules" != .none])
dnl 
dnl AM_CONDITIONAL([DO_MODULE_INTL_GETTEXT], [test ".$_do_module_intl_gettext" = .1])
dnl AM_CONDITIONAL([DO_MODULE_JAVASCRIPT_MOZJS], [test ".$_do_module_javascript_mozjs" = .1])
dnl AM_CONDITIONAL([DO_MODULE_JAVASCRIPT_NJS], [test ".$_do_module_javascript_njs" = .1])
dnl AM_CONDITIONAL([DO_MODULE_SYSTEM], [test ".$_do_module_system" = .1])
dnl AM_CONDITIONAL([DO_MODULE_TEST], [test ".$_do_module_test" = .1])
dnl AM_CONDITIONAL([DO_MODULE_WML], [test ".$_do_module_wml" = .1])
dnl 
dnl dnl  This line is needed for Debian builds
dnl AC_SUBST(CFLAGS)
dnl 
dnl AC_CONFIG_FILES([
dnl    Makefile
dnl    intl/Makefile
dnl    pcre/Makefile
dnl    lib/Makefile
dnl    src/Makefile
dnl    po/Makefile.in
dnl    modules/Makefile
dnl    modules/system/Makefile
dnl    modules/intl/Makefile
dnl    doc/Makefile
dnl    tests/Makefile
dnl ])
dnl AC_OUTPUT
dnl 
dnl echo ''
dnl $shtool echo -e "Now please type %Bmake%b to compile mp4h"
dnl echo ''
