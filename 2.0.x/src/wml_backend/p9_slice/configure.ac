dnl ##
dnl ##  configure.in -- GNU autoconf configuration source
dnl ##

AC_PREREQ(2.50)dnl
AC_REVISION($Revision: 1.1 $)dnl

AC_INIT

shtool="./etc/shtool"

dnl ##  version id
SLICE_VERSION="`$shtool version -l perl -d long slice_vers.pl`"
$shtool echo -e "%BConfiguring for Slice $SLICE_VERSION %b"

AC_CONFIG_SRCDIR([README])
AC_PREFIX_DEFAULT(/usr/local)

AC_PROG_MAKE_SET

test "x$prefix" = xNONE && prefix=$ac_default_prefix
eval "dir=$prefix"
case $dir in
    *slice* ) libsubdir= ;;
          * ) libsubdir="/slice" ;;
esac
AC_SUBST(libsubdir)

AC_CHECK_PERL_INTERPRETER
AC_CHECK_PERL_MM_PATHS
AC_ARG_WITH(closedworld,dnl
[  --with-closedworld      force the private install of all Perl modules],[
have_getoptlong=0
have_bitvector=0
have_io=0
],)dnl
AC_CHECK_PERL_MODULE(Getopt::Long, 2.13, getoptlong)
AC_CHECK_PERL_MODULE(Bit::Vector, 5.0, bitvector)
AC_CHECK_PERL_MODULE(IO::File, 1.07, io)

AC_CONFIG_FILES([
        Makefile
        t/Makefile
        lib/Makefile
])

AC_OUTPUT

dnl ##EOF##
