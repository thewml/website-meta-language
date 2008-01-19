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
**  eperl_perl5.c -- ePerl Perl5 related stuff
*/

#include "eperl_config.h"
#include "eperl_global.h"
#include "eperl_perl5.h"
#include "eperl_perl5_sm.h"
#include "eperl_proto.h"

#ifdef HAVE_PERL_DYNALOADER

extern void boot_DynaLoader _((pTHX_ CV* cv));

void give_version_extended_perl(void)
{
    give_version();
    fprintf(stdout, "Characteristics of this binary:\n");
    fprintf(stdout, "  Perl Version    : %s (%s)\n", AC_perl_vers, AC_perl_prog);
    fprintf(stdout, "  Perl I/O Layer  : %s\n", PERL_IO_LAYER_ID);
    fprintf(stdout, "  Perl Library    : %s/CORE/libperl.a\n", AC_perl_archlib);
    fprintf(stdout, "  Perl DynaLoader : %s\n", AC_perl_dla);
    fprintf(stdout, "  System Libs     : %s\n", AC_perl_libs);
    fprintf(stdout, "  Built User      : %s\n", AC_build_user);
    fprintf(stdout, "  Built Time      : %s\n", AC_build_time_iso);
    fprintf(stdout, "\n");
}

/*
**
**  the Perl XS init function for dynamic library loading
**
*/
void Perl5_XSInit(pTHX)
{
   char *file = __FILE__;
   /* dXSUB_SYS; */
   /* dummy = 0; */ /* make gcc -Wall happy ;-) */

   /* do newXS() the available modules */
   DO_NEWXS_STATIC_MODULES
}
#endif /* HAVE_PERL_DYNALOADER */

/*
**
**  Force Perl to use unbuffered I/O
**
*/
void Perl5_ForceUnbufferedStdout(pTHX)
{
    dTHR;
    IoFLAGS(GvIOp(PL_defoutgv)) |= IOf_FLUSH; /* $|=1 */
    return;
}

/*
**
**  set a Perl environment variable
**
*/
char **Perl5_SetEnvVar(char **env, char *str) 
{
    char ca[1024];
    char *cp;

    strncpy(ca, str, 1023);
    ca[1023] = NUL;
    cp = strchr(ca, '=');
    if (cp != NULL)
        *cp++ = '\0';
    else
        cp = "";
    return mysetenv(env, ca, cp);
}

/*
**
**  sets a Perl scalar variable
**
*/
void Perl5_SetScalar(pTHX_ char *pname, char *vname, char *vvalue)
{
    dTHR;
    ENTER;
    save_hptr(&PL_curstash); 
    PL_curstash = gv_stashpv(pname, TRUE);
    sv_setpv(perl_get_sv(vname, TRUE), vvalue);
    LEAVE;
    return;
}

/*
**
**  remember a Perl scalar variable
**  and set it later
**
**  (this is needed because we have to
**   remember the scalars when parsing 
**   the command line, but actually setting
**   them can only be done later when the
**   Perl 5 interpreter is allocated !!)
**
*/

char *Perl5_RememberedScalars[1024] = { NULL };

void Perl5_RememberScalar(char *str) 
{
    int i;

    for (i = 0; Perl5_RememberedScalars[i] != NULL; i++)
        ;
    Perl5_RememberedScalars[i++] = strdup(str);
    Perl5_RememberedScalars[i++] = NULL;
    return;
}

void Perl5_SetRememberedScalars(pTHX) 
{
    char ca[1024];
    char *cp;
    int i;

    for (i = 0; Perl5_RememberedScalars[i] != NULL; i++) {
        strncpy(ca, Perl5_RememberedScalars[i], 1023);
        ca[1023] = NUL;
        cp = strchr(ca, '=');
        if (cp != NULL)
            *cp++ = '\0';
        else
            cp = "";
        Perl5_SetScalar(aTHX_ "main", ca, cp);
    }
}

