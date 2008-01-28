/* config.h.in.  Generated automatically from configure.in by autoheader.  */

/* Define if on AIX 3.
   System headers sometimes define this.
   We just want to avoid a redefinition error message.  */
#ifndef _ALL_SOURCE
#undef _ALL_SOURCE
#endif

/* Define if using alloca.c.  */
#cmakedefine C_ALLOCA

/* Define to empty if the keyword does not work.  */
#cmakedefine const

/* Define to one of _getb67, GETB67, getb67 for Cray-2 and Cray-YMP systems.
   This function is required for alloca.c support on those systems.  */
#cmakedefine CRAY_STACKSEG_END

/* Define if you have alloca, as a function or macro.  */
#cmakedefine HAVE_ALLOCA 1

/* Define if you have <alloca.h> and it should be used (not on Ultrix).  */
#cmakedefine HAVE_ALLOCA_H 1

/* Define if you don't have vprintf but do have _doprnt.  */
#cmakedefine HAVE_DOPRNT 1

/* Define if you have the vprintf function.  */
#cmakedefine HAVE_VPRINTF 1

/* Define if on MINIX.  */
#cmakedefine _MINIX

/* Define if the system does not provide POSIX.1 features except
   with this defined.  */
#cmakedefine _POSIX_1_SOURCE

/* Define if you need to in order for stat and other things to work.  */
#cmakedefine _POSIX_SOURCE

/* Define as the return type of signal handlers (int or void).  */
#cmakedefine RETSIGTYPE

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
#cmakedefine size_t

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
 STACK_DIRECTION > 0 => grows toward higher addresses
 STACK_DIRECTION < 0 => grows toward lower addresses
 STACK_DIRECTION = 0 => direction of growth unknown
 */
#cmakedefine STACK_DIRECTION

/* Define if you have the ANSI C header files.  */
#cmakedefine STDC_HEADERS

/* Define to 1 if the changeword(REGEXP) functionnality is wanted.  */
#cmakedefine ENABLE_CHANGEWORD

/* Define to 1 if you have ecvt(3), fcvt(3) and gcvt(3), define to 2 if
   these are declared in <stdlib.h>.  */
#cmakedefine HAVE_EFGCVT 1

/* Define to the name of the distribution.  */
#cmakedefine PRODUCT @PRODUCT@

/* Define to 1 if ANSI function prototypes are usable.  */
#cmakedefine PROTOTYPES

/* Define to the version of the distribution.  */
#cmakedefine VERSION @VERSION@

/* Define to 1 for better use of the debugging malloc library.  See 
   site ftp.antaire.com in antaire/src, file dmalloc/dmalloc.tar.gz.  */
#cmakedefine WITH_DMALLOC

/* Define if you have the ecvt function.  */
#cmakedefine HAVE_ECVT 1

/* Define if you have the mkstemp function.  */
#cmakedefine HAVE_MKSTEMP 1

/* Define if you have the sigaction function.  */
#cmakedefine HAVE_SIGACTION 1

/* Define if you have the sigaltstack function.  */
#cmakedefine HAVE_SIGALTSTACK 1

/* Define if you have the sigstack function.  */
#cmakedefine HAVE_SIGSTACK 1

/* Define if you have the sigvec function.  */
#cmakedefine HAVE_SIGVEC 1

/* Define if you have the strerror function.  */
#cmakedefine HAVE_STRERROR 1

/* Define if you have the strtol function.  */
#cmakedefine HAVE_STRTOL 1

/* Define if you have the tmpfile function.  */
#cmakedefine HAVE_TMPFILE 1

/* Define if you have the <limits.h> header file.  */
#cmakedefine HAVE_LIMITS_H 1

/* Define if you have the <memory.h> header file.  */
#cmakedefine HAVE_MEMORY_H 1

/* Define if you have the <siginfo.h> header file.  */
#cmakedefine HAVE_SIGINFO_H 1

/* Define if you have the <string.h> header file.  */
#cmakedefine HAVE_STRING_H 1

/* Define if you have the <unistd.h> header file.  */
#cmakedefine HAVE_UNISTD_H 1
