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
#include "eperl_security.h"
#include "eperl_proto.h"

int mode = MODE_UNKNOWN;

char *allowed_file_ext[]   = LIST_OF_ALLOWED_FILE_EXT;
char *allowed_caller_uid[] = LIST_OF_ALLOWED_CALLER_UID;

/*
 *  Display an error message and a logfile content as a HTML page
 */
void PrintError(int mode, char *scripturl, char *scriptfile, char *logfile, char *str, ...)
{
    va_list ap;
    char ca[1024];
    char *cpBuf;
    char *cp;

    va_start(ap, str);
    vsnprintf(ca, sizeof(ca), str, ap);
    ca[sizeof(ca)-1] = NUL;

    if (mode == MODE_CGI || mode == MODE_NPHCGI) {
        printf("Content-Type: text/html\r\n\r\n");
        printf("<html>\n");
        printf("<head>\n");
        printf("<title>ePerl: ERROR: %s</title>\n", ca);
        printf("</head>\n");
        printf("<body bgcolor=\"#d0d0d0\">\n");
        printf("<blockquote>\n");
        cp = getenv("SCRIPT_NAME");
        if (cp == NULL)
            cp = "UNKNOWN_IMG_DIR";
        printf("<table cellspacing=0 cellpadding=0 border=0>\n");
        printf("<tr>\n");
        printf("</tr>\n");
        printf("<tr>\n");
        printf("</tr>\n");
        printf("</table>\n");
        printf("<p>\n");
        printf("<table bgcolor=\"#d0d0f0\" cellspacing=0 cellpadding=10 border=0>\n");
        printf("<tr><td bgcolor=\"#b0b0d0\">\n");
        printf("<font face=\"Arial, Helvetica\"><b>ERROR:</b></font>\n");
        printf("</td></tr>\n");
        printf("<tr><td>\n");
        printf("<h1><font color=\"#3333cc\">%s</font></h1>\n", ca);
        printf("</td></tr>\n");
        printf("</table>\n");
        if (logfile != NULL) {
        }
        printf("</blockquote>\n");
        printf("</body>\n");
        printf("</html>\n");
    }
    else {
        fprintf(stderr, "ePerl:Error: %s\n", ca);
        if (logfile != NULL) {
        }
    }
    fflush(stderr);
    fflush(stdout);

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
    fprintf(stderr, "  -m, --mode=STR            force runtime mode to FILTER, CGI or NPH-CGI\n");
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
    myexit(EX_FAIL);
}

void myinit(void)
{
    /* caught signals */
    signal(SIGINT,  mysighandler);
    signal(SIGTERM, mysighandler);
}

void myexit(int rc)
{
    /* cleanup */
#ifndef DEBUG_ENABLED
#endif

    /* restore signals */
    signal(SIGINT,  SIG_DFL);
    signal(SIGTERM, SIG_DFL);

#ifdef DEBUG_ENABLED
#ifdef HAVE_DMALLOC
    dmalloc_shutdown();
#endif
#endif

    /* die gracefully */
    exit(rc);
}

char *ePerl_ReadSourceFile(char *filename, char **cpBufC, int *nBufC)
{
    char *rc;
    FILE *fp = NULL;
    char *cpBuf = NULL;
    int nBuf;
    char tmpfile[256], *ptr_tmpfile;
    int usetmp = 0;
    int c;

    if (stringEQ(filename, "-")) {
        /* file is given on stdin */
        ptr_tmpfile = "ePerl.source";
        sprintf(tmpfile, "%s", ptr_tmpfile);
        if ((fp = fopen(tmpfile, "w")) == NULL) {
            ePerl_SetError("Cannot open temporary source file %s for writing", tmpfile);
            CU(NULL);
        }
        nBuf = 0;
        while ((c = fgetc(stdin)) != EOF) {
            fprintf(fp, "%c", c);
        }
        fclose(fp);
        fp = NULL;
        filename = tmpfile;
        usetmp = 1;
    }

    if ((fp = fopen(filename, "r")) == NULL) {
        ePerl_SetError("Cannot open source file %s for reading", filename);
        CU(NULL);
    }
    fseek(fp, 0, SEEK_END);
    nBuf = ftell(fp);
    if (nBuf == 0) {
        cpBuf = (char *)malloc(sizeof(char) * 1);
        *cpBuf = NUL;
    }
    else {
        if ((cpBuf = (char *)malloc(sizeof(char) * nBuf+1)) == NULL) {
            ePerl_SetError("Cannot allocate %d bytes of memory", nBuf);
            CU(NULL);
        }
        fseek(fp, 0, SEEK_SET);
        if (fread(cpBuf, nBuf, 1, fp) == 0) {
            ePerl_SetError("Cannot read from file %s", filename);
            CU(NULL);
        }
        cpBuf[nBuf] = '\0';
    }
    *cpBufC = cpBuf;
    *nBufC  = nBuf;
    RETURN_WVAL(cpBuf);

    CUS:
    if (cpBuf)
        free(cpBuf);
    if (fp)
        fclose(fp);
    if (usetmp)
        unlink(tmpfile);
    RETURN_EXRC;
}
/*
 *  main procedure
 */
