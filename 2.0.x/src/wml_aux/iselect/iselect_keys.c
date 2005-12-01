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
**  iselect_keys.c -- custom Curses Key definition
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
#include "iselect_keys.h"

CustomKey *KeyList[1024] = { NULL }; 

typedef struct keydef {
    char *str;
    int   key;
} keydef;

keydef KeyDef[] = {
    { "SPACE",     ' ' },
    { "RETURN",    '\n' },
    { "KEY_LEFT",  KEY_LEFT },
    { "KEY_RIGHT", KEY_RIGHT },
    { "KEY_UP",    KEY_UP },
    { "KEY_DOWN",  KEY_DOWN },
    { "KEY_NPAGE", KEY_NPAGE },
    { "KEY_PPAGE", KEY_PPAGE },
    { NULL, 0 }
};

char *key2asc(int key) 
{
    char ca[1024];
    int i;

    if (key >= 32 && key <= 126) {
        ca[0] = (char)(key);
        ca[1] = NUL;
        return strdup(ca);
    }
    else {
        for (i = 0; KeyDef[i].str != NULL; i++) {
            if (KeyDef[i].key == key)
                return strdup(KeyDef[i].str);
        }
    }
    return "UNKNOWN";
}

int asc2key(char *str) 
{
    int i;

    if (strlen(str) == 1 && (str[0] >= 32 && str[0] <= 126))
        return (int)(str[0]);
    else {
        for (i = 0; KeyDef[i].str != NULL; i++) {
            if (strcmp(KeyDef[i].str, str) == 0)
                return KeyDef[i].key;
        }
        fprintf(stderr, "ERROR\n");
        exit(1);
    }
}

void configure_custom_key(char *config)
{
    char out[1024];
    char in[1024];
    char *cp;
    CustomKey *kc;
    int i;

    if ((cp = strchr(config, ':')) != NULL) {
        strncpy(in, config, cp-config);
        in[cp-config] = NUL;
        strcpy(out, cp+1);
    }
    else {
        strcpy(in, config);
        strcpy(out, "RETURN");
    }

    kc = (CustomKey *)malloc(sizeof(CustomKey));
    kc->in  = asc2key(in);
    kc->out = asc2key(out);

    for (i = 0; KeyList[i] != NULL; i++)
        ;
    KeyList[i++] = kc;
    KeyList[i++] = NULL;

    return;
}

int do_custom_key(int key)
{
    int i;

    for (i = 0; KeyList[i] != NULL; i++) {
        if (KeyList[i]->in == key)
            key = KeyList[i]->out;
    }
    return key;
}

/*EOF*/
