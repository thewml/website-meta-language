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

int Perl5_Run(int myargc, char **myargv, char **env)
{
    int rc;
    PerlInterpreter *my_perl = NULL;

    my_perl = perl_alloc();
    perl_construct(my_perl);

    /*  now parse the script!
        NOTICE: At this point, the script gets
        only _parsed_, not evaluated/executed!  */
    rc = perl_parse(my_perl, NULL, myargc, myargv, env);
    if (rc != 0) {
        fprintf(stderr, "Perl parsing error (interpreter rc=%d) error=%s", rc, SvTRUE(ERRSV) ? SvPV_nolen(ERRSV) : "");
        CU(-1);
    }

    /*  NOW IT IS TIME to evaluate/execute the script!!! */
    rc = perl_run(my_perl);

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

    if ((fp = fopen(perlscript, "w")) == NULL) {
        fprintf(stderr, "Cannot open Perl script file `%s' for writing", perlscript);
        CU(-1);
    }
    if (fwrite(cpScript, strlen(cpScript), 1, fp) != 1) {
        fprintf(stderr, "Cannot write to Perl script file `%s'", perlscript);
        CU(-1);
    }
    fclose(fp); fp = NULL;

    fprintf(stderr, "----internally created Perl script-----------------------------------\n%s\n--end--\n", cpScript);

    /*  create command line...  */
    myargc = 0;
    /*  - program name and possible -T -w options */
    myargv[myargc++] = argv[0];
    /*  - and the script itself  */
    myargv[myargc++] = perlscript;

    rc = Perl5_Run(myargc, myargv, env);
    if (rc != 0) {
        CU(-1);
    }

    CUS: /* the Clean Up Sequence */

    exit(rc);
}
