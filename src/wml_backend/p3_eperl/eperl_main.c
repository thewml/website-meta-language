#include "config_sc.h"


/*
 * **
 * **  Headers from the Unix system
 * **
 * */
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


/*
 * **
 * **  OS Return Values
 * **
 * */
/* OK and FAIL exits should ALWAYS exists */
#ifndef EX_OK
#define EX_OK   0
#endif
#ifndef EX_FAIL
#define EX_FAIL 1
#endif


/*
 * **
 * **  Boolean Values -- defined in a general and portable way
 * **
 * */
/* typedef enum { false = FALSE, true = TRUE } bool; */
#undef  TRUE
#define TRUE  (0 || !(0))
#undef  FALSE
#define FALSE (!(TRUE))


/*
**
**  CU() -- CleanUp Makro (implemented in a safety way)
**
*/
#define STMT(stuff) do { stuff } while (0)
#define CU(returncode) STMT( rc = returncode; goto CUS; )
#define VCU STMT( goto CUS; )
#define RETURN_WVAL(val) return (val)
#define RETURN_EXRC return (rc)
#define RETURN_NORC return

/*  first include the standard Perl
 *      includes designed for embedding   */
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

#if !defined(WITH_THR) && (PERL_VERSION < 16)
#  define PL_defoutgv defoutgv
#endif

/*
 *   Initialization of locales when building a new Perl interpreter.
 *        Perl 5.003 calls perl_init_i18nl14n
 *             Perl 5.004 and 5.005 call perl_init_i18nl10n
 *                  In Perl 5.6.0 this routine is already called by perl_construct
 *                  */
#ifndef perl_init_i18nl10n
#  define perl_init_i18nl10n perl_init_i18nl14n
#else
#  if (PERL_REVISION > 5) || ((PERL_REVISION == 5) && (PERL_VERSION >= 6))
#    undef perl_init_i18nl10n
#    define perl_init_i18nl10n(a)
#  endif
#endif

#include "eperl_perl5_sm.h"

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

int Perl5_Run(int myargc, char **myargv, int mode, char *source, char **env, char *perlstderr, char *perlstdout)
{
    int rc;
    FILE *er;
    FILE *out;
    char *cpBuf = NULL;
    static PerlInterpreter *my_perl = NULL;
    int size;

    /* open a file for Perl's STDOUT channel
       and redirect stdout to the new channel */
    if ((out = fopen(perlstdout, "w")) == NULL) {
        fprintf(stderr, "Cannot open STDOUT file `%s' for writing", perlstdout);
        CU(mode == 0 ? -1 : EX_OK);
    }

    /* open a file for Perl's STDERR channel
       and redirect stderr to the new channel */
    if ((er = fopen(perlstderr, "w")) == NULL) {
        fprintf(stderr, "Cannot open STDERR file `%s' for writing", perlstderr);
        CU(mode == 0 ? -1 : EX_OK);
    }

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
        {
            fclose(er); er = NULL;
            fprintf(stderr, "Perl parsing error (interpreter rc=%d) error=%s", rc, SvTRUE(ERRSV) ? SvPV_nolen(ERRSV) : "");
            CU(mode == 0 ? EX_FAIL : EX_OK);
        }
    }

    /*  NOW IT IS TIME to evaluate/execute the script!!! */
    rc = perl_run(my_perl);

    /*  pre-close the handles, to be able to check
        its size and to be able to display the contents */
    fclose(out); out = NULL;
    fclose(er);  er  = NULL;

    CUS: /* the Clean Up Sequence */

    /* Ok, the script got evaluated. Now we can destroy
       and de-allocate the Perl interpreter */
    if (my_perl) {
       perl_destruct(my_perl);
       perl_free(my_perl);
    }
    return rc;
}

extern int Perl5_Run(int myargc, char **myargv, int mode, char *source, char **env, char *perlstderr, char *perlstdout);


/*
 *  Display an error message and a logfile content as a HTML page
 */
/*
 *  main procedure
 */
int main(int argc, char **argv, char **env)
{
    int rc;
    FILE *fp = NULL;
    char perlscript[1024] = "";
    char perlstderr[1024] = "";
    char perlstdout[1024] = "";
    int myargc;
    char *myargv[20];
    char *cpScript = "print \"foo\";\nprint \"\\n\";\n";

#define mode 0
    /* convert bristled source to valid Perl code */
    /* write buffer to temporary script file */
    strcpy(perlscript, "ePerl.script");
#ifndef DEBUG_ENABLED
    unlink(perlscript);
#endif
    if ((fp = fopen(perlscript, "w")) == NULL) {
        fprintf(stderr, "Cannot open Perl script file `%s' for writing", perlscript);
        CU(mode == 0 ? -1 : EX_OK);
    }
    if (fwrite(cpScript, strlen(cpScript), 1, fp) != 1) {
        fprintf(stderr, "Cannot write to Perl script file `%s'", perlscript);
        CU(mode == 0 ? -1 : EX_OK);
    }
    fclose(fp); fp = NULL;

    if (1) {
        fp = stderr;
        fprintf(fp, "----internally created Perl script-----------------------------------\n");
        if (fwrite(cpScript, strlen(cpScript)-1, 1, fp) != 1)
        {
            fprintf(stderr, "%s\n", "Cannot write");
            CU(mode == 0 ? -1 : EX_OK);
        }
        if (cpScript[strlen(cpScript)-1] == '\n')
            fprintf(fp, "%c", cpScript[strlen(cpScript)-1]);
        else
            fprintf(fp, "%c\n", cpScript[strlen(cpScript)-1]);
        fprintf(fp, "----internally created Perl script-----------------------------------\n");
        fp = NULL;
    }

    /* temporary filename for Perl's STDOUT channel */
    strcpy(perlstdout, "/tmp/ePerl.stdout");
#ifndef DEBUG_ENABLED
    unlink(perlstdout);
#endif

    /* temporary filename for Perl's STDERR channel */
    strcpy(perlstderr, "ePerl.stderr");
#ifndef DEBUG_ENABLED
    unlink(perlstderr);
#endif

    /*  create command line...  */
    myargc = 0;
    /*  - program name and possible -T -w options */
    myargv[myargc++] = argv[0];
    /*  - and the script itself  */
    myargv[myargc++] = perlscript;

    rc = Perl5_Run(myargc, myargv, mode, "", env, perlstderr, perlstdout);
    if (rc != 0) {
        if (rc == -1)
            CU(EX_OK);
        CU(mode == 0 ? EX_FAIL : EX_OK);
    }

    CUS: /* the Clean Up Sequence */

    /* close all still open file handles */
    if (fp)
        fclose(fp);

    exit(rc);
}

/*EOF*/
