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
**  iselect_browse.c -- Curses-based file browser
*/

#include "config_ac.h"
#include "config_sc.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#ifdef USE_NCURSES
#include <ncurses.h>
#endif
#ifdef USE_SLCURSES
#include <slcurses.h>
#endif
#ifdef USE_CURSES
#include <curses.h>
#endif

#include "iselect_global.h"
#include "iselect_browse.h"
#include "iselect_keys.h"

extern char *iSelect_Help[];
extern char *iSelect_README[];

void strip(char *string);
void diehard(int signum);
void diesoft(void);
void setup(void);
char *iSelect_InputField(int wYSize, int wXSize, int wYPos, int wXPos, int bAllowEmpty);
void iSelect_Draw(WINDOW *wField,
                  int wYSize, int wXSize, int wYPos, int wXPos,
                  int nAbsFirstLine, int nAbsLastLine,
                  int nRelMarked,
                  int nRelFirstDraw, int nRelLastDraw,
                  int nbLines,
                  WINDOW *sField, char *title, char *name,
                  WINDOW *mField, char *msg, char *tagbegin, char *tagend);
int iSelect_Browser(int wYSize, int wXSize, int wYPos, int wXPos, int selectpos, int multiselect,
                    int sYSize, int sXSize, int sYPos, int sXPos, char *title, char *name,
                    int mYSize, int mXSize, int mYPos, int mXPos,
                    char **keystr, char *tagbegin, char *tagend);
int iSelect(char *caBuf, int pos, char *title, char *name,
            char *tagbegin, char *tagend, int stripco, int stripws, 
            int browsealways, int allselectable,
            int multiselect, int exitnoselect,
            char **keystr); 

/*
 *  GLOBALS
 */
struct Line *spaLines[MAXLINES];  /* filled by iSelect() */
int          nLines;


/*   
 *  Strip leading and trailing blanks 
 *  from a string buffer
 */
void strip(char *string)
{
    char *cps;
    char *cp;

    if (string != NULL) {
        /* strip leading blanks */
        cps = string;
        cp  = string;
        for (; (*cp == ' ' || *cp == '\t') && *cp != NUL; )
            cp++;
        if (cp > cps) {
            for (; *cp != NUL; )
                *cps++ = *cp++;
            *cps = NUL;
        }
        /* strip trailing blanks */
        cp = string+strlen(string);
        for (; cp > string && (*(cp-1) == ' ' || *(cp-1) == '\t'); ) 
            *(--cp) = NUL;
    }
    return;
}

/*
 *  Die gracefully...
 */
void diehard(int signum)
{
    signal(SIGINT,  SIG_IGN);      /* ignore all further interrupts */
    signal(SIGTERM, SIG_IGN);      /* ignore all further interrupts */
#ifndef USE_SLCURSES
    mvcur(0, COLS-1, LINES-1, 0);  /* move curser to lower left corner */
#endif
    endwin();                      /* make terminal the way it was */
    exit(0);
}

void diesoft(void)
{
#ifndef USE_SLCURSES
    mvcur(0, COLS-1, LINES-1, 0);  /* move curser to lower left corner */
#endif
    endwin();                      /* make terminal the way it was */
}

/*  
 *  Startup with trapping of interrupt signal
 */
void setup(void)
{
    signal(SIGINT,  diehard); /* die gracefully on interrupt signal */
    signal(SIGTERM, diehard); /* die gracefully on terminate signal */
    initscr();                          /* initialize Curses package */
    return;
}

/*
 *  Function to query an input string
 */
