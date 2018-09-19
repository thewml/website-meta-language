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
**  eperl_main.c -- ePerl main procedure
*/

#include <errno.h>

#include "eperl_config.h"
#include "eperl_global.h"
#include "eperl_proto.h"

/*
 *  Display an error message and a logfile content as a HTML page
 */
void PrintError(int mode, char *scripturl, char *scriptfile, char *logfile, char *str, ...)
{
    va_list ap;
    va_start(ap, str);
    vfprintf(stderr, str, ap);
    va_end(ap);
    return;
}

void give_usage(char *name)
{
    fprintf(stderr, "Usage: %s [options] [scriptfile]\n", name);
    fprintf(stderr, "\n");
    fprintf(stderr, "Input Options:\n");
    fprintf(stderr, "  -d, --define=NAME=VALUE   define global Perl variable ($main::name)\n");
    fprintf(stderr, "  -I, --includedir=PATH     add @INC/#include directory\n");
    fprintf(stderr, "  -B, --block-begin=STR     set begin block delimiter\n");
    fprintf(stderr, "  -E, --block-end=STR       set end block delimiter\n");
    fprintf(stderr, "  -n, --nocase              force block delimiters to be case insensitive\n");
    fprintf(stderr, "  -k, --keepcwd             force keeping of current working directory\n");
    fprintf(stderr, "  -P, --preprocess          enable ePerl Preprocessor\n");
    fprintf(stderr, "  -L, --line-continue       enable line continuation via backslashes\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Output Options:\n");
    fprintf(stderr, "  -T, --tainting            enable Perl Tainting\n");
    fprintf(stderr, "  -w, --warnings            enable Perl Warnings\n");
    fprintf(stderr, "  -x, --debug               enable ePerl debugging output on console\n");
    fprintf(stderr, "  -o, --outputfile=PATH     force the output to be send to this file (default=stdout)\n");
    fprintf(stderr, "  -c, --check               run syntax check only and exit (no execution)\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Giving Feedback:\n");
    fprintf(stderr, "  -l, --license             display ePerl license files (COPYING and ARTISTIC)\n");
    fprintf(stderr, "  -h, --help                display ePerl usage list (this one)\n");
    fprintf(stderr, "\n");
}

char *RememberedINC[1024] = { NULL };

void RememberINC(char *str)
{
    int i;

    for (i = 0; RememberedINC[i] != NULL; i++)
        ;
    RememberedINC[i++] = strdup(str);
    RememberedINC[i++] = NULL;
    return;
}

void mysighandler(int rc)
{
    /* ignore more signals */
    signal(SIGINT,  SIG_IGN);
    signal(SIGTERM, SIG_IGN);

    /* give interrupt information */
    fprintf(stderr, "ePerl: **INTERRUPT**\n");

    /* exit immediately */
    exit(EX_FAIL);
}

void myinit(void)
{
    /* caught signals */
    signal(SIGINT,  mysighandler);
    signal(SIGTERM, mysighandler);
}

/*
 *  main procedure
 */
