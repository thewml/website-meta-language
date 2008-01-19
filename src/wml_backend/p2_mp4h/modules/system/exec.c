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

#define MP4H_MODULE
#include <mp4h.h>
#undef MP4H_MODULE

#define mp4h_macro_table                exec_LTX_mp4h_macro_table
#define mp4h_init_module                exec_LTX_mp4h_init_module
#define mp4h_finish_module              exec_LTX_mp4h_finish_module

module_init_t mp4h_init_module;         /* initialisation function */
module_finish_t mp4h_finish_module;     /* cleanup function */

DECLARE(mp4m_system_execute);

#undef DECLARE

builtin mp4h_macro_table[] =
{
  { "exec",              TRUE,    TRUE,   mp4m_system_execute },
  { 0,                  FALSE,    FALSE,  0 },
};

void
mp4h_init_module (struct obstack *obs)
{
}

void
mp4h_finish_module(void)
{
}

static void
mp4m_system_execute (MP4H_BUILTIN_ARGS)
{
  const char *type, *s;
  const builtin *bp;

  type = predefined_attribute ("type", &argc, argv, FALSE);
  if (!type)
    {
      MP4HERROR ((warning_status, errno,
        _("Warning:%s:%d: Missing `type' attribute in <%s>"),
             CURRENT_FILE_LINE, ARG (0)));
      return;
    }

  if (bad_argc (argv[0], argc, 1, 1))
    return;

  s = xstrdup(type);
  bp = find_builtin_by_name(s);
  if (!bp)
    {
      MP4HERROR ((warning_status, errno,
        _("Warning:%s:%d: %s: Undefined type in <%s>"),
             CURRENT_FILE_LINE, type, ARG (0)));
      return;
    }
  (*(bp->func))(MP4H_BUILTIN_RECUR);
}

