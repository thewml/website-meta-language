dnl  As this file is called configure.ac, Autoconf 2.50 is required
AC_PREREQ([2.50])

AC_INIT([mp4h], [1.3.0])
PACKAGE_DATE="06-Jul-2002"

AC_CONFIG_SRCDIR([src/mp4h.c])
AM_CONFIG_HEADER([config.h])

dnl   Now $srcdir is defined and can be used
shtool=$srcdir/shtool

AC_SUBST([PACKAGE_DATE])
AC_DEFINE_UNQUOTED([PACKAGE_DATE], ["$PACKAGE_DATE"], [Release date])
$shtool echo -e "Configuring for %B$PACKAGE_NAME $PACKAGE_VERSION ($PACKAGE_DATE)%b"

AM_INIT_AUTOMAKE([$PACKAGE_NAME], [$PACKAGE_VERSION])
AM_MAINTAINER_MODE

if test x$datadir = 'x${prefix}/share'; then
    if test x$prefix = xNONE; then
        MP4HDATADIR="$ac_default_prefix/share/mp4h"
    else
        MP4HDATADIR="$prefix/share/mp4h"
    fi
else
    MP4HDATADIR="$datadir/mp4h"
fi
if test x$libdir = 'x${exec_prefix}/lib'; then
    if test x$prefix = xNONE; then
        MP4HLIBDIR="$ac_default_prefix/lib/mp4h"
    else
        MP4HLIBDIR="$prefix/lib/mp4h"
    fi
else
    MP4HLIBDIR="$libdir/mp4h"
fi
AC_DEFINE_UNQUOTED([MP4HLIBDIR], ["$MP4HLIBDIR/$PACKAGE_VERSION:$MP4HDATADIR"],
        [Default location for mp4h modules]) 

AC_PROG_CC
AC_PROG_CPP
AC_PROG_LIBTOOL
AC_PROG_MAKE_SET
AC_PROG_INSTALL 

echo ''
$shtool echo -e "Checking for %Bheaders and libraries%b"
AC_AIX
AC_MINIX
AC_ISC_POSIX

AM_C_PROTOTYPES
AC_HEADER_STDC

AC_TYPE_SIZE_T
AC_C_CONST

AC_CHECK_HEADERS([limits.h locale.h memory.h stdarg.h string.h unistd.h time.h])
AC_CHECK_HEADERS([pwd.h grp.h dirent.h sys/param.h sys/stat.h sys/types.h sys/times.h])

AC_CHECK_FUNCS([mkstemp strchr strerror tmpfile getpwuid getgrgid bcopy memmove])
AC_FUNC_ALLOCA
AC_FUNC_VPRINTF
AC_REPLACE_FUNCS(strtol obstack xmalloc xstrdup)
AC_CHECK_FUNC(getopt_long, , [
   AC_LIBOBJ([getopt1])
   AC_LIBOBJ([getopt])
])
AC_MSG_CHECKING([for HTML postprocessing])
AC_ARG_WITH(tidy,dnl
[  --with-tidy=PATH        post-process HTML documentation with tidy],[dnl
TIDY=$with_tidy
test "$TIDY" = yes && TIDY=tidy
AC_MSG_RESULT([$TIDY])
],[
TIDY="@:"
AC_MSG_RESULT([no])
])
AC_SUBST([TIDY])

ALL_LINGUAS=
AM_GNU_GETTEXT


if test ".$USE_INCLUDED_LIBINTL" = .yes; then
    INTLINCL='-I$(top_srcdir)/intl'
    AC_SUBST([INTLINCL])
fi

MP4H_LOADABLE_MODULES

dnl    Must be called after MP4H_LOADABLE_MODULES
AM_WITH_DMALLOC

module_list='intl:gettext system'

for m in $module_list
do
    eval "_do_module_`echo $m | sed -e 's/[[^a-z]]/_/g'`=0"
done

test ".$with_modules" = .yes && with_modules=`echo $module_list | sed -e 's/  */,/g'`

if test ".$with_modules" != .no; then
    echo ''
    $shtool echo -e "Checking for %Bloadable modules%b support"
    AC_LIBTOOL_WIN32_DLL
    AC_LIBLTDL_CONVENIENCE
    AC_ENABLE_SHARED
    AC_DISABLE_STATIC
    AC_LIBTOOL_DLOPEN
    AC_LIBTOOL_SETUP
    AC_PROG_LIBTOOL

    case ,$with_modules, in
      *,intl:gettext,* )
        if test ".$nls_cv_use_gnu_gettext" = .yes; then
            _do_module_intl_gettext=1
        else
            AC_CHECK_HEADER([libintl.h], [_do_module_intl_gettext=1])
        fi
        ;;
    esac
    case ,$with_modules, in
      *,javascript:mozjs,* )
        AC_CHECK_HEADER([js/jsapi.h],
            [AC_CHECK_LIB([js], [JS_NewRuntime],
                          [_do_module_javascript_mozjs=1])])
        ;;
    esac
    case ,$with_modules, in
      *,javascript:njs,* )
        AC_CHECK_HEADER([js.h],
            [AC_CHECK_LIB([js], [js_free],
                          [_do_module_javascript_njs=1])])
        ;;
    esac
    case ,$with_modules, in
      *,system,* ) _do_module_system=1 ;;
    esac
    case ,$with_modules, in
      *,test,* ) _do_module_test=1 ;;
    esac
    case ,$with_modules, in
      *,wml,* ) _do_module_wml=1 ;;
    esac

    for m in $module_list
    do
        eval "test .\$_do_module_`echo $m | sed -e 's/[[^a-z]]/_/g'` = .1" || with_modules=`echo ,$with_modules, | sed -e "s/,$m,/,/" -e 's/^,//' -e 's/,$//'`
    done
    with_modules=`echo $with_modules | sed -e 's/,/ /g'`
else
    with_modules=
fi

test ".$with_modules" = . && with_modules=none

AC_LIB_LTDL
AC_SUBST(INCLTDL)
AC_SUBST(LIBLTDL)

AC_MSG_CHECKING([which modules to compile])
AC_MSG_RESULT([$with_modules])

AM_CONDITIONAL([LOADABLE_MODULES], [test ".$with_modules" != .none])

AM_CONDITIONAL([DO_MODULE_INTL_GETTEXT], [test ".$_do_module_intl_gettext" = .1])
AM_CONDITIONAL([DO_MODULE_JAVASCRIPT_MOZJS], [test ".$_do_module_javascript_mozjs" = .1])
AM_CONDITIONAL([DO_MODULE_JAVASCRIPT_NJS], [test ".$_do_module_javascript_njs" = .1])
AM_CONDITIONAL([DO_MODULE_SYSTEM], [test ".$_do_module_system" = .1])
AM_CONDITIONAL([DO_MODULE_TEST], [test ".$_do_module_test" = .1])
AM_CONDITIONAL([DO_MODULE_WML], [test ".$_do_module_wml" = .1])

dnl  This line is needed for Debian builds
AC_SUBST(CFLAGS)

AC_CONFIG_FILES([
   Makefile
   intl/Makefile
   pcre/Makefile
   lib/Makefile
   src/Makefile
   po/Makefile.in
   modules/Makefile
   modules/system/Makefile
   modules/intl/Makefile
   doc/Makefile
   tests/Makefile
])
AC_OUTPUT

echo ''
$shtool echo -e "Now please type %Bmake%b to compile mp4h"
echo ''