int main(int argc, char **argv, char **env)
{
    int rc;
    FILE *fp = NULL;
    char *cpBuf2 = NULL;
    char *cpBuf3 = NULL;
    char perlscript[1024] = "";
    char perlstderr[1024] = "";
    char perlstdout[1024] = "";
    char dir_tmp[1024];
    char *dir_home;
    char *dir_script;
    char ca[1024] = "";
    int myargc;
    char *myargv[20];
    char *progname;
    int nBuf;
    int nOut;
    char *source = NULL;
    char sourcedir[2048];
    char *cp;
    struct stat st;
    struct passwd *pw;
    struct passwd *pw2;
    struct group *gr;
    int uid, gid;
    int keepcwd = FALSE;
    int c;
    char *cpScript = "print \"foo\";\nprint \"\\n\";\n";
    int allow;
    int i, n, k;
    char *outputfile = NULL;
    char cwd[MAXPATHLEN];
    int fCheck = FALSE;
    int fTaint = FALSE;
    int fWarn = FALSE;
    int fNoCase = FALSE;
    int fPP = FALSE;
    char *cwd2;
    int fOkSwitch;
    char *cpHost;
    char *cpPort;
    char *cpPath;
    char *cpCGIgi;
    char *cpCGIpt;
    char *cpCGIqs;
    int fCGIqsEqualChar;

    /*  first step: our process initialisation */
    myinit();

    /*  second step: canonicalize program name */
    progname = argv[0];
    if ((cp = strrchr(progname, '/')) != NULL) {
        progname = cp+1;
    }

    /*  parse the option arguments */
    opterr = 0;
    /*
     *  determine source filename and runtime mode
     */

    if ((cpCGIgi = getenv("GATEWAY_INTERFACE")) == NULL)
        cpCGIgi = "";
    if ((cpCGIpt = getenv("PATH_TRANSLATED")) == NULL)
        cpCGIpt = "";
    if ((cpCGIqs = getenv("QUERY_STRING")) == NULL)
        cpCGIqs = "";
    fCGIqsEqualChar = FALSE;
    if (cpCGIqs != NULL && strchr(cpCGIqs, '=') != NULL)
        fCGIqsEqualChar = TRUE;

    /*
     *  Server-Side-Scripting-Language:
     *
     *  Request:
     *      /url/to/nph-eperl/url/to/script.phtml[?query-string]
     *  Environment:
     *      GATEWAY_INTERFACE=CGI/1.1
     *      SCRIPT_NAME=/url/to/nph-eperl
     *      SCRIPT_FILENAME=/path/to/nph-eperl
     *      PATH_INFO=/url/to/script.phtml
     *      PATH_TRANSLATED=/path/to/script.phtml
     *      a) QUERY_STRING=""
     *         optind=argc
     *      b) QUERY_STRING=query-string (containing "=" char)
     *         optind=argc
     *      c) QUERY_STRING=query-string (containing NO "=" char)
     *         optind=argc-1
     *         argv[optind]=query-string
     */
    if (   cpCGIgi[0] != NUL
        && cpCGIpt[0] != NUL
        && (   (   optind == argc
                && (   cpCGIqs[0] == NUL
                    || fCGIqsEqualChar      ) )
            || (   optind == argc-1
                && !fCGIqsEqualChar
                && stringEQ(argv[optind], cpCGIqs) )      ) ) {

        if (strncasecmp(cpCGIgi, "CGI/1", 5) != 0) {
            fprintf(stderr, "ePerl:Error: Unknown gateway interface: NOT CGI/1.x\n");
            CU(EX_IOERR);
        }

        /*  CGI/1.1 or NPH-CGI/1.1 script,
            source in PATH_TRANSLATED. */
        source = cpCGIpt;

        /* set the command line for ``ps'' output */
        snprintf(ca, sizeof(ca), "%s %s [%sCGI/SSSL]", argv[0], source, 0 ? "NPH-" : "");
        ca[sizeof(ca)-1] = NUL;
        argv[0] = strdup(ca);
    }
    /*
     *  Stand-Alone inside Webserver environment:
     *
     *  Request:
     *      /url/to/script.cgi[/path-info][?query-string]
     *      [script.cgi has shebang #!/path/to/eperl]
     *  Environment:
     *      GATEWAY_INTERFACE=CGI/1.1
     *      SCRIPT_NAME=/url/to/script.cgi
     *      SCRIPT_FILENAME=/path/to/script.cgi
     *      PATH_INFO=/path-info
     *      PATH_TRANSLATED=/path/to/docroot/path-info
     *      a) QUERY_STRING=""
     *         optind=argc-1
     *         argv[optind]=/path/to/script.cgi
     *      b) QUERY_STRING=query-string (containing "=" char)
     *         optind=argc-1
     *         argv[optind]=/path/to/script.cgi
     *      c) QUERY_STRING=query-string (containing NO "=" char)
     *         optind=argc-2
     *         argv[optind]=/path/to/script.cgi
     *         argv[optind+1]=query-string
     */
    else if (   cpCGIgi[0] != NUL
             && ( (   optind == argc-1
                   && (   cpCGIqs[0] == NUL
                       || fCGIqsEqualChar      ) ) ||
                  (   optind == argc-2
                   && !fCGIqsEqualChar
                   && stringEQ(argv[optind+1], cpCGIqs)) ) ) {

        if (strncasecmp(cpCGIgi, "CGI/1", 5) != 0) {
            fprintf(stderr, "ePerl:Error: Unknown gateway interface: NOT CGI/1.x\n");
            CU(EX_IOERR);
        }

        /*  CGI/1.1 or NPH-CGI/1.1 script,
            source in ARGV */
        source = argv[optind];

    }
    /*
     *  Stand-Alone outside Webserver environment:
     *
     *  Request:
     *      eperl script
     *  Environment:
     *      GATEWAY_INTERFACE=""
     *      SCRIPT_NAME=""
     *      SCRIPT_FILENAME=""
     *      PATH_INFO=""
     *      PATH_TRANSLATED=""
     *      QUERY_STRING=""
     *      optind=argc-1
     *      argv[optind]=script
     */
    else if (   cpCGIgi[0] == NUL
             && cpCGIpt[0] == NUL
             && cpCGIqs[0] == NUL
             && optind == argc-1  ) {

        /*  stand-alone filter, source as argument:
            either manually on the console or via shebang */
        source = argv[optind];
    }
    /*
     *   Any other calling environment is an error...
     */
    else {
        fprintf(stderr, "ePerl:Error: Missing required file to process\n");
        fprintf(stderr, "ePerl:Error: Use either a filename, `-' for STDIN or PATH_TRANSLATED.\n");
        fprintf(stderr, "Try `%s --help' for more information.\n", progname);
        exit(EX_USAGE);
    }

    /* CGI modes imply
       - Preprocessor usage
       - HTML entity conversions
       - adding of DOCUMENT_ROOT to include paths */
    /* check for valid source file */
    if (*source == NUL) {
        PrintError(0, "", NULL, NULL, "Filename is empty");
        CU(0 ? EX_IOERR : EX_OK);
    }

#define mode 0
    /* check for existing source file */
    if ((stat(source, &st)) != 0) {
        PrintError(mode, source, NULL, NULL, "File `%s' not exists", source);
        CU(mode == 0 ? EX_IOERR : EX_OK);
    }

    /* now set the additional env vars */
    if ((cpPath = getenv("PATH_INFO")) != NULL) {
        if ((cpHost = getenv("SERVER_NAME")) == NULL)
            cpHost = "localhost";
        cpPort = getenv("SERVER_PORT");
        if (stringEQ(cpPort, "80"))
            cpPort = NULL;
        snprintf(ca, sizeof(ca), "http://%s%s%s%s",
                cpHost, cpPort != NULL ? ":" : "", cpPort != NULL ? cpPort : "", cpPath);
        ca[sizeof(ca)-1] = NUL;
    }
    else {
    }

    stat(source, &st);
    cp = ctime(&(st.st_mtime));
    cp[strlen(cp)-1] = NUL;
    if ((pw = getpwuid(st.st_uid)) != NULL)
    {}
    /* optionally run the ePerl preprocessor */
    if (fPP) {
        /* switch to directory where script stays */
        if (! getcwd(cwd, MAXPATHLEN) ) {
            PrintError(mode, source, NULL, NULL, "getcwd failed with errno %ld", (long)errno);
            CU(mode == 0 ? EX_IOERR : EX_OK);
        }
        strncpy(sourcedir, source, sizeof(sourcedir));
        sourcedir[sizeof(sourcedir)-1] = NUL;
        for (cp = sourcedir+strlen(sourcedir); cp > sourcedir && *cp != '/'; cp--)
            ;
        *cp = NUL;
        if (chdir(sourcedir) != 0) {
            PrintError(mode, source, NULL, NULL, "chdir failed with errno %ld", (long)errno);
            CU(mode == 0 ? EX_IOERR : EX_OK);
        }
        /* switch to previous dir */
        if (chdir(cwd) != 0) {
            PrintError(0, source, NULL, NULL, "chdir failed with errno %ld", (long)errno);
            CU(mode == 0 ? EX_IOERR : EX_OK);
        }
    }

    /* convert bristled source to valid Perl code */
    /* write buffer to temporary script file */
    strncpy(perlscript, "ePerl.script", sizeof(perlscript));
    perlscript[sizeof(perlscript)-1] = NUL;
#ifndef DEBUG_ENABLED
    unlink(perlscript);
#endif
    if ((fp = fopen(perlscript, "w")) == NULL) {
        PrintError(mode, source, NULL, NULL, "Cannot open Perl script file `%s' for writing", perlscript);
        CU(mode == 0 ? EX_IOERR : EX_OK);
    }
    if (fwrite(cpScript, strlen(cpScript), 1, fp) != 1) {
        PrintError(mode, source, NULL, NULL, "Cannot write to Perl script file `%s'", perlscript);
        CU(mode == 0 ? EX_IOERR : EX_OK);
    }
    fclose(fp); fp = NULL;

#ifdef __CYGWIN__
#define USE_stderr
#endif
    if (1) {
        fp = stderr;
        fprintf(fp, "----internally created Perl script-----------------------------------\n");
        if (fwrite(cpScript, strlen(cpScript)-1, 1, fp) != 1)
        {
            PrintError(mode, source, NULL, NULL, "%s\n", "Cannot write");
            CU(mode == 0 ? EX_IOERR : EX_OK);
        }
        if (cpScript[strlen(cpScript)-1] == '\n')
            fprintf(fp, "%c", cpScript[strlen(cpScript)-1]);
        else
            fprintf(fp, "%c\n", cpScript[strlen(cpScript)-1]);
        fprintf(fp, "----internally created Perl script-----------------------------------\n");
#ifndef USE_stderr
        fclose(fp);
#endif
        fp = NULL;
    }

    /* temporary filename for Perl's STDOUT channel */
    strncpy(perlstdout, "/tmp/ePerl.stdout", sizeof(perlstdout));
    perlstdout[sizeof(perlstdout)-1] = NUL;
#ifndef DEBUG_ENABLED
    unlink(perlstdout);
#endif

    /* temporary filename for Perl's STDERR channel */
    strncpy(perlstderr, "ePerl.stderr", sizeof(perlstderr));
    perlstderr[sizeof(perlstderr)-1] = NUL;
#ifndef DEBUG_ENABLED
    unlink(perlstderr);
#endif

    /*  create command line...  */
    myargc = 0;
    /*  - program name and possible -T -w options */
    myargv[myargc++] = progname;
    if (fTaint)
        myargv[myargc++] = "-T";
    if (fWarn)
        myargv[myargc++] = "-w";
    /*  - previously remembered Perl 5 INC entries (option -I) */
    for (i = 0; RememberedINC[i] != NULL; i++) {
        myargv[myargc++] = "-I";
        myargv[myargc++] = RememberedINC[i];
    }
    /*  - and the script itself  */
    myargv[myargc++] = perlscript;

    rc = Perl5_Run(myargc, myargv, mode, fCheck, keepcwd, source, env, perlscript, perlstderr, perlstdout);
    /*  Return code:
     *     0: ok
     *    -1: fCheck && mode == 0 and
     *        no error detected by perl_parse()
     *    otherwise: error detected by perl_parse() or perl_run()
     *  Error message has already been delivered bu Perl5_Run.
     */
    if (rc != 0) {
        if (rc == -1)
            CU(EX_OK);
        CU(mode == 0 ? EX_FAIL : EX_OK);
    }

    CUS: /* the Clean Up Sequence */

    /* close all still open file handles */
    if (fp)
        fclose(fp);

    /* de-allocate the script buffer */
    if (cpBuf2)
        free(cpBuf2);

    /* remove temporary files */
#ifndef DEBUG_ENABLED
    if (*perlstderr != NUL)
        unlink(perlstderr);
    if (*perlstdout != NUL)
        unlink(perlstdout);
    if (*perlscript != NUL)
        unlink(perlscript);
#endif

    exit(rc);
}

/*EOF*/
