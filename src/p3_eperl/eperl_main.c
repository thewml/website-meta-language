#include "config_sc.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
**
**  CU() -- CleanUp Makro (implemented in a safety way)
**
*/
#define CU(returncode) do { rc = returncode; goto CUS; } while (0)

/*  first include the standard Perl
 *      includes designed for embedding   */
#define PERL_NO_GET_CONTEXT     /* for efficiency reasons, see perlguts(3) */
#include <EXTERN.h>
#include <perl.h>

#include "eperl_perl5_sm.h"

int Perl5_Run(int myargc, char **myargv, char **env, char *perlstderr, char *perlstdout)
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
        CU(-1);
    }

    /* open a file for Perl's STDERR channel
       and redirect stderr to the new channel */
    if ((er = fopen(perlstderr, "w")) == NULL) {
        fprintf(stderr, "Cannot open STDERR file `%s' for writing", perlstderr);
        CU(-1);
    }

    my_perl = perl_alloc();
    perl_construct(my_perl);

    /*  now parse the script!
        NOTICE: At this point, the script gets
        only _parsed_, not evaluated/executed!  */
    rc = perl_parse(my_perl, NULL, myargc, myargv, env);
    if (rc != 0) {
        fclose(er); er = NULL;
        fprintf(stderr, "Perl parsing error (interpreter rc=%d) error=%s", rc, SvTRUE(ERRSV) ? SvPV_nolen(ERRSV) : "");
        CU(-1);
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

/*
 *  main procedure
 */
int main(int argc, char **argv, char **env)
{
    int rc;
    FILE *fp = NULL;
    char *perlscript = "ePerl.script";
    int myargc;
    char *myargv[20];
    char *cpScript = "print \"foo\";\nprint \"\\n\";\n";

#ifndef DEBUG_ENABLED
    unlink(perlscript);
#endif
    if ((fp = fopen(perlscript, "w")) == NULL) {
        fprintf(stderr, "Cannot open Perl script file `%s' for writing", perlscript);
        CU(-1);
    }
    if (fwrite(cpScript, strlen(cpScript), 1, fp) != 1) {
        fprintf(stderr, "Cannot write to Perl script file `%s'", perlscript);
        CU(-1);
    }
    fclose(fp); fp = NULL;

    if (1) {
        fp = stderr;
        fprintf(fp, "----internally created Perl script-----------------------------------\n");
        if (fwrite(cpScript, strlen(cpScript)-1, 1, fp) != 1)
        {
            fprintf(stderr, "%s\n", "Cannot write");
            CU(-1);
        }
        if (cpScript[strlen(cpScript)-1] == '\n')
            fprintf(fp, "%c", cpScript[strlen(cpScript)-1]);
        else
            fprintf(fp, "%c\n", cpScript[strlen(cpScript)-1]);
        fprintf(fp, "----internally created Perl script-----------------------------------\n");
        fp = NULL;
    }

    /* temporary filename for Perl's STDOUT channel */
    char *perlstdout= "/tmp/ePerl.stdout";
#ifndef DEBUG_ENABLED
    unlink(perlstdout);
#endif

    /* temporary filename for Perl's STDERR channel */
    char * perlstderr= "ePerl.stderr";
#ifndef DEBUG_ENABLED
    unlink(perlstderr);
#endif

    /*  create command line...  */
    myargc = 0;
    /*  - program name and possible -T -w options */
    myargv[myargc++] = argv[0];
    /*  - and the script itself  */
    myargv[myargc++] = perlscript;

    rc = Perl5_Run(myargc, myargv, env, perlstderr, perlstdout);
    if (rc != 0) {
        if (rc == -1)
            CU(0);
        CU(-1);
    }

    CUS: /* the Clean Up Sequence */

    /* close all still open file handles */
    if (fp)
        fclose(fp);

    exit(rc);
}

/*EOF*/
