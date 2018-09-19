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

/* eperl_main.c */
extern int mode;
extern char *allowed_file_ext[];
extern char *allowed_caller_uid[];
extern void PrintError(int mode, char *scripturl, char *scriptfile, char *logfile, char *str, ...);
extern void give_version(void);
extern void give_usage(char *name);
extern char *RememberedINC[1024];
extern void RememberINC(char *str);
extern void mysighandler(int rc);
extern void myinit(void);
extern void myexit(int rc);

/* eperl_parse.c */
extern char *ePerl_begin_delimiter;
extern char *ePerl_end_delimiter;
extern int ePerl_case_sensitive_delimiters;
extern int ePerl_convert_entities;
extern int ePerl_line_continuation;
extern void ePerl_SetError(char *str, ...);
extern char *ePerl_GetError(void);
extern char *ePerl_fprintf(char *cpOut, char *str, ...);
extern char *ePerl_fnprintf(char *cpOut, int *n, char *str, ...);
extern char *ePerl_fwrite(char *cpBuf, int nBuf, int cNum, char *cpOut);
extern char *ePerl_fnwrite(char *cpBuf, int nBuf, int cNum, char *cpOut, int *cpOutLen);
extern char *ePerl_Efwrite(char *cpBuf, int nBuf, int cNum, char *cpOut);
extern char *ePerl_Efnwrite(char *cpBuf, int nBuf, int cNum, char *cpOut, int *n);
extern char *ePerl_Cfwrite(char *cpBuf, int nBuf, int cNum, char *cpOut);
extern char *ePerl_Cfnwrite(char *cpBuf, int nBuf, int cNum, char *cpOut, int *cpOutLen);
extern char *ep_strnchr(char *buf, char chr, int n);
extern char *ep_strnstr(char *buf, char *str, int n);
extern char *ep_strncasestr(char *buf, char *str, int n);
extern char *ePerl_Bristled2Plain(char *cpBuf);

/* eperl_perl5.c */
extern void give_version_extended_perl(void);
extern int Perl5_Run(int myargc, char **myargv, int mode, int fCheck, int keepcwd, char *source, char **env, char *perlscript, char *perlstderr, char *perlstdout);
extern void Perl5_RememberScalar(char *str);

/* eperl_debug.c */
extern int fDebug;
extern char *cpDebugFile;
extern void Debug(char *str, ...);

/* eperl_config.c */

/* eperl_version.c */

/*_END_PROTO_*/

#endif /* EPERL_PROTO_H */
/*EOF*/
