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
#define EX__BASE        64      /* base value for error messages */
#define EX_USAGE        64      /* command line usage error */
#define EX_DATAERR      65      /* data format error */
#define EX_NOINPUT      66      /* cannot open input */
#define EX_NOUSER       67      /* addressee unknown */
#define EX_NOHOST       68      /* host name unknown */
#define EX_UNAVAILABLE  69      /* service unavailable */
#define EX_SOFTWARE     70      /* internal software error */
#define EX_OSERR        71      /* system error (e.g., can't fork) */
#define EX_OSFILE       72      /* critical OS file missing */
#define EX_CANTCREAT    73      /* can't create (user) output file */
#define EX_IOERR        74      /* input/output error */
#define EX_TEMPFAIL     75      /* temp failure; user is invited to retry */
#define EX_PROTOCOL     76      /* remote error in protocol */
#define EX_NOPERM       77      /* permission denied */
#define EX_CONFIG       78      /* configuration error */
#define EX__MAX         78      /* maximum listed value */

/* OK and FAIL exits should ALWAYS exists */
#ifndef EX_OK
#define EX_OK   0
#endif
#ifndef EX_FAIL
#define EX_FAIL 1
#endif


/*
 * **
 * **  ASCII Control Codes
 * **
 * */
#define ASC_NUL '\x00'
#define ASC_SOH '\x01'
#define ASC_STX '\x02'
#define ASC_ETX '\x03'
#define ASC_EOT '\x04'
#define ASC_ENQ '\x05'
#define ASC_ACK '\x06'
#define ASC_BEL '\x07'
#define ASC_BS  '\x08'
#define ASC_HT  '\x09'
#define ASC_LF  '\x0a'
#define ASC_VT  '\x0b'
#define ASC_FF  '\x0c'
#define ASC_CR  '\x0d'
#define ASC_SO  '\x0e'
#define ASC_SI  '\x0f'
#define ASC_DLE '\x10'
#define ASC_DC1 '\x11'
#define ASC_DC2 '\x12'
#define ASC_DC3 '\x13'
#define ASC_DC4 '\x14'
#define ASC_NAK '\x15'
#define ASC_SYN '\x16'
#define ASC_ETB '\x17'
#define ASC_CAN '\x18'
#define ASC_EM  '\x19'
#define ASC_SUB '\x1a'
#define ASC_ESC '\x1b'
#define ASC_FS  '\x1c'
#define ASC_GS  '\x1d'
#define ASC_RS  '\x1e'
#define ASC_US  '\x1f'
#define ASC_SP  '\x20'
#define ASC_DEL '\x7f'
#define NUL ASC_NUL

#define ASC_QUOTE '\x22'
#define ASC_NL    ASC_LF
#define NL        ASC_NL


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

/*
**  Shortcuts for string comparisons
*/
#define stringEQ(s1,s2)    (s1 != NULL && s2 != NULL && strcmp(s1,s2) == 0)
#define stringNE(s1,s2)    (s1 != NULL && s2 != NULL && strcmp(s1,s2) != 0)

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

/* eperl_perl5.c */
/*  These prototypes can no longer be included in eperl_proto.h because
 *      pTHX argument has been introduced in Perl 5.6.0  */
extern void Perl5_XSInit(pTHX);
extern void Perl5_SetScalar(pTHX_ char *pname, char *vname, char *vvalue);
extern char *Perl5_RememberedScalars[1024];
extern void Perl5_RememberScalar(char *str);
extern void Perl5_SetRememberedScalars(pTHX);

#include "eperl_perl5_sm.h"

#ifdef HAVE_PERL_DYNALOADER

extern void boot_DynaLoader _((pTHX_ CV* cv));

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
        ca[1023] = 0;
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
    int rc;
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
        fprintf(stderr, "Cannot open STDOUT file `%s' for writing", perlstdout);
        CU(mode == 0 ? EX_IOERR : EX_OK);
    }

    /* open a file for Perl's STDERR channel
       and redirect stderr to the new channel */
    if ((er = fopen(perlstderr, "w")) == NULL) {
        fprintf(stderr, "Cannot open STDERR file `%s' for writing", perlstderr);
        CU(mode == 0 ? EX_IOERR : EX_OK);
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
        if (fCheck && mode == 0) {
            fclose(er); er = NULL;
            CU(EX_FAIL);
        }
        else {
            fclose(er); er = NULL;
            fprintf(stderr, "Perl parsing error (interpreter rc=%d) error=%s", rc, SvTRUE(ERRSV) ? SvPV_nolen(ERRSV) : "");
            CU(mode == 0 ? EX_FAIL : EX_OK);
        }
    }

    /* Stop when we are just doing a syntax check */
    if (fCheck && mode == 0) {
        fclose(er); er = NULL;
        fprintf(stderr, "%s syntax OK\n", source);
        CU(-1);
    }

    /* change to directory of script:
       this actually is not important to us, but really useful
       for the ePerl source file programmer!! */
    cwd[0] = NUL;
    if (!keepcwd) {
        /* if running as a Unix filter remember the cwd for outputfile */
        if (mode == 0)
        {
            if (! getcwd(cwd, MAXPATHLEN))
            {
                CU(-1);
            }
        }
        /* determine dir of source file and switch to it */
        strncpy(sourcedir, source, sizeof(sourcedir));
        sourcedir[sizeof(sourcedir)-1] = NUL;
        for (cp = sourcedir+strlen(sourcedir); cp > sourcedir && *cp != '/'; cp--)
            ;
        *cp = NUL;
        if (chdir(sourcedir) != 0) {
            CU(-1);
        }
    }

    /*  Set the previously remembered Perl 5 scalars (option -d) */
    Perl5_SetRememberedScalars(aTHX);

    /*  NOW IT IS TIME to evaluate/execute the script!!! */
    rc = perl_run(my_perl);

    /*  pre-close the handles, to be able to check
        its size and to be able to display the contents */
    fclose(out); out = NULL;
    fclose(er);  er  = NULL;

    /*  when the Perl interpreter failed or there
        is data on stderr, we print a error page */
    if (stat(perlstderr, &st) == 0)
        size = st.st_size;
    else
        size = 0;
    if (rc != 0 || size > 0) {
        fprintf(stderr, "Perl runtime error (interpreter rc=%d)", rc);
        CU(mode == 0 ? EX_FAIL : EX_OK);
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

extern int Perl5_Run(int myargc, char **myargv, int mode, int fCheck, int keepcwd, char *source, char **env, char *perlscript, char *perlstderr, char *perlstdout);
extern void Perl5_RememberScalar(char *str);


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
