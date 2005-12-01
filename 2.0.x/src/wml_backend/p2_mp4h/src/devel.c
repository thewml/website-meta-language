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
   Copyright (C) 1989, 90, 91, 92, 93, 94, 98 Free Software Foundation, Inc.
  
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

/* Global function useful for builtin.c and loadable modules.  */

#define MP4H_MODULE
#include "mp4h.h"
#undef MP4H_MODULE

static const char * skip_space __P ((const char *));

/*  Stack for predefined attributes.  */
static var_stack *tag_attr = NULL;

/*------------------------------------------------------------------------.
| Give friendly warnings if a builtin macro is passed an inappropriate    |
| number of arguments.  NAME is macro name for messages, ARGC is actual   |
| number of arguments, MIN is the minimum number of acceptable arguments, |
| negative if not applicable, MAX is the maximum number, negative if not  |
| applicable.                                                             |
`------------------------------------------------------------------------*/

boolean
bad_argc (token_data *name, int argc, int min, int max)
{
  boolean isbad = FALSE;

  if (min > 0 && argc < min)
    {
      if (!suppress_warnings)
        MP4HERROR ((warning_status, 0,
          _("Warning:%s:%d: Too few arguments to built-in `%s'"),
               CURRENT_FILE_LINE, TOKEN_DATA_TEXT (name)));
      isbad = TRUE;
    }
  else if (max > 0 && argc > max && !suppress_warnings)
    {
      MP4HERROR ((warning_status, 0,
        _("Warning:%s:%d: Excess arguments to built-in `%s' ignored"),
             CURRENT_FILE_LINE, TOKEN_DATA_TEXT (name)));
    }

  return isbad;
}

/*--------------------------------------------------------------------------.
| The function numeric_arg () converts ARG to an int pointed to by VALUEP.  |
| If the conversion fails, print error message for macro MACRO.  Return     |
| TRUE iff conversion succeeds.                                             |
`--------------------------------------------------------------------------*/
static const char *
skip_space (const char *arg)
{
  while (IS_SPACE (*arg))
    arg++;
  return arg;
}

boolean
numeric_arg (token_data *macro, const char *arg, boolean warn, int *valuep)
{
  char *endp;

  if (*arg == 0 || (*valuep = strtol (skip_space (arg), &endp, 10), 
                    *skip_space (endp) != 0))
    {
      if (warn)
        MP4HERROR ((warning_status, 0,
          _("Warning:%s:%d: Argument `%s' non-numeric in the <%s> tag"),
               CURRENT_FILE_LINE, arg, TOKEN_DATA_TEXT (macro)));
      return FALSE;
    }
  return TRUE;
}

/*----------------------------------------------------------------------.
| Format an int VAL, and stuff it into an obstack OBS.  Used for macros |
| expanding to numbers.                                                 |
`----------------------------------------------------------------------*/

void
shipout_int (struct obstack *obs, int val)
{
  char buf[128];

  sprintf (buf, "%d", val);
  obstack_grow (obs, buf, strlen (buf));
}

/*---------------------.
| Idem, for long int.  |
`---------------------*/

void
shipout_long (struct obstack *obs, long val)
{
  char buf[128];

  sprintf (buf, "%ld", val);
  obstack_grow (obs, buf, strlen (buf));
}

/*----------------------------------------------------------------.
| The shipout_string is used when string length can be computed.  |
`----------------------------------------------------------------*/

void
shipout_string (struct obstack *obs, const char *s, int len)
{
  if (s == NULL)
    s = "";

  if (len == 0)
    len = strlen (s);

  obstack_grow (obs, s, len);
}

/*----------------------------------------------------------.
| Print ARGC arguments from the table ARGV to obstack OBS,  |
| separated by SEP.                                         |
`----------------------------------------------------------*/

void
dump_args (struct obstack *obs, int argc, token_data **argv, const char *sep)
{
  int i;

  for (i = 1; i < argc; i++)
    {
      if (i > 1 && sep)
        obstack_grow (obs, sep, strlen (sep));

      obstack_1grow (obs, CHAR_BGROUP);
      /*   Remove surrounding double quotes  */
      if (*ARG (i) == '"' && LAST_CHAR (ARG (i)) == '"')
        obstack_grow (obs, ARG (i) + 1, strlen (ARG (i)) - 2);
      else
        obstack_grow (obs, ARG (i), strlen (ARG (i)));

      obstack_1grow (obs, CHAR_EGROUP);
    }
}

/*--------------------------------------------------------------------------.
| The function predefined_attribute () reads attributes and returns the     |
| value associated with the key named ``key''.                              |
`--------------------------------------------------------------------------*/

const char *
predefined_attribute (const char *key, int *ptr_argc, token_data **argv,
                      boolean lowercase)
{
  var_stack *next;
  char *cp, *sp, *lower;
  int i, j, special_chars;
  boolean found = FALSE;

  i = 1;
  while (i<*ptr_argc)
    {
      special_chars = 0;
      sp = TOKEN_DATA_TEXT (argv[i]);
      while (IS_GROUP (*sp))
        {
          sp++;
          special_chars++;
        }

      cp = strchr (sp, '=');
      if ((cp == NULL && strcasecmp (sp, key) == 0) ||
          (cp != NULL && strncasecmp (sp, key, strlen (key)) == 0
               && *(sp + strlen (key)) == '='))
        {
          found = TRUE;
          next = (var_stack *) xmalloc (sizeof (var_stack));
          next->prev = tag_attr;
          if (cp)
            {
              next->text = (char *) xmalloc (special_chars + strlen (cp+1) + 1);
              if (special_chars)
                strncpy (next->text, TOKEN_DATA_TEXT (argv[i]), special_chars);
              strcpy (next->text+special_chars, cp+1);
            }
          else
            next->text = xstrdup (key);
          tag_attr = next;

          if (lowercase)
            for (lower=tag_attr->text; *lower != '\0'; lower++)
              *lower = tolower (*lower);

          /* remove this attribute from argv[].  */
          for (j=i+1; j<=*ptr_argc; j++)
            argv[j-1] = argv[j];

          (*ptr_argc)--;
        }
      i++;
    }

  return (found ? tag_attr->text : NULL );
}

/*--------------------------------------------------------------------------.
| Clear stack containing predefined attributes.  This function is called    |
| after macro has been evaluated.                                           |
`--------------------------------------------------------------------------*/

void
clear_tag_attr (void)
{
  var_stack *pa;

  while (tag_attr)
    {
      pa = tag_attr->prev;
      xfree ((voidstar) tag_attr->text);
      xfree ((voidstar) tag_attr);
      tag_attr = pa;
    }
}

