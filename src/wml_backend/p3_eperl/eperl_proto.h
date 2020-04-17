/*
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
#include <stdbool.h>

/*_BEGIN_PROTO_*/

/* eperl_main.c */
extern void PrintError(int mode, char *scripturl, char *scriptfile, char *logfile, char *str, ...);

/* eperl_parse.c */
extern char *ePerl_begin_delimiter;
extern char *ePerl_end_delimiter;
extern bool ePerl_case_sensitive_delimiters;
extern bool ePerl_convert_entities;
extern bool ePerl_line_continuation;
extern void ePerl_SetError(char *str, ...);
extern char *ePerl_GetError(void);
extern char *ePerl_fnwrite(char *cpBuf, int nBuf, int cNum, char *cpOut, int *cpOutLen);
extern char *ePerl_Efnwrite(char *cpBuf, int nBuf, int cNum, char *cpOut, int *n);
extern char *ePerl_Cfnwrite(char *cpBuf, int nBuf, int cNum, char *cpOut, int *cpOutLen);
extern char *ep_strnstr(char *buf, char *str, int n);
extern char *ePerl_Bristled2Plain(char *cpBuf);

/* eperl_perl5.c */
extern void give_version_extended_perl(void);
extern int Perl5_Run(int myargc, char **myargv, int mode, int fCheck, int keepcwd, char *source, char **env, char *perlscript, char *perlstderr, char *perlstdout);
extern void Perl5_RememberScalar(char *str);
extern char **Perl5_SetEnvVar(char **env, char *str);

/* eperl_pp.c */
extern char *ePerl_PP_GetError(void);
extern char *ePerl_PP(char *cpBuf, char **cppINC);

/* eperl_sys.c */
extern char **mysetenv(char **env, char *var, char *str, ...);
extern void IO_redirect_stdin(FILE *fp);
extern void IO_redirect_stdout(FILE *fp);
extern void IO_redirect_stderr(FILE *fp);
extern bool IO_is_stdin_redirected(void);
extern bool IO_is_stdout_redirected(void);
extern bool IO_is_stderr_redirected(void);
extern void IO_restore_stdin(void);
extern void IO_restore_stdout(void);
extern void IO_restore_stderr(void);
extern char *mytmpfile(const char *id);
extern void remove_mytmpfiles(void);
extern char *isotime(time_t *t);
extern char *ePerl_ReadSourceFile(char *filename, char **cpBufC, int *nBufC);
extern char *ePerl_ReadErrorFile(char *filename, char *scriptfile, char *scripturl);
extern char *filename(char *path);
extern char *dirname(char *path);
extern char *abspath(char *path);

/* eperl_http.c */
extern char *HTTP_PrintResponseHeaders(char *cpBuf);
extern void HTTP_StripResponseHeaders(char **cpBuf, int *nBuf);
extern int HTTP_IsHeaderLine(char *cp1, char *cp2);
extern int HTTP_HeadersExists(char *cpBuf);
extern int HTTP_HeaderLineExists(char *cpBuf, char *name);
extern FILE *HTTP_openURLasFP(char *url);

/* eperl_debug.c */
extern bool fDebug;
extern char *cpDebugFile;
extern void Debug(char *str, ...);

/* eperl_config.c */

/* eperl_version.c */

/* eperl_readme.c */
extern char *ePerl_README;

/* eperl_license.c */
extern char *ePerl_LICENSE;

/* eperl_logo.c */
extern int ePerl_LOGO_size;
extern unsigned char ePerl_LOGO_data[];

/* eperl_powered.c */
extern int ePerl_POWERED_size;
extern unsigned char ePerl_POWERED_data[];
/*_END_PROTO_*/

#endif /* EPERL_PROTO_H */
/*EOF*/