char *iSelect_InputField(int wYSize, int wXSize, int wYPos, int wXPos, int bAllowEmpty)
{
    WINDOW *wField;
    int c;
    int nBufEnd;
    int nBufCurPos;
    int nScrCurPos;
    int bEOI;
    char caBuf[1024];
    char *cpBuf;
    int i;

    cpBuf = caBuf;

    wField = newwin(wYSize, wXSize, wYPos, wXPos);
    keypad(wField, TRUE);
    scrollok(wField, FALSE);

    nScrCurPos = 0;
    nBufCurPos = 0;
    nBufEnd = 0;
    bEOI = FALSE;

    wmove(wField, 0, nScrCurPos);
    while (bEOI == FALSE) {
        c = wgetch(wField);
        if (c >= KEY_MIN && c <= KEY_MAX) {
            /* NOP */
        }
        else {
            c = c & 0xff; /* strip down to 8bit */
            if (c < 32 || c > 126) { 
                /*
                 *  a control code
                 */
                if (c == '\n') {      /* RETURN */
                    if (nBufEnd == 0 && !bAllowEmpty) 
                        continue;
                    cpBuf[nBufEnd] = '\0';
                    bEOI = TRUE;
                }
                else if (c == 0x1b) { /* ESCAPE */
                    strcpy(caBuf, "ESC");
                    bEOI = TRUE;
                }
                else if (c == 0x04) { /* DELETE */
                    waddch(wField, '?');
                    wrefresh(wField);
                }
                else if (c == 0x7f) { /* BACKSPACE */
                    if (nBufCurPos != 0) {
                        for (i = nBufCurPos; i < nBufEnd; i++) {
                            cpBuf[i-1] = cpBuf[i];
                        }
                        cpBuf[i] = '\0';
                        nBufEnd--;
                        nBufCurPos--;

                        if (nScrCurPos == (wXSize-1) && nBufEnd >= (wXSize-1)) {
                            wmove(wField, 0, 0);
                            for (i = nBufEnd-(wXSize-1); i < nBufEnd; i++) {
                                waddch(wField, cpBuf[i]);
                            }
                        }
                        else {
                            nScrCurPos--;
                            wmove(wField, 0, nScrCurPos);
                            waddch(wField, ' ');
                            wmove(wField, 0, nScrCurPos);
                        }
                        wrefresh(wField);
                    }
                }
            }
            if (c >= 32 && c <= 126) { 
                /*
                 *  a printable character
                 */
                if (nScrCurPos == (wXSize-1)) {
                    wmove(wField, 0, 0);
                    for (i = nBufEnd-(wXSize-1)+1; i < nBufEnd; i++) {
                        waddch(wField, cpBuf[i]);
                    }
                    nScrCurPos--; /* neutralize following increment */
                }
                cpBuf[nBufEnd++] = c;
                nBufCurPos++;
                nScrCurPos++;
                waddch(wField, c);
                wrefresh(wField);
            }
        }
    }

    fflush(stdin);
    delwin(wField);

    return strdup(caBuf);
}

/*
 *  Function to draw a complete screen
 */
