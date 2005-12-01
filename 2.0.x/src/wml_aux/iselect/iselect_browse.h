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
**  iselect_browse.h -- Curses-based file browser header
*/

/*
 *  The Structure of our screen lines
 */
struct Line { 
    char *cpLine;      /* the input line */
    int   fSelectable; /* whether selectable or not */
    int   fSelected;   /* whether already selected or not */
    char *cpResult;    /* the result string */
} Line;

#define MAXLINELEN 1024
#define MAXLINES   1024
#define MAXBUF     MAXLINELEN*MAXLINES

extern struct Line *spaLines[MAXLINES];
extern int          nLines;

/*
 *  Prototypes
 */
extern int iSelect(char *caBuf, int pos, char *title, char *name,
                   char *tagbegin, char *tagend, int stripco, int stripws, 
                   int browsealways, int allselectable,
                   int multiselect, int exitnoselect,
                   char **keystr);

/*EOF*/
