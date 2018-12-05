#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*  first include the standard Perl
 *      includes designed for embedding   */
#if 0
#define PERL_NO_GET_CONTEXT     /* for efficiency reasons, see perlguts(3) */
#endif
#include <EXTERN.h>
#include <perl.h>

int Perl5_Run(int myargc, char **myargv, char **env)
{
    int rc;
    PerlInterpreter *my_perl = NULL;
    /* See https://rt.perl.org/Public/Bug/Display.html?id=133661
     * Thanks to Tony Cook
     * */
    PERL_SYS_INIT3(&myargc, &myargv, &env);

    my_perl = perl_alloc();
    perl_construct(my_perl);

    /*  now parse the script!
        NOTICE: At this point, the script gets
        only _parsed_, not evaluated/executed!  */
    rc = perl_parse(my_perl, NULL, myargc, myargv, env);
    if (rc != 0) {
        fprintf(stderr, "Perl parsing error (interpreter rc=%d) error=%s", rc, SvTRUE(ERRSV) ? SvPV_nolen(ERRSV) : "");
        goto CUS;
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
    FILE *fp = NULL;
    char *perlscript = "ePerl.script";
    char *myargv[20];
    char *cpScript = "print \"foo\";\nprint \"\\n\";\0";

    if ((fp = fopen(perlscript, "w")) == NULL) {
        fprintf(stderr, "Cannot open Perl script file `%s' for writing", perlscript);
        return -1;
    }
    if (fwrite(cpScript, strlen(cpScript), 1, fp) != 1) {
        fprintf(stderr, "Cannot write to Perl script file `%s'", perlscript);
        return -1;
    }
    fclose(fp); fp = NULL;

    fprintf(stderr, "----internally created Perl script-----------------------------------\n%s\n--end--\n", cpScript);

    if ((fp = fopen(perlscript, "r")) == NULL) {
        fprintf(stderr, "Cannot open Perl script file `%s' for reading", perlscript);
        return -1;
    }
    fprintf(stderr, "----written Perl script-----------------------------------\n");
    while (!feof(fp)) {
        int c = fgetc(fp);
        if (c < 0) {
            break;
        }
        fprintf(stderr, "%c", (unsigned char)c);
    }
    fclose(fp); fp = NULL;
    fprintf(stderr, "\n\n end of----written Perl script-----------------------------------\n");
    /*  create command line...  */
    int myargc = 0;
    /*  - program name and possible -T -w options */
    myargv[myargc++] = argv[0];
    /*  - and the script itself  */
    myargv[myargc++] = perlscript;
    myargv[myargc] = NULL;

    const int rc = Perl5_Run(myargc, myargv, env);
    return rc;
}
