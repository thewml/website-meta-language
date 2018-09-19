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
**  eperl_proto.h -- ePerl ANSI C prototypes
*/
#ifndef EPERL_PROTO_H
#define EPERL_PROTO_H 1

/*_BEGIN_PROTO_*/

/* eperl_perl5.c */
extern int Perl5_Run(int myargc, char **myargv, int mode, int fCheck, int keepcwd, char *source, char **env, char *perlscript, char *perlstderr, char *perlstdout);
extern void Perl5_RememberScalar(char *str);

/* eperl_config.c */

/* eperl_version.c */

/*_END_PROTO_*/

#endif /* EPERL_PROTO_H */
/*EOF*/