int main(int argc, char **argv, char **env)
{
    DECL_EXRC;
    FILE *fp = NULL;
    char *cpBuf = NULL;
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
    char *cpOut0 = NULL;
    char *cpOut = NULL;
    struct passwd *pw;
    struct passwd *pw2;
    struct group *gr;
    int uid, gid;
    int keepcwd = FALSE;
    int c;
    char *cpScript = NULL;
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

        /*  determine whether pure CGI or NPH-CGI mode */
        if ((cp = getenv("SCRIPT_FILENAME")) != NULL) {
            strncpy(ca, cp, sizeof(ca));
            ca[sizeof(ca)-1] = NUL;
            if ((cp = strrchr(ca, '/')) != NULL)
                *cp++ = NUL;
            else
                cp = ca;
            if (strncasecmp(cp, "nph-", 4) == 0)
                mode = (mode == MODE_UNKNOWN ? MODE_NPHCGI : mode);
            else
                mode = (mode == MODE_UNKNOWN ? MODE_CGI : mode);
        }
        else {
            mode = (mode == MODE_UNKNOWN ? MODE_CGI : mode);
        }

        /* set the command line for ``ps'' output */
        snprintf(ca, sizeof(ca), "%s %s [%sCGI/SSSL]", argv[0], source, mode == MODE_NPHCGI ? "NPH-" : "");
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

        /*  determine whether pure CGI or NPH-CGI mode */
        if ((cp = getenv("SCRIPT_FILENAME")) != NULL) {
            strncpy(ca, cp, sizeof(ca));
            ca[sizeof(ca)-1] = NUL;
            if ((cp = strrchr(ca, '/')) != NULL)
                *cp++ = NUL;
            else
                cp = ca;
            if (strncasecmp(cp, "nph-", 4) == 0)
                mode = (mode == MODE_UNKNOWN ? MODE_NPHCGI : mode);
            else
                mode = (mode == MODE_UNKNOWN ? MODE_CGI : mode);
        }
        else {
            mode = (mode == MODE_UNKNOWN ? MODE_CGI : mode);
        }

        /* set the command line for ``ps'' output */
        snprintf(ca, sizeof(ca), "%s %s [%sCGI/stand-alone]", argv[0], source, mode == MODE_NPHCGI ? "NPH-" : "");
        ca[sizeof(ca)-1] = NUL;
        argv[0] = strdup(ca);
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
        mode   = (mode == MODE_UNKNOWN ? MODE_FILTER : mode);

    }
    /*
     *   Any other calling environment is an error...
     */
    else {
        fprintf(stderr, "ePerl:Error: Missing required file to process\n");
        fprintf(stderr, "ePerl:Error: Use either a filename, `-' for STDIN or PATH_TRANSLATED.\n");
        fprintf(stderr, "Try `%s --help' for more information.\n", progname);
        myexit(EX_USAGE);
    }

    /* set default delimiters */
    if (ePerl_begin_delimiter == NULL) {
        if (mode == MODE_FILTER)
            ePerl_begin_delimiter = BEGIN_DELIMITER_FILTER;
        else
            ePerl_begin_delimiter = BEGIN_DELIMITER_CGI;
    }
    if (ePerl_end_delimiter == NULL) {
        if (mode == MODE_FILTER)
            ePerl_end_delimiter = END_DELIMITER_FILTER;
        else
            ePerl_end_delimiter = END_DELIMITER_CGI;
    }
    if (fNoCase)
        ePerl_case_sensitive_delimiters = FALSE;
    else
        ePerl_case_sensitive_delimiters = TRUE;

    /* CGI modes imply
       - Preprocessor usage
       - HTML entity conversions
       - adding of DOCUMENT_ROOT to include paths */
    if (mode == MODE_CGI || mode == MODE_NPHCGI) {
        fPP = TRUE;
        ePerl_convert_entities = TRUE;
        if ((cp = getenv("DOCUMENT_ROOT")) != NULL)
            RememberINC(cp);
    }

    /* check for valid source file */
    if (*source == NUL) {
        PrintError(mode, "", NULL, NULL, "Filename is empty");
        CU(mode == MODE_FILTER ? EX_IOERR : EX_OK);
    }

    /* check for existing source file */
    if ((stat(source, &st)) != 0) {
        PrintError(mode, source, NULL, NULL, "File `%s' not exists", source);
        CU(mode == MODE_FILTER ? EX_IOERR : EX_OK);
    }

    /*
     * Security Checks for the CGI modes
     */
    if (mode == MODE_CGI || mode == MODE_NPHCGI) {

        /*
         *
         *  == General Security ==
         *
         */

        /* general security check: allowed file extension */
        if (CGI_NEEDS_ALLOWED_FILE_EXT) {
            allow = FALSE;
            n = strlen(source);
            for (i = 0; allowed_file_ext[i] != NULL; i++) {
                k = strlen(allowed_file_ext[i]);
                if (stringEQ(source+n-k, allowed_file_ext[i]))
                    allow = TRUE;
            }
            if (!allow) {
                PrintError(mode, source, NULL, NULL, "File `%s' is not allowed to be interpreted by ePerl (wrong extension!)", source);
                CU(EX_OK);
            }
        }

        /*
         *
         *  == Perl Security ==
         *
         */

        /* perhaps force Taint mode */
        if (CGI_MODES_FORCE_TAINTING)
            fTaint = TRUE;

        /* perhaps force Warnings */
        if (CGI_MODES_FORCE_WARNINGS)
            fWarn = TRUE;

        /*
         *
         * == UID/GID switching ==
         *
         */

        /* we can only do a switching if we have euid == 0 (root) */
        if (geteuid() == 0) {

            fOkSwitch = TRUE;

            /* get our real user id (= caller uid) */
            uid = getuid();

            /* security check: valid caller uid */
            pw = getpwuid(uid);
            if (SETUID_NEEDS_VALID_CALLER_UID && pw == NULL) {
                if (DO_FOR_FAILED_STEP == STOP_AND_ERROR) {
                    PrintError(mode, source, NULL, NULL, "Invalid UID %d of caller", uid);
                    CU(EX_OK);
                }
                else
                    fOkSwitch = FALSE;
            }
            else {
                /* security check: allowed caller uid */
                if (SETUID_NEEDS_ALLOWED_CALLER_UID) {
                    allow = FALSE;
                    for (i = 0; allowed_caller_uid[i] != NULL; i++) {
                        if (isdigit(allowed_caller_uid[i][0]))
                            pw2 = getpwuid(atoi(allowed_caller_uid[i]));
                        else
                            pw2 = getpwnam(allowed_caller_uid[i]);
                        if (stringEQ(pw->pw_name, pw2->pw_name)) {
                            allow = TRUE;
                            break;
                        }
                    }
                    if (!allow) {
                        if (DO_FOR_FAILED_STEP == STOP_AND_ERROR) {
                            PrintError(mode, source, NULL, NULL, "UID %d of caller not allowed", uid);
                            CU(EX_OK);
                        }
                        else
                            fOkSwitch = FALSE;
                    }
                }
            }

            /* security check: valid owner UID */
            pw = getpwuid(st.st_uid);
            if (SETUID_NEEDS_VALID_OWNER_UID && pw == NULL)
                if (DO_FOR_FAILED_STEP == STOP_AND_ERROR) {
                    PrintError(mode, source, NULL, NULL, "Invalid UID %d of owner", st.st_uid);
                    CU(EX_OK);
                }
                else
                    fOkSwitch = FALSE;
            else
                uid = pw->pw_uid;

            /* security check: valid owner GID */
            gr = getgrgid(st.st_gid);
            if (SETUID_NEEDS_VALID_OWNER_GID && gr == NULL)
                if (DO_FOR_FAILED_STEP == STOP_AND_ERROR) {
                    PrintError(mode, source, NULL, NULL, "Invalid GID %d of owner", st.st_gid);
                    CU(EX_OK);
                }
                else
                    fOkSwitch = FALSE;
            else
                gid = gr->gr_gid;

            /* security check: file has to stay below owner homedir */
            if (fOkSwitch && SETUID_NEEDS_BELOW_OWNER_HOME) {
                /* preserve current working directory */
                cwd2 = getcwd(NULL, 1024);

                /* determine physical homedir of owner */
                pw = getpwuid(st.st_uid);
                if (chdir(pw->pw_dir) == -1) {
                    if (DO_FOR_FAILED_STEP == STOP_AND_ERROR) {
                        PrintError(mode, source, NULL, NULL, "Invalid homedir ``%s'' of file owner", pw->pw_dir);
                        CU(EX_OK);
                    }
                    else
                        fOkSwitch = FALSE;
                }
                else {
                    dir_home = getcwd(NULL, 1024);

                    /* determine physical dir of file */
                    strncpy(dir_tmp, source, sizeof(dir_tmp));
                    dir_tmp[sizeof(dir_tmp)-1] = NUL;
                    if ((cp = strrchr(dir_tmp, '/')) == NULL) {
                        if (DO_FOR_FAILED_STEP == STOP_AND_ERROR) {
                            PrintError(mode, source, NULL, NULL, "Invalid script ``%s'': no absolute path", source);
                            CU(EX_OK);
                        }
                        else
                            fOkSwitch = FALSE;
                    }
                    else {
                        *cp = NUL;
                        if (chdir(dir_tmp) == -1) {
                            if (DO_FOR_FAILED_STEP == STOP_AND_ERROR) {
                                PrintError(mode, source, NULL, NULL, "Invalid script ``%s'': cannot chdir to its location", source);
                                CU(EX_OK);
                            }
                            else
                                fOkSwitch = FALSE;
                        }
                        else {
                            dir_script = getcwd(NULL, 1024);

                            /* dir_home has to be a prefix of dir_script */
                            if (strncmp(dir_script, dir_home, strlen(dir_home)) < 0) {
                                if (DO_FOR_FAILED_STEP == STOP_AND_ERROR) {
                                    PrintError(mode, source, NULL, NULL, "Invalid script ``%s'': does not stay below homedir of owner", source);
                                    CU(EX_OK);
                                }
                                else
                                    fOkSwitch = FALSE;
                            }

                            free(dir_script);
                        }
                    }
                    free(dir_home);
                }

                /* restore original cwd */
                if (chdir(cwd2) != 0)
                {
                    PrintError(mode, source, NULL, NULL, "chdir failed with errno: %li\n", (long)errno);
                }

                free(cwd2);
            }

            if (fOkSwitch && uid != 0 && gid != 0) {
                /* switch to new uid/gid */
                if (((setgid(gid)) != 0) || (initgroups(pw->pw_name,gid) != 0)) {
                    PrintError(mode, source, NULL, NULL, "Unable to set GID %d: setgid/initgroups failed", gid);
                    CU(mode == MODE_FILTER ? EX_IOERR : EX_OK);
                }
                if ((setuid(uid)) != 0) {
                    PrintError(mode, source, NULL, NULL, "Unable to set UID %d: setuid failed", uid);
                    CU(mode == MODE_FILTER ? EX_IOERR : EX_OK);
                }
            }
        }
    }

    /* read source file into internal buffer */
    if ((cpBuf = ePerl_ReadSourceFile(source, &cpBuf, &nBuf)) == NULL) {
        PrintError(mode, source, NULL, NULL, "Cannot open source file `%s' for reading\n%s", source, ePerl_GetError);
        CU(mode == MODE_FILTER ? EX_IOERR : EX_OK);
    }

    /* strip shebang prefix */
    if (strncmp(cpBuf, "#!", 2) == 0) {
        for (cpScript = cpBuf;
             (*cpScript != ' ' && *cpScript != '\t' && *cpScript != '\n') && (cpScript-cpBuf < nBuf);
             cpScript++)
            ;
        for (cpScript = cpBuf;
             *cpScript != '\n' && (cpScript-cpBuf < nBuf);
             cpScript++)
            ;
        cpScript++;
    }
    else
        cpScript = cpBuf;

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
            CU(mode == MODE_FILTER ? EX_IOERR : EX_OK);
        }
        strncpy(sourcedir, source, sizeof(sourcedir));
        sourcedir[sizeof(sourcedir)-1] = NUL;
        for (cp = sourcedir+strlen(sourcedir); cp > sourcedir && *cp != '/'; cp--)
            ;
        *cp = NUL;
        if (chdir(sourcedir) != 0) {
            PrintError(mode, source, NULL, NULL, "chdir failed with errno %ld", (long)errno);
            CU(mode == MODE_FILTER ? EX_IOERR : EX_OK);
        }
        /* switch to previous dir */
        if (chdir(cwd) != 0) {
            PrintError(mode, source, NULL, NULL, "chdir failed with errno %ld", (long)errno);
            CU(mode == MODE_FILTER ? EX_IOERR : EX_OK);
        }
    }

    /* convert bristled source to valid Perl code */
    if ((cpBuf2 = ePerl_Bristled2Plain(cpScript)) == NULL) {
        PrintError(mode, source, NULL, NULL, "Cannot convert bristled code file `%s' to pure HTML", source);
        CU(mode == MODE_FILTER ? EX_IOERR : EX_OK);
    }
    cpScript = cpBuf2;

    /* write buffer to temporary script file */
    strncpy(perlscript, "ePerl.script", sizeof(perlscript));
    perlscript[sizeof(perlscript)-1] = NUL;
