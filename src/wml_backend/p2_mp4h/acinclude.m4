# Written by René Seindal (rene@seindal.dk)
# Hacked by Denis Barbier for mp4h  <barbier@engelschall.com>
#
# This file can be copied and used freely without restrictions.  It can
# be used in projects which are not available under the GNU Public License
# but which still want to provide support for the GNU gettext functionality.
# Please note that the actual code is *not* freely available.

# serial 1

AC_DEFUN(MP4H_LOADABLE_MODULES,
  [AC_MSG_CHECKING(if support for loadable modules is requested)
  AC_ARG_WITH(modules,
  [  --with-modules=x,y,z    select loadable modules to compile],
  [with_modules=$withval], [with_modules=yes])
  AC_MSG_RESULT($with_modules)

  if test ".$with_modules" != .no; then
    dnl We might no have it anyway, after all.
    select_modules=$with_modules
    with_modules=no

    dnl Test for dlopen in libc
    AC_CHECK_FUNCS([dlopen])
    if test "$ac_cv_func_dlopen" = yes; then
       with_modules=yes
    fi

    dnl Test for dlopen in libdl
    if test "$with_modules" = no; then
      AC_CHECK_LIB(dl, dlopen)
      if test "$ac_cv_lib_dl_dlopen" = yes; then
	with_modules=yes

#	LIBS="$LIBS -ldl"
	AC_DEFINE(HAVE_DLOPEN,1,
                [Define to 1 if the -ldl library should be used])
      fi
    fi

#    dnl Test for dld_link in libdld
#    if test "$with_modules" = no; then
#      AC_CHECK_LIB(dld, dld_link)
#      if test "$ac_cv_lib_dld_dld_link" = "yes"; then
#	 with_modules=yes
#	 AC_DEFINE(HAVE_DLD,1)
#      fi
#    fi

    dnl Test for shl_load in libdld
    if test "$with_modules" = no; then
       AC_CHECK_LIB(dld, shl_load)
       if test "$ac_cv_lib_dld_shl_load" = yes; then
	  with_modules=yes

#	  LIBS="$LIBS -ldld"
	  AC_DEFINE(HAVE_SHL_LOAD, 1,
                [Define to 1 if the -ldld library should be used])
       fi
    fi

    if test "$with_modules" != no; then
      dnl This is for libtool
      DLLDFLAGS=-export-dynamic

      AC_DEFINE([WITH_MODULES], 1,
        [Define to 1 if there is support for dynamic loading of modules.])
    fi

    if test "$with_modules" = no; then
      AC_MSG_WARN([Loadable modules have not been found on your computer, this feature is disabled])
      select_modules=
    fi
    AC_SUBST(DLLDFLAGS)
    with_modules=$select_modules
  fi
  ])

