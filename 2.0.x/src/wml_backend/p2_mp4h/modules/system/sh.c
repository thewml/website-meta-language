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

#define mp4h_macro_table                sh_LTX_mp4h_macro_table
#define mp4h_init_module                sh_LTX_mp4h_init_module
#define mp4h_finish_module              sh_LTX_mp4h_finish_module

DECLARE(mp4m_system_sh);

#undef DECLARE

builtin mp4h_macro_table[] =
{
  { "sh",                TRUE,    TRUE,   mp4m_system_sh },
  { 0,                  FALSE,    FALSE,  0 },
};

/*   Load `exec' module if not yet loaded.   */
void
mp4h_init_module (struct obstack *obs)
{
  module_load("system/exec", obs);
}

void
mp4h_finish_module(void)
{
}

static void
mp4m_system_sh (MP4H_BUILTIN_ARGS)
{
  const char *verbatim;
  char *script;
  FILE *fp;

  verbatim = predefined_attribute ("verbatim", &argc, argv, TRUE);
  script = xstrdup(ARGBODY);
  remove_special_chars (script, TRUE);

  fp = popen(script, "r");
  if (fp == NULL)
    {
      MP4HERROR ((warning_status, errno,
                  _("Warning:%s:%d: Cannot execute %s"),
                  CURRENT_FILE_LINE, script));
      return;
    }

  push_file (fp, script);
  if (verbatim && strcmp (verbatim, "true") == 0)
    read_file_verbatim (obs);

  xfree ((voidstar) script);
}