#ifndef DEBUG_ENABLED
    unlink(perlscript);
#endif
    if ((fp = fopen(perlscript, "w")) == NULL) {
        PrintError(mode, source, NULL, NULL, "Cannot open Perl script file `%s' for writing", perlscript);
        CU(mode == MODE_FILTER ? EX_IOERR : EX_OK);
    }
    if (fwrite(cpScript, strlen(cpScript), 1, fp) != 1) {
        PrintError(mode, source, NULL, NULL, "Cannot write to Perl script file `%s'", perlscript);
        CU(mode == MODE_FILTER ? EX_IOERR : EX_OK);
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
            CU(mode == MODE_FILTER ? EX_IOERR : EX_OK);
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
     *    -1: fCheck && mode == MODE_FILTER and
     *        no error detected by perl_parse()
     *    otherwise: error detected by perl_parse() or perl_run()
     *  Error message has already been delivered bu Perl5_Run.
     */
    if (rc != 0) {
        if (rc == -1)
            CU(EX_OK);
        CU(mode == MODE_FILTER ? EX_FAIL : EX_OK);
    }

    /*  else all processing was fine, so
        we read in the stdout contents */
    if ((cpOut = ePerl_ReadSourceFile(perlstdout, &cpOut, &nOut)) == NULL) {
        PrintError(mode, source, NULL, NULL, "Cannot open STDOUT file `%s' for reading", perlstdout);
        CU(mode == MODE_FILTER ? EX_FAIL : EX_OK);
    }
    cpOut0 = cpOut; /* cpOut will move by HTTP_PrintResponseHeaders() later */
    stat(perlstdout, &st);

    /*  if we are running as a NPH-CGI/1.1 script
        we had to provide the HTTP reponse headers ourself */
    if (mode == MODE_NPHCGI) {
    }
    else if (mode == MODE_CGI) {
    }
    else if (mode == MODE_FILTER) {
    }

    /* now when the request was not a HEAD request we create the output */
    cp = getenv("REQUEST_METHOD");
    if (! ((mode == MODE_CGI || mode == MODE_NPHCGI) &&
           cp != NULL && stringEQ(cp, "HEAD"))) {
        if (outputfile != NULL && stringNE(outputfile, "-")) {
            /* if we remembered current working dir, restore it now */
            if (mode == MODE_FILTER && cwd[0] != NUL)
            {
                if (chdir(cwd) != 0)
                {
                    PrintError(mode, source, NULL, NULL, "%s\n", "Cannot chdir");
                    CU(mode == MODE_FILTER ? EX_FAIL : EX_OK);
                }
            }
            /* open outputfile and write out the data */
            if ((fp = fopen(outputfile, "w")) == NULL) {
                PrintError(mode, source, NULL, NULL, "Cannot open output file `%s' for writing", outputfile);
                CU(mode == MODE_FILTER ? EX_IOERR : EX_OK);
            }
            if (fwrite(cpOut, nOut, 1, fp) != 1) {
                PrintError(mode, source, NULL, NULL, "Cannot write to Perl script file `%s'", perlscript);
                CU(mode == MODE_FILTER ? EX_IOERR : EX_OK);
            }
            fclose(fp); fp = NULL;
        }
        else {
            /* data just goes to stdout */
            fflush(stdout);
        }
    }

    CUS: /* the Clean Up Sequence */

    /* close all still open file handles */
    if (fp)
        fclose(fp);

    /* de-allocate the script buffer */
    if (cpBuf)
        free(cpBuf);
    if (cpBuf2)
        free(cpBuf2);
    if (cpOut0)
        free(cpOut0);

    /* remove temporary files */
#ifndef DEBUG_ENABLED
    if (*perlstderr != NUL)
        unlink(perlstderr);
    if (*perlstdout != NUL)
        unlink(perlstdout);
    if (*perlscript != NUL)
        unlink(perlscript);
#endif

    myexit(EXRC);
    return EXRC; /* make -Wall happy ;-) */
}

/*EOF*/
