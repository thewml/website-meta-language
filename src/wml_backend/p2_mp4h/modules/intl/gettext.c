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

#include <libintl.h>

#define mp4h_macro_table                gettext_LTX_mp4h_macro_table
#define mp4h_init_module                gettext_LTX_mp4h_init_module
#define mp4h_finish_module              gettext_LTX_mp4h_finish_module

module_init_t mp4h_init_module;         /* initialisation function */
module_finish_t mp4h_finish_module;     /* cleanup function */

DECLARE(mp4m_intl_textdomain);
DECLARE(mp4m_intl_bindtextdomain);
DECLARE(mp4m_intl_bind_textdomain_codeset);
DECLARE(mp4m_intl_gettext);

#undef DECLARE

/* The table of builtins defined by this module - just one */

builtin mp4h_macro_table[] =
{
  /* name             container   expand    function
                                attributes                      */

  { "textdomain",       FALSE,    TRUE,   mp4m_intl_textdomain },
  { "bindtextdomain",   FALSE,    TRUE,   mp4m_intl_bindtextdomain },
  { "bind_textdomain_codeset",
                        FALSE,    TRUE,   mp4m_intl_bind_textdomain_codeset },
  { "gettext",           TRUE,    TRUE,   mp4m_intl_gettext },
  { 0,                  FALSE,   FALSE,   0 },
};

void
mp4h_init_module(struct obstack *obs)
{
}

void
mp4h_finish_module(void)
{
}

/* The functions for builtins can be static */

static void
mp4m_intl_textdomain (MP4H_BUILTIN_ARGS)
{
  const char *domain;

  domain = predefined_attribute ("domain", &argc, argv, FALSE);
  if (!domain)
    {
      MP4HERROR ((warning_status, 0,
        _("Warning:%s:%d: In <%s>, required attribute `%s' is not specified"),
           CURRENT_FILE_LINE, ARG (0), "domain"));
      return;
    }
  textdomain (domain);
}

static void
mp4m_intl_bindtextdomain (MP4H_BUILTIN_ARGS)
{
  const char *domain, *path;

  domain = predefined_attribute ("domain", &argc, argv, FALSE);
  path   = predefined_attribute ("path", &argc, argv, FALSE);
  if (!domain)
    {
      MP4HERROR ((warning_status, 0,
        _("Warning:%s:%d: In <%s>, required attribute `%s' is not specified"),
           CURRENT_FILE_LINE, ARG (0), "domain"));
      return;
    }
  if (!path)
    {
      MP4HERROR ((warning_status, 0,
        _("Warning:%s:%d: In <%s>, required attribute `%s' is not specified"),
           CURRENT_FILE_LINE, ARG (0), "path"));
      return;
    }
  bindtextdomain (domain, path);
}

static void
mp4m_intl_bind_textdomain_codeset (MP4H_BUILTIN_ARGS)
{
  const char *domain, *codeset;

  domain  = predefined_attribute ("domain", &argc, argv, FALSE);
  codeset = predefined_attribute ("codeset", &argc, argv, FALSE);
  if (!domain)
    {
      MP4HERROR ((warning_status, 0,
        _("Warning:%s:%d: In <%s>, required attribute `%s' is not specified"),
           CURRENT_FILE_LINE, ARG (0), "domain"));
      return;
    }
  if (!codeset)
    {
      MP4HERROR ((warning_status, 0,
        _("Warning:%s:%d: In <%s>, required attribute `%s' is not specified"),
           CURRENT_FILE_LINE, ARG (0), "codeset"));
      return;
    }
  bind_textdomain_codeset (domain, codeset);
}

static void
mp4m_intl_gettext (MP4H_BUILTIN_ARGS)
{
  const char *domain;
  char *cp, *msgstr;

  domain = predefined_attribute ("domain", &argc, argv, FALSE);
  for (cp = ARGBODY; *cp != '\0'; cp++)
    if (CHAR_SLASH == *cp)
      *cp = '/';

  if (domain)
    msgstr = dgettext(domain, ARGBODY);
  else
    msgstr = gettext(ARGBODY);
  obstack_grow (obs, msgstr, strlen (msgstr));
}