void iSelect_Draw(WINDOW *wField,
                  int wYSize, int wXSize, int wYPos, int wXPos,
                  int nAbsFirstLine, int nAbsLastLine,
                  int nRelMarked,
                  int nRelFirstDraw, int nRelLastDraw,
                  int nbLines,
                  WINDOW *sField, char *title, char *name,
                  WINDOW *mField, char *msg, char *tagbegin, char *tagend)
{
    int i, j, k, mode;
    int percent;
    char ca[80];
    char boldbegin[1024];
    int nboldbegin;
    char boldend[1024];
    int nboldend;

    /*
     *  create actual bold tags
     */
    sprintf(boldbegin, "%sb%s", tagbegin, tagend);
    nboldbegin = strlen(boldbegin);
    sprintf(boldend, "%s/b%s", tagbegin, tagend);
    nboldend = strlen(boldend);

    /*
     *  draw browser window 
     */
    for (i = nRelFirstDraw; i <= nRelLastDraw; i++) {
        wmove(wField, i, 0);
        wclrtoeol(wField);
        mode = A_NORMAL;
        if (spaLines[nAbsFirstLine+nRelFirstDraw+i]->fSelectable)
            mode = mode | A_BOLD;
        if (i == nRelMarked)
#ifdef USE_SLCURSES
            mode = A_REVERSE;
#else
            mode = mode | A_REVERSE;
#endif
        wattrset(wField, mode);
        if (spaLines[nAbsFirstLine+nRelFirstDraw+i]->fSelected)
            waddstr(wField, "*");
        else
            waddstr(wField, " "); /* cursor pos == first blank */
        for (j = k = 0; spaLines[nAbsFirstLine+nRelFirstDraw+i]->cpLine[j] != NUL && j <= (wXSize-1)-2;) {
            if (strniEQ(&spaLines[nAbsFirstLine+nRelFirstDraw+i]->cpLine[j], boldbegin, nboldbegin)) {
                mode = mode | A_BOLD;
                wattrset(wField, mode);
                j += nboldbegin;
                continue;
            }
            if (strniEQ(&spaLines[nAbsFirstLine+nRelFirstDraw+i]->cpLine[j], boldend, nboldend)) {
                mode = mode & ~A_BOLD;
                wattrset(wField, mode);
                j += nboldend;
                continue;
            }
            waddch(wField, spaLines[nAbsFirstLine+nRelFirstDraw+i]->cpLine[j]);
            j++;
            k++;
        }
        for ( ; k <= (wXSize-1)-2; k++)
            waddch(wField, ' ');
        wattrset(wField, A_NORMAL);
    }
    wmove(wField, nRelMarked, (wXSize-1)-1);

    /* 
     *  draw status bar 
     */
    werase(sField);
    wattrset(sField, A_REVERSE);
    wmove(sField, 0, 0);
    for (i = 0; i < COLS; i++) {
        waddch(sField, ' ');
    }

    wmove(sField, 0, 1);
    waddstr(sField, name);

    wmove(sField, 0, COLS-10);
    percent = (int)(((nAbsFirstLine+nRelMarked)*100)/nbLines);
    if (1+nAbsFirstLine+nRelMarked == nbLines)
        percent = 100;
    sprintf(ca, "%4d,%3d%%", 1+nAbsFirstLine+nRelMarked, percent);
    waddstr(sField, ca);

    i = (COLS-1)/2-(strlen(title)/2);
    wmove(sField, 0, i);
    waddstr(sField, title);

    wattrset(sField, A_NORMAL);
    wrefresh(sField);

    /* 
     *  draw message field
     */
    werase(mField);
    wmove(mField, 0, 0);
    waddstr(mField, msg);
    wrefresh(mField);

    return;
}

/*
 *  Function to do a complete selection screen
 */
