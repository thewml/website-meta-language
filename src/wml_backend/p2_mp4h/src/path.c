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
   Copyright (C) 1989, 90, 91, 92, 93, 98 Free Software Foundation, Inc.
  
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
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* Handling of path search of included files via the builtins "include"
   and "sinclude".  */

#include "mp4h.h"

#define DEBUG_INCL
#undef DEBUG_INCL

static struct search_path_info dirpath; /* the list of path directories */


static void
search_path_add (struct search_path_info *info, const char *dir,
                const int dirlen, const int userdef)
{
  search_path *path;

  path = (struct search_path *) xmalloc (sizeof (struct search_path));
  path->next = NULL;
  if (*dir == '\0')
    {
      path->len = 1;
      path->dir = xstrdup (".");
    }
  else
    {
      if (-1 == dirlen)
        {
          path->len = strlen (dir);
          path->dir = xstrdup (dir);
        }
      else
        {
          path->len = dirlen;
          path->dir = (char *) xcalloc (dirlen + 1, 1);
          strncpy ((char *) path->dir, dir, dirlen);
        }
    }

  if (path->len > info->max_length) /* remember len of longest directory */
    info->max_length = path->len;

  if (userdef)
    {
#ifdef DEBUG_INCL
      fprintf (stderr, "search_path_add user (%s);\n", path->dir);
#endif
      path->next = info->sys;
      if (info->list_end == NULL)
        info->list = path;
      else
        info->list_end->next = path;
      info->list_end = path;
    }
  else
    {
#ifdef DEBUG_INCL
      fprintf (stderr, "search_path_add sys (%s);\n", path->dir);
#endif
      if (info->sys_end == NULL)
        info->sys = path;
      else
        info->sys_end->next = path;
      info->sys_end = path;
    }
}

static void
search_path_env_init (struct search_path_info *info, const char *path,
                const int userdef)
{
  char *path_end;

  if (info == NULL || path == NULL)
    return;

#ifdef DEBUG_INCL
  fprintf (stderr, "search_path_env_init (%s);\n", path);
#endif

  do
    {
      path_end = strchr (path, ':');
      if (path_end)
        search_path_add (info, path, (int) (path_end - path), userdef);
      else
        search_path_add (info, path, -1, userdef);
      path = path_end + 1;
    }
  while (path_end);
}


void
include_init (void)
{
  dirpath.list = NULL;
  dirpath.list_end = NULL;
  dirpath.sys = NULL;
  dirpath.sys_end = NULL;
  dirpath.max_length = strlen (MP4HLIBDIR);
  /*   dirpath.sys must be initialized first.  */
  search_path_env_init (&dirpath, MP4HLIBDIR, 0);
  /*   set dirpath.list to "."  */
  search_path_add (&dirpath, "", -1, 1);
}

void
include_deallocate (void)
{
  search_path *path, *path_next;

  path = dirpath.list;
  while (path)
    {
      path_next = path->next;
      xfree ((voidstar) path->dir);
      xfree ((voidstar) path);
      path = path_next;
    }
}


/* Functions for normal input path search */

void
include_env_init (void)
{
  search_path_env_init (&dirpath, getenv ("MP4HLIB"), 1);
}


void
add_include_directory (const char *dir)
{
  search_path_add (&dirpath, dir, -1, 1);
}

FILE *
path_search (const char *dir, char **expanded_name)
{
  FILE *fp;
  struct search_path *incl;
  char *name;                    /* buffer for constructed name */

  /* Look in current working directory first.  */
  fp = fopen (dir, "r");
  if (fp != NULL)
    {
      if (expanded_name != NULL)
        *expanded_name = xstrdup (dir);
      return fp;
    }

  /* If file not found, and filename absolute, fail.  */
  if (*dir == '/')
    return NULL;

  /* Look into user-defined path directories.  */
  name = (char *) xmalloc (dirpath.max_length + 1 + strlen (dir) + 1);
  for (incl = dirpath.list; incl != NULL; incl = incl->next)
    {
      strncpy (name, incl->dir, incl->len);
      name[incl->len] = '/';
      strcpy (name + incl->len + 1, dir);

#ifdef DEBUG_INCL
      fprintf (stderr, "path_search (%s) -- trying %s\n", dir, name);
#endif

      fp = fopen (name, "r");
      if (fp != NULL)
        {
          if (debug_level & DEBUG_TRACE_PATH)
            DEBUG_MESSAGE2 ("Path search for `%s' found `%s'", dir, name);

          if (expanded_name != NULL)
            *expanded_name = xstrdup (name);
          break;
        }
    }

  xfree ((voidstar) name);

  return fp;
}
