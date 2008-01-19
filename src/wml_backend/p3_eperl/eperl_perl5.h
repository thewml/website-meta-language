/*
**        ____           _ 
**    ___|  _ \ ___ _ __| |
**   / _ \ |_) / _ \ '__| |
**  |  __/  __/  __/ |  | |
**   \___|_|   \___|_|  |_|
** 
**  ePerl -- Embedded Perl 5 Language
**
**  ePerl interprets an ASCII file bristled with Perl 5 program statements
**  by evaluating the Perl 5 code while passing through the plain ASCII
**  data. It can operate both as a standard Unix filter for general file
**  generation tasks and as a powerful Webserver scripting language for
**  dynamic HTML page programming. 
**
**  ======================================================================
**
**  Copyright (c) 1996,1997,1998,1999 Ralf S. Engelschall <rse@engelschall.com>
**
**  This program is free software; it may be redistributed and/or modified
**  only under the terms of either the Artistic License or the GNU General
**  Public License, which may be found in the ePerl source distribution.
**  Look at the files ARTISTIC and COPYING or run ``eperl -l'' to receive
**  a built-in copy of both license files.
**
**  This program is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See either the
**  Artistic License or the GNU General Public License for more details.
**
**  ======================================================================
**
**  eperl_perl5.h -- Perl 5 header file mangling
*/
#ifndef EPERL_PERL5_H
#define EPERL_PERL5_H 1


/*  first include the standard Perl 
    includes designed for embedding   */
#define PERL_NO_GET_CONTEXT     /* for efficiency reasons, see perlguts(3) */
#include <EXTERN.h>
#include <perl.h>                 

#ifndef dTHR
#  ifdef WIN32
#       define dTHR extern int Perl___notused
#  else
#       define dTHR extern int errno
#  endif
#endif

#ifndef aTHX
#  define aTHX
#  define aTHX_
#  define pTHX void
#  define pTHX_
#endif

/*  define the I/O type string for verbosity */
#ifdef USE_PERLIO
#  ifdef USE_SFIO
#    define PERL_IO_LAYER_ID "PerlIO/SfIO"
#  else
#    define PERL_IO_LAYER_ID "PerlIO/StdIO"
#  endif
#else
#  define PERL_IO_LAYER_ID "Raw/StdIO"
#endif

#if (PERL_VERSION < 4) || ((PERL_VERSION == 4) && (PERL_SUBVERSION <= 5))
#  define PL_curstash curstash
#endif

#ifndef WITH_THR
#  define PL_defoutgv defoutgv
#endif

/*  
  Initialization of locales when building a new Perl interpreter.
     Perl 5.003 calls perl_init_i18nl14n
     Perl 5.004 and 5.005 call perl_init_i18nl10n
     In Perl 5.6.0 this routine is already called by perl_construct
*/
#ifndef perl_init_i18nl10n
#  define perl_init_i18nl10n perl_init_i18nl14n
#else
#  if (PERL_REVISION > 5) || ((PERL_REVISION == 5) && (PERL_VERSION >= 6))
#    undef perl_init_i18nl10n
#    define perl_init_i18nl10n(a)
#  endif
#endif

/* eperl_perl5.c */
/*  These prototypes can no longer be included in eperl_proto.h because
    pTHX argument has been introduced in Perl 5.6.0  */
extern void Perl5_XSInit(pTHX);
extern void Perl5_ForceUnbufferedStdout(pTHX);
extern char **Perl5_SetEnvVar(char **env, char *str);
extern void Perl5_SetScalar(pTHX_ char *pname, char *vname, char *vvalue);
extern char *Perl5_RememberedScalars[1024];
extern void Perl5_RememberScalar(char *str);
extern void Perl5_SetRememberedScalars(pTHX);

#endif /* EPERL_PERL5_H */
/*EOF*/