int iSelect_Browser(int wYSize, int wXSize, int wYPos, int wXPos, int selectpos, int multiselect,
                    int sYSize, int sXSize, int sYPos, int sXPos, char *title, char *name,
                    int mYSize, int mXSize, int mYPos, int mXPos,
                    char **keystr, char *tagbegin, char *tagend)
{
    WINDOW *wField;
    WINDOW *sField;
    WINDOW *mField;
    WINDOW *hField;
    int i;
    int nFirstLine, nLastLine;        /* first & last line in buffer */
    int nAbsFirstLine, nAbsLastLine;  /* first & last line of output buffer */
    int nRelMarked;                   /* relative line inside output buffer of marked line */
    int nRelFirstDraw, nRelLastDraw;  /* relative first & last line inside output buffer */
    int c;
    int bEOI;
    int bQuit = FALSE;
    int y;
    int x;
    char msg[1024];
    char ca[1024];
    char ca3[1024];
    char *cp;
    char *cp2;
    char *cp3;
    char **cpp;
    int ok;
    int bAllowEmpty;

    /*
     *  Browser field
     */
    wField = newwin(wYSize, wXSize, wYPos, wXPos);
    werase(wField);
    crmode();
    noecho();
    keypad(wField, TRUE);
    scrollok(wField, FALSE);

    /* 
     *  Status field
     */
    sField = newwin(sYSize, sXSize, sYPos, sXPos);
    werase(sField);
    strcpy(msg, "");

    /* 
     *  Message field
     */
    mField = newwin(mYSize, mXSize, mYPos, mXPos);
    werase(mField);

    /* dimension of file */
    nFirstLine = 0;
    nLastLine  = nLines-1;

    /* determine curses select position */
    if (selectpos < -1)
        selectpos = -1;
    if (selectpos > nLastLine)
        selectpos = nLastLine;
    if (selectpos == -1) {
        selectpos = 0;
        /* search for first selectable line */
        for (i = nFirstLine; i < nLastLine; i++) {
            if (spaLines[i]->fSelectable) {
                selectpos = i;
                break;
            }
        }
    }

    /* calculate browser view borders */
    if (nLastLine < (wYSize-1)) {
        /* buffer has fewer lines then our browser window */

        nAbsFirstLine = nFirstLine;
        nAbsLastLine  = nLastLine;
        nRelFirstDraw = 0;
        nRelLastDraw  = nLastLine-nFirstLine;
        nRelMarked    = selectpos;
    }
    else {
        /* browser window is smaller then file */
        
        /* find top view position, so adjust the 
           cursor into the middle of the browser window */
        y = selectpos - (int)((wYSize-1)/2);
        if (y <= 0)
            y = 0;
        if (y+(wYSize-1) > nLastLine)
            y = nLastLine-(wYSize-1);

        nAbsFirstLine = y;
        nAbsLastLine  = y+(wYSize-1);
        nRelFirstDraw = 0;
        nRelLastDraw  = (wYSize-1);
        nRelMarked    = selectpos-y;
    }


    ok = FALSE;
    for (i = nFirstLine; i < nLastLine; i++) {
        if (spaLines[i]->fSelectable) {
            ok = TRUE;
            break;
        }
    }
    if (!ok)
        strcpy(msg, "WARNING! No lines selectable.");


    bEOI = FALSE;
    while (bEOI == FALSE) {
         iSelect_Draw(wField,
                      wYSize, wXSize, wYPos, wXPos,
                      nAbsFirstLine, nAbsLastLine,
                      nRelMarked,
                      nRelFirstDraw, nRelLastDraw,
                      nLines,
                      sField, title, name,
                      mField, msg,
                      tagbegin, tagend);
        wrefresh(wField);
        strcpy(msg, "");
        c = wgetch(wField);
        *keystr = key2asc(c);
        c = do_custom_key(c);
        if (c == KEY_LEFT)
            c = 'q';
        if (c == KEY_RIGHT)
            c = '\n';
        if (c >= KEY_MIN && c <= KEY_MAX) {
            /*
             *  a curses special function key
             */
            if (c == KEY_DOWN) { 
                if (nAbsFirstLine+nRelMarked < nAbsLastLine) {
                    nRelMarked++;
                    /* nRelFirstDraw=nRelMarked-1; !!OPTIMIZE!! */
                    /* nRelLastDraw=nRelMarked;    !!OPTIMIZE!! */
                }
                else {
                    if (nAbsLastLine < nLastLine) {
                        wscrl(wField, 1);
                        nAbsFirstLine++;
                        nAbsLastLine++;
                        /* nRelFirstDraw=(wYSize-1); !!OPTIMIZE!! */
                        /* nRelLastDraw=(wYSize-1);  !!OPTIMIZE!!*/
                    }
                    else {
                        strcpy(msg, "Already at End.");
                    }
                }
            }   
            else if (c == KEY_UP) { 
                if (nRelMarked > 0) {
                    nRelMarked--;
                    /* nRelLastDraw=nRelMarked;    !!OPTIMIZE!! */
                    /* nRelFirstDraw=nRelMarked+1; !!OPTIMIZE!! */
                }
                else {
                    if (nAbsFirstLine > nFirstLine) {
                        wscrl(wField, -1);
                        nAbsFirstLine--;
                        nAbsLastLine--;
                        /* nRelFirstDraw=0 !!OPTIMIZE!! */
                        /* nRelLastDraw=0; !!OPTIMIZE!! */
                    }
                    else {
                        strcpy(msg, "Already at Begin.");
                    }
                }
            }   
            else if (c == KEY_NPAGE) { 
                if (nAbsFirstLine+nRelMarked == nLastLine) {
                    strcpy(msg, "Already at End.");
                }
                else {
                    for (i = 0; i < (wYSize-1); i++) {
                        if (nAbsFirstLine+nRelMarked < nAbsLastLine)
                            nRelMarked++;
                        else {
                            if (nAbsLastLine < nLastLine) {
                                wscrl(wField, 1);
                                nAbsFirstLine++;
                                nAbsLastLine++;
                            }
                        }
                    }
                }
            }
            else if (c == KEY_PPAGE) { 
                if (nAbsFirstLine+nRelMarked == nFirstLine) {
                    strcpy(msg, "Already at Begin.");
                }
                else {
                    for (i = 0; i < (wYSize-1); i++) {
                        if (nRelMarked > 0)
                            nRelMarked--;
                        else {
                            if (nAbsFirstLine > nFirstLine) {
                                wscrl(wField, -1);
                                nAbsFirstLine--;
                                nAbsLastLine--;
                            }
                        }
                    }
                }
            }
            else {
                strcpy(msg, "Invalid special key. Press 'h' for Help Page!");
            }
        }
        else {
            c = c & 0xff; /* strip down to 8bit */
            if (c < 32 || c > 126) { 
                /*
                 *  a control code
                 */
                if (c == '\n' || c == '\r') {      /* RETURN */
                    if (spaLines[nAbsFirstLine+nRelMarked]->fSelectable) {
                        spaLines[nAbsFirstLine+nRelMarked]->fSelected = TRUE;
                        bEOI = TRUE;
                    }
                    else {
                        if (multiselect) {
                            for (i = 0; i < nLines; i++) {
                                if (spaLines[i]->fSelected) {
                                    bEOI = TRUE;
                                    break;
                                }
                            }
                            if (!bEOI)
                                strcpy(msg, "Line not selectable and still no others selected.");
                        }
                        else {
                            strcpy(msg, "Line not selectable.");
                        }
                    }

                    /* additionally ask for query strings */
                    if (bEOI == TRUE) {
                        cp = spaLines[nAbsFirstLine+nRelMarked]->cpResult;
                        cp2 = ca;
                        while (bEOI == TRUE && *cp != NUL) {
                            if (strnEQ(cp, "%[", 2)) {
                                cp += 2;
                                for (cp3 = cp; !strniEQ(cp3, "]s", 2); cp3++)
                                    ;
                                strncpy(ca3, cp, cp3-cp);
                                ca3[cp3-cp] = NUL;
                                cp = cp3+1;
                                if (*cp == 's')
                                    bAllowEmpty = TRUE;
                                else
                                    bAllowEmpty = FALSE;
                                cp++;
    
                                sprintf(msg, "%s: ", ca3);
                                iSelect_Draw(wField, wYSize, wXSize, wYPos, wXPos, nAbsFirstLine, nAbsLastLine, -1, nRelFirstDraw, nRelLastDraw, nLines, sField, title, name, mField, msg, tagbegin, tagend);
                                wrefresh(wField);
                                cp3 = iSelect_InputField(mYSize, mXSize-strlen(msg), mYPos, mXPos+strlen(msg), bAllowEmpty);
                                if (strEQ(cp3, "ESC")) {
                                    bEOI = FALSE;
                                    spaLines[nAbsFirstLine+nRelMarked]->fSelected = FALSE;
                                    strcpy(msg, "Selection cancelled.");
                                    continue;
                                }
                                strcpy(msg, "");
                                strcpy(cp2, cp3);
                                cp2 += strlen(cp3);
                            }
                            else {
                                *cp2++ = *cp++;
                            }
                        }
                        if (bEOI == TRUE) {
                            *cp2 = NUL;
                            if (strNE(spaLines[nAbsFirstLine+nRelMarked]->cpResult, ca))
                                spaLines[nAbsFirstLine+nRelMarked]->cpResult = strdup(ca);
                        }
                    }
                }
            }
            if (c >= 32 && c <= 126) { 
                /*
                 *  a printable character
                 */
                 if (c == ' ') {
                     if (multiselect) {
                         if (spaLines[nAbsFirstLine+nRelMarked]->fSelectable) {
                             if (spaLines[nAbsFirstLine+nRelMarked]->fSelected == FALSE)
                                 spaLines[nAbsFirstLine+nRelMarked]->fSelected = TRUE;
                             else
                                 spaLines[nAbsFirstLine+nRelMarked]->fSelected = FALSE;
                         }
                         else {
                             strcpy(msg, "Line not selectable.");
                         }
                     }
                     else {
                            strcpy(msg, "No multi-line selection allowed.");
                     }
                 }
                 else if (c == 'q') {
                     bEOI = TRUE;
                     bQuit = TRUE;
                 }
                 else if (c == 'g') { 
                     if (nAbsFirstLine+nRelMarked == nFirstLine) {
                         strcpy(msg, "Already at Begin.");
                     }
                     else {
                         if (nLastLine < (wYSize-1)) {
                             nAbsFirstLine = nFirstLine;
                             nAbsLastLine  = nLastLine;
                             nRelFirstDraw = 0;
                             nRelLastDraw  = nLastLine-nFirstLine;
                             nRelMarked    = 0;
                         }
                         else {
                             nAbsFirstLine = nFirstLine;
                             nAbsLastLine  = nFirstLine+(wYSize-1);
                             nRelFirstDraw = 0;
                             nRelLastDraw  = (wYSize-1);
                             nRelMarked    = 0;
                         }
                     }
                 }
                 else if (c == 'G') { 
                     if (nAbsFirstLine+nRelMarked == nLastLine) {
                         strcpy(msg, "Already at End.");
                     }
                     else {
                         if (nLastLine < (wYSize-1)) {
                             nAbsFirstLine = nFirstLine;
                             nAbsLastLine  = nLastLine;
                             nRelFirstDraw = 0;
                             nRelLastDraw  = nLastLine-nFirstLine;
                             nRelMarked    = nLastLine-nFirstLine;
                         }
                         else {
                             nAbsFirstLine = nLastLine-(wYSize-1);
                             nAbsLastLine  = nLastLine;
                             nRelFirstDraw = 0;
                             nRelLastDraw  = (wYSize-1);
                             nRelMarked    = (wYSize-1);
                         }
                     }
                 }
                 else if (c == 'h' || c == 'v') {
                     if (c == 'h') 
                         strcpy(msg, "Help Page: Press 'q' to exit");
                     else 
                         strcpy(msg, "Version Page: Press 'q' to exit");
                     iSelect_Draw(wField, wYSize, wXSize, wYPos, wXPos, nAbsFirstLine, nAbsLastLine, nRelMarked, nRelFirstDraw, nRelLastDraw, nLines, sField, title, name, mField, msg, tagbegin, tagend);
                     wrefresh(wField);

                     hField = newwin(wYSize, wXSize, wYPos, wXPos);
                     werase(hField);
                     if (c == 'h') 
                         cpp = iSelect_Help;
                     else
                         cpp = iSelect_README;
                     for (y = 0; y < wYSize && cpp[y] != NULL; y++) {
                         sprintf(ca, cpp[y]);
                         cp = ca;
                         x = 0;
                         while (1) {
                             if ((cp2 = strstr(cp, "<b>")) != NULL) {
                                 *cp2 = NUL;
                                 wmove(hField, y, x); waddstr(hField, cp); x += strlen(cp);
                                 wattrset(hField, A_NORMAL|A_BOLD);
                                 cp = cp2+3;
                                 cp2 = strstr(cp, "</b>");
                                 *cp2 = NUL;
                                 wmove(hField, y, x); waddstr(hField, cp); x += strlen(cp);
                                 wattrset(hField, A_NORMAL);
                                 cp = cp2+4;
                             }
                             else {
                                 wmove(hField, y, x); waddstr(hField, cp);
                                 break;
                             }
                        }
                     }
                     wrefresh(hField);
                     while (1) {
                         c = wgetch(wField);
                         c = c & 0xff; /* strip down to 8bit */
                         if (c == 'q')
                             break;
                     }
                     delwin(hField);

                     nRelFirstDraw = 0;
                     nRelLastDraw = nAbsLastLine-nAbsFirstLine;
                     strcpy(msg, "");
                     iSelect_Draw(wField, wYSize, wXSize, wYPos, wXPos, nAbsFirstLine, nAbsLastLine, nRelMarked, nRelFirstDraw, nRelLastDraw, nLines, sField, title, name, mField, msg, tagbegin, tagend);
#ifndef USE_SLCURSES
                     redrawwin(wField);
#endif
                     wrefresh(wField);
                 }
                 else {
                     strcpy(msg, "Invalid key. Press 'h' for Help Page!");
                 }
            }
        }
    }

    fflush(stdin);
    echo();
#ifndef USE_SLCURSES
    nocrmode();
#endif
    delwin(wField);

    if (bQuit) 
        return(-1);
    else
        return(nAbsFirstLine+nRelMarked);
}

