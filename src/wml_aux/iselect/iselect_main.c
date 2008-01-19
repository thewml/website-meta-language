/*      _ ____       _           _   
**     (_) ___|  ___| | ___  ___| |_ 
**    / /\___ \ / _ \ |/ _ \/ __| __|
**   / /  ___) |  __/ |  __/ (__| |_ 
**  (_(  |____/ \___|_|\___|\___|\__|
**
**  iSelect -- Interactive Selection Tool
**
**  iSelect is a Curses-based tool for interactive line selection 
**  in an ASCII file via a full-screen terminal session.
**  
**  ======================================================================
**
**  Copyright (c) 1996-1999 Ralf S. Engelschall.
**
**  This program is free software; it may be redistributed and/or
**  modified only under the terms of the GNU General Public License, 
**  which may be found in the iSelect source distribution.  
**  Look at the file COPYING for details. 
**
**  This program is distributed in the hope that it will be useful, 
**  but WITHOUT ANY WARRANTY; without even the implied warranty of 
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
**  See the the GNU General Public License for more details.
**
**  ======================================================================
**
**  iselect_main.c -- main program
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "iselect_global.h"
#include "iselect_getopt.h"
#include "iselect_browse.h"
#include "iselect_keys.h"
#define _ISELECT_VERSION_C_AS_HEADER_
#include "iselect_version.c"
#undef  _ISELECT_VERSION_C_AS_HEADER_

void give_version(char *name);
void give_usage(char *name);

void give_version(char *name)
{
    fprintf(stderr, "%s\n", iselect_version.v_tex);
    fprintf(stdout, "\n");
    fprintf(stdout, "Copyright (c) 1996,1997,1998 Ralf S. Engelschall.\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "This program is distributed in the hope that it will be useful,\n");
    fprintf(stdout, "but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
    fprintf(stdout, "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
    fprintf(stdout, "GNU General Public License for more details.\n");
    fprintf(stdout, "\n");
}

void give_usage(char *name)
{
    fprintf(stderr, "Usage: %s [options] [line1 line2 ...]\n", name);
    fprintf(stderr, "\n");
    fprintf(stderr, " Input Options:\n");
    fprintf(stderr, "  -d, --delimiter=STR,STR selection tag delimiters\n");
    fprintf(stderr, "  -c, --strip-comments    strip sharp-comments in input buffer\n");
    fprintf(stderr, "  -f, --force-browse      browse even if input has 0 or only 1 line\n");
    fprintf(stderr, "  -a, --all-select        force all lines to be selectable\n");
    fprintf(stderr, "  -e, --exit-no-select    exit immediately if no lines are selectable\n");
    fprintf(stderr, "\n");
    fprintf(stderr, " Display Options:\n");
    fprintf(stderr, "  -p, --position=NUM      initial line position of cursor\n");
    fprintf(stderr, "  -k, --key=KEY[:OKEY]    enable an additional input key\n");
    fprintf(stderr, "  -m, --multi-line        allow multiple lines to be selected\n");
    fprintf(stderr, "  -n, --name=STR          program name shown flush-left on status bar\n");
    fprintf(stderr, "  -t, --title=STR         title string shown centered on status bar\n");
    fprintf(stderr, "\n");
    fprintf(stderr, " Output Options:\n");
    fprintf(stderr, "  -S, --strip-result      strip whitespaces in result string\n");
    fprintf(stderr, "  -P, --position-result   prefix result string with `N:' (N=line number)\n");
    fprintf(stderr, "  -K, --key-result        prefix result string with `K:' (K=select key)\n");
    fprintf(stderr, "  -Q, --quit-result=STR   result string on quit (default='')\n");
    fprintf(stderr, "\n");
    fprintf(stderr, " Giving Feedback:\n");
    fprintf(stderr, "  -V, --version           display version string\n");
    fprintf(stderr, "  -h, --help              display this page\n");
    fprintf(stderr, "\n");
}

struct option options[] = {
    { "strip-comments",  FALSE, NULL, 'c'  },
    { "force-browse",    FALSE, NULL, 'f'  },
    { "all-select",      FALSE, NULL, 'a'  },
    { "exit-no-select",  FALSE, NULL, 'e'  },
    { "position",        TRUE,  NULL, 'p'  },
    { "key",             TRUE,  NULL, 'k'  },
    { "multi-line",      FALSE, NULL, 'm'  },
    { "name",            TRUE,  NULL, 'n'  },
    { "title",           TRUE,  NULL, 't'  },
    { "strip-result",    FALSE, NULL, 'S'  },
    { "position-result", FALSE, NULL, 'P'  },
    { "key-result",      FALSE, NULL, 'K'  },
    { "quit-result",     TRUE,  NULL, 'Q'  },
    { "version",         FALSE, NULL, 'V'  },
    { "help",            FALSE, NULL, 'h'  },
    { NULL,              FALSE, NULL, '\0' },
};

char caBuf[MAXBUF];

int main(int argc, char **argv)
{
    int fpStdout;
    int fpStdin;
    char *cp;
    char c;
    int pos = -1;
    char *progname;
    int nBuf, p;
    char ca[1024];
    char *title = "";
    char *name = "iSelect";
    int stripco = FALSE;
    int stripws = FALSE;
    int resultline = FALSE;
    int keyresultline = FALSE;
    int browsealways = FALSE;
    int allselectable = FALSE;
    int multiselect = FALSE;
    int exitnoselect = FALSE;
    int i;
    char *keystr;
    char *abortstr = NULL;
	char *tagbegin = "<";
	char *tagend   = ">";

    /*
     *  argument handling
     */

    /*  canonicalize program name */
    if ((cp = strrchr(argv[0], '/')) != NULL)
        progname = cp+1;
    else
        progname = argv[0];
    argv[0] = progname;

    /*  parse the option arguments */
    opterr = 0;
    while ((c = getopt_long(argc, argv, "d:cfaep:k:mn:t:SPKQ:Vh", options, NULL)) != (char)(-1)) {
        if (optarg == NULL) 
            optarg = "(null)";
        switch (c) {
            case 'd':
				tagbegin = strdup(optarg);
				if ((cp = strchr(tagbegin, ',')) == NULL) {
                    fprintf(stderr, "iSelect: bad argument to option '%c'\n", optopt);
                    fprintf(stderr, "Try `%s --help' for more information.\n", progname);
                    exit(EX_USAGE);
				}
				*cp++ = NUL;
				tagend = cp;
                break;
            case 'c':
                stripco = TRUE;
                break;
            case 'f':
                browsealways = TRUE;
                break;
            case 'a':
                allselectable = TRUE;
                break;
            case 'e':
                exitnoselect = TRUE;
                break;
            case 'p':
                pos = atoi(optarg); 
                break;
            case 'k':
                configure_custom_key(optarg); 
                break;
            case 'm':
                multiselect = TRUE;
                break;
            case 'n':
                name = strdup(optarg); 
                break;
            case 't':
                title = strdup(optarg); 
                break;
            case 'S':
                stripws = TRUE;
                break;
            case 'P':
                resultline = TRUE;
                break;
            case 'K':
                keyresultline = TRUE;
                break;
            case 'Q':
                abortstr = strdup(optarg); 
                break;
            case 'V':
                give_version(progname);
                exit(EX_OK);
            case 'h':
                give_usage(progname);
                exit(EX_OK);
            case '?':
                fprintf(stderr, "iSelect: invalid option: '%c'\n", optopt);
                fprintf(stderr, "Try `%s --help' for more information.\n", progname);
                exit(EX_USAGE);
            case ':':
                fprintf(stderr, "iSelect: missing argument to option '%c'\n", optopt);
                fprintf(stderr, "Try `%s --help' for more information.\n", progname);
                exit(EX_USAGE);
        }
    }

    /*
     *  read input
     */

    if (optind < argc) {
        /* browsing text is given as arguments */
        nBuf = 0;
        for (; optind < argc; ++optind) {
            cp = (argv[optind] == NULL ? "" : argv[optind]);
            sprintf(caBuf+nBuf, "%s\n", cp);
            nBuf += strlen(cp)+1;
        }
        caBuf[nBuf++] = NUL;
    }
    else if (optind == argc && !feof(stdin)) {
        /* browsing text is given on stdin */
        nBuf = 0;
        while ((c = fgetc(stdin)) != (char)(EOF)) {
            caBuf[nBuf++] = c;
        }
        caBuf[nBuf++] = NUL;

        /* save stdin filehandle and reconnect it to tty */
        fpStdin = dup(0);
        close(0);
        open("/dev/tty", O_RDONLY);
    }
    else {
        give_usage(progname);
        exit(EX_USAGE);
    }

    /*
     *  preserve stdout filehandle for result string, i.e.
     *  use the terminal directly for output
     */
    fpStdout = dup(1);
    close(1);
    open("/dev/tty", O_RDWR);

    pos = (pos < 1 ? 1 : pos);

    p = iSelect(caBuf, pos-1, title, name,
                tagbegin, tagend, stripco, stripws, 
                browsealways, allselectable, multiselect, exitnoselect, &keystr);

    /*
     *  give back the result string to the user via 
     *  the stdout file handle
     */
    if (p != -1) {
        for (i = 0; i < nLines; i++) {
            if (spaLines[i]->fSelected) {
                if (resultline) {
                    sprintf(ca, "%d:", i+1);
                    write(fpStdout, ca, strlen(ca));
                }
                if (keyresultline) {
                    sprintf(ca, "%s:", keystr);
                    write(fpStdout, ca, strlen(ca));
                }
                write(fpStdout, spaLines[i]->cpResult, strlen(spaLines[i]->cpResult));
                sprintf(ca, "\n");
                write(fpStdout, ca, strlen(ca));
            }
        }
    }
    else {
        if (abortstr != NULL)
            write(fpStdout, abortstr, strlen(abortstr));
    }
    exit(0);
}


/*EOF*/
