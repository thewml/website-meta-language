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
**  iselect_keys.h -- custom Curses keys
*/

typedef struct CustomKey {
    int in;
    int out;
} CustomKey;

extern void configure_custom_key(char *config);
extern int  do_custom_key(int key);
extern char *key2asc(int key);
extern int asc2key(char *key);

/*EOF*/