/*
 *  The iSelect function...
 */
int iSelect(char *caBuf, int pos, char *title, char *name,
            char *tagbegin, char *tagend, int stripco, int stripws, 
            int browsealways, int allselectable,
            int multiselect, int exitnoselect,
            char **keystr) 
{
    WINDOW *wMain;
    int i, j, k;
    char ca[MAXLINELEN], ca2[MAXLINELEN];
    char ca3[MAXLINELEN];
    char *cp, *cps, *cpe;
    struct Line *spLine;
    int rc;
    int ok;

    /*
     *  convert input buffer caBuf into 
     *  spaLines array of browsable strings
     */
    j = 0;
    nLines = 0;
    for (i = 0; caBuf[i] != NUL; i++) {
        if (caBuf[i] != NL) {
            /* line accumulation */
            ca[j++] = caBuf[i];
        }
        else {
            /* end of line accumulation reached, now convert it */
            ca[j++] = NUL;
            j = 0;

            /* skip comment lines */
            if (stripco) {
                if (strnEQ(ca, "#", 1))
                    continue;
            }

            spLine = (struct Line *)malloc(sizeof(struct Line));

            sprintf(ca2, "%ss", tagbegin);
            sprintf(ca3, "%sS", tagbegin);
            if (   (cp = strstr(ca, ca2)) != NULL
                || (cp = strstr(ca, ca3)) != NULL) {
                /* is a selectable line */
                cps = cp;
                cp += strlen(tagbegin)+1;
                if (strnEQ(cp, ":", 1)) {
                    cp++;
                    for (k = 0; !strnEQ(cp, tagend, strlen(tagend)); k++)
                        ca2[k] = *cp++;
                    ca2[k] = NUL;
                    cpe = cp+strlen(tagend);
                    for (; *cpe != NUL; )
                        *cps++ = *cpe++;
                    *cps = NUL;
                }
                else if (strnEQ(cp, tagend, strlen(tagend))) {
                    cpe = cp+strlen(tagend);
                    for (; *cpe != NUL; )
                        *cps++ = *cpe++;
                    *cps = NUL;
                    strcpy(ca2, ca);
                }
                else {
                    CU(EX_FAIL);
                }
                spLine->cpLine = strdup(ca);
                spLine->fSelectable = TRUE;
                spLine->cpResult = strdup(ca2);
            }
            else {
                /*  is normal plain text line  */
                spLine->cpLine = strdup(ca);
                if (allselectable)
                    spLine->fSelectable = TRUE;
                else
                    spLine->fSelectable = FALSE;
                spLine->cpResult = strdup(ca);
            }

            spLine->fSelected = FALSE;
            if (stripws)
                strip(spLine->cpResult);
            spaLines[nLines++] = spLine;
        }
    }
#ifdef DEBUG
    for (i = 0; i < nLines; i++) {
        printf("spaLines[%d] = {\n", i);
        printf("    cpLine      = \"%s\"\n", spaLines[i]->cpLine);
        printf("    fSelectable = %d\n", spaLines[i]->fSelectable);
        printf("    cpResult    = \"%s\"\n", spaLines[i]->cpResult);
        printf("}\n");
    }
#endif

    if (!browsealways && nLines == 0)
        CU(-1);
    if (!browsealways && nLines == 1) {
        spaLines[0]->fSelected = TRUE;
        CU(0);
    }

    if (exitnoselect) {
        ok = FALSE;
        for (i = 0; i < nLines; i++) {
            if (spaLines[i]->fSelectable) {
                ok = TRUE;
                break;
            }
        }
        if (!ok)
            CU(-1);
    }

    /*
     *  setup Curses package and
     *  open our own first window which holds the complete screen
     */
    setup();
    wMain = newwin(LINES, COLS, 0, 0);
    werase(wMain);
    wrefresh(wMain);

    /*
     *  Now run the browser...
     */
    rc = iSelect_Browser(/* Browser: */ LINES-2, COLS-2, 0, 1, pos, multiselect,
                         /* Status:  */ 1, COLS, LINES-2, 0, title, name,
                         /* Message: */ 1, COLS-1, LINES-1, 0,
                         /* Result:  */ keystr,
                         /* Tags:    */ tagbegin, tagend);

    /*  
     *  delete the main window and
     *  close Curses package
     */
    werase(wMain);
    wrefresh(wMain);
    delwin(wMain);
    diesoft();

    CUS:
    return rc;
}

/*EOF*/