int Perl5_Run(int myargc, char **myargv, int mode, int fCheck, int keepcwd, char *source, char **env, char *perlscript, char *perlstderr, char *perlstdout)
{
    DECL_EXRC;
    FILE *er;
    FILE *out;
    char *cpBuf = NULL;
    char sourcedir[2048];
    char *cp;
    static PerlInterpreter *my_perl = NULL; 
    struct stat st;
    int size;
    char cwd[MAXPATHLEN];

    /* open a file for Perl's STDOUT channel
       and redirect stdout to the new channel */
    if ((out = fopen(perlstdout, "w")) == NULL) {
        PrintError(mode, source, NULL, NULL, "Cannot open STDOUT file `%s' for writing", perlstdout);
        CU(mode == MODE_FILTER ? EX_IOERR : EX_OK);
    }
    IO_redirect_stdout(out);

    /* open a file for Perl's STDERR channel 
       and redirect stderr to the new channel */
    if ((er = fopen(perlstderr, "w")) == NULL) {
        PrintError(mode, source, NULL, NULL, "Cannot open STDERR file `%s' for writing", perlstderr);
        CU(mode == MODE_FILTER ? EX_IOERR : EX_OK);
    }
    IO_redirect_stderr(er);

    my_perl = perl_alloc();   
    perl_construct(my_perl); 
    perl_init_i18nl10n(1);

    /*  now parse the script! 
        NOTICE: At this point, the script gets 
        only _parsed_, not evaluated/executed!  */
#ifdef HAVE_PERL_DYNALOADER
    rc = perl_parse(my_perl, Perl5_XSInit, myargc, myargv, env);
#else
    rc = perl_parse(my_perl, NULL, myargc, myargv, env);
#endif
    if (rc != 0) { 
        if (fCheck && mode == MODE_FILTER) {
            fclose(er); er = NULL;
            IO_restore_stdout();
            IO_restore_stderr();
            if ((cpBuf = ePerl_ReadErrorFile(perlstderr, perlscript, source)) != NULL) {
                fprintf(stderr, cpBuf);
            }
            CU(EX_FAIL);
        }
        else {
            fclose(er); er = NULL;
            PrintError(mode, source, perlscript, perlstderr, "Perl parsing error (interpreter rc=%d)", rc);
            CU(mode == MODE_FILTER ? EX_FAIL : EX_OK);
        }
    }

    /* Stop when we are just doing a syntax check */
    if (fCheck && mode == MODE_FILTER) {
        fclose(er); er = NULL;
        IO_restore_stdout();
        IO_restore_stderr();
        fprintf(stderr, "%s syntax OK\n", source);
        CU(-1);
    }

    /* change to directory of script:
       this actually is not important to us, but really useful 
       for the ePerl source file programmer!! */
    cwd[0] = NUL;
    if (!keepcwd) {
        /* if running as a Unix filter remember the cwd for outputfile */
        if (mode == MODE_FILTER)
            getcwd(cwd, MAXPATHLEN);
        /* determine dir of source file and switch to it */
        strncpy(sourcedir, source, sizeof(sourcedir));
        sourcedir[sizeof(sourcedir)-1] = NUL;
        for (cp = sourcedir+strlen(sourcedir); cp > sourcedir && *cp != '/'; cp--)
            ;
        *cp = NUL;
        chdir(sourcedir);
    }

    /*  Set the previously remembered Perl 5 scalars (option -d) */
    Perl5_SetRememberedScalars(aTHX);

    /*  Force unbuffered I/O */
    Perl5_ForceUnbufferedStdout(aTHX);

    /*  NOW IT IS TIME to evaluate/execute the script!!! */
    rc = perl_run(my_perl);

    /*  pre-close the handles, to be able to check
        its size and to be able to display the contents */
    fclose(out); out = NULL;
    fclose(er);  er  = NULL;

    /* ok, now recover the stdout and stderr */
    IO_restore_stdout();
    IO_restore_stderr();

    /*  when the Perl interpreter failed or there
        is data on stderr, we print a error page */
    if (stat(perlstderr, &st) == 0)
        size = st.st_size;
    else
        size = 0;
    if (rc != 0 || size > 0) {
        PrintError(mode, source, perlscript, perlstderr, "Perl runtime error (interpreter rc=%d)", rc);
        CU(mode == MODE_FILTER ? EX_FAIL : EX_OK);
    }

    CUS: /* the Clean Up Sequence */

    /* Ok, the script got evaluated. Now we can destroy 
       and de-allocate the Perl interpreter */
    if (my_perl) {
       perl_destruct(my_perl);                                                    
       perl_free(my_perl);
    }
    return rc;
}

/*EOF*/
