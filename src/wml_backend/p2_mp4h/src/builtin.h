/* mp4h -- A macro processor for HTML documents
   Copyright 2000-2002, Denis Barbier
   All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is a work based on GNU m4 version 1.4n. Below is the
   original copyright.
*/
/* GNU m4 -- A simple macro processor
   Copyright (C) 1998 Free Software Foundation, Inc.
  
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.
  
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
  
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/* Declarations for builtin macros.  */ 

#ifndef BUILTIN_H
#define BUILTIN_H 1

#if defined(HAVE_DIRENT_H) && defined(HAVE_SYS_STAT_H) && \
    defined(HAVE_SYS_TYPES_H) && defined(HAVE_PWD_H) && \
    defined(HAVE_GRP_H) && defined(HAVE_SYS_PARAM_H)
#define HAVE_FILE_FUNCS 1
#else
#undef HAVE_FILE_FUNCS
#endif

#include "pcre.h"

#ifdef HAVE_FILE_FUNCS
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <sys/param.h>
#endif

#include <sys/times.h>
#include <math.h>
#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif
#include <time.h>

/*  From Perl 5.6  */
#ifndef MAXPATHLEN
#  ifdef PATH_MAX
#    ifdef _POSIX_PATH_MAX
#       if PATH_MAX > _POSIX_PATH_MAX
/* MAXPATHLEN is supposed to include the final null character,
 * as opposed to PATH_MAX and _POSIX_PATH_MAX. */
#         define MAXPATHLEN (PATH_MAX+1)
#       else
#         define MAXPATHLEN (_POSIX_PATH_MAX+1)
#       endif
#    else
#      define MAXPATHLEN (PATH_MAX+1)
#    endif
#  else
#    ifdef _POSIX_PATH_MAX
#       define MAXPATHLEN (_POSIX_PATH_MAX+1)
#    else
#       define MAXPATHLEN 4096	/* Err on the large side. */
#    endif
#  endif
#endif

#endif /* BUILTIN_H */
