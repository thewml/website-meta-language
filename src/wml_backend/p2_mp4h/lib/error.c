/* Error handler for noninteractive utilities
   Copyright (C) 1990,91,92,93,94,95,96,97,98 Free Software Foundation, Inc.

   This file is part of the GNU C Library.  Its master source is NOT part of
   the C library, however.  The master source lives in /gd/gnu/lib.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* Written by David MacKenzie <djm@gnu.ai.mit.edu>.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>

#  include <stdarg.h>
#  define VA_START(args, lastarg) va_start(args, lastarg)

#include <stdlib.h>
#include <string.h>

#include "error.h"

#ifndef _
# define _(String) String
#endif

/* If NULL, error will flush stdout, then print on stderr the program
   name, a colon and a space.  Otherwise, error will call this
   function without parameters instead.  */
void (*error_print_progname) ( void );

/* This variable is incremented each time `error' is called.  */
unsigned int error_message_count;

#ifdef _LIBC
/* In the GNU C library, there is a predefined variable for this.  */

# define program_name program_invocation_name
# include <errno.h>

/* In GNU libc we want do not want to use the common name `error' directly.
   Instead make it a weak alias.  */
# define error __error
# define error_at_line __error_at_line

# ifdef USE_IN_LIBIO
# include <libio/iolibio.h>
#  define fflush(s) _IO_fflush (s)
# endif

#else /* not _LIBC */

/* The calling program should define program_name and set it to the
   name of the executing program.  */
extern char *program_name;

# ifdef HAVE_STRERROR_R
#  define __strerror_r strerror_r
# endif	/* HAVE_STRERROR_R */
#endif	/* not _LIBC */

/* Print the program name and error message MESSAGE, which is a printf-style
   format string with optional args.
   If ERRNUM is nonzero, print its corresponding system error message.
   Exit with status STATUS if it is nonzero.  */
/* VARARGS */

void
error (int status, int errnum, const char *message, ...)
{
  va_list args;

  if (error_print_progname)
    (*error_print_progname) ();
  else
    {
      fflush (stdout);
      fprintf (stderr, "%s: ", program_name);
    }

  VA_START (args, message);
  vfprintf (stderr, message, args);
  va_end (args);

  ++error_message_count;
  if (errnum)
    {
#if defined HAVE_STRERROR_R || defined _LIBC
      char errbuf[1024];
      fprintf (stderr, ": %s", __strerror_r (errnum, errbuf, sizeof errbuf));
#else
      fprintf (stderr, ": %s", strerror (errnum));
#endif
    }
  putc ('\n', stderr);
  fflush (stderr);
  if (status)
    exit (status);
}

/* Sometimes we want to have at most one error per line.  This
   variable controls whether this mode is selected or not.  */
int error_one_per_line;

void
error_at_line (int status, int errnum, const char *file_name,
	       unsigned int line_number, const char *message, ...)
{
  va_list args;

  if (error_one_per_line)
    {
      static const char *old_file_name;
      static unsigned int old_line_number;

      if (old_line_number == line_number &&
	  (file_name == old_file_name || !strcmp (old_file_name, file_name)))
	/* Simply return and print nothing.  */
	return;

      old_file_name = file_name;
      old_line_number = line_number;
    }

  if (error_print_progname)
    (*error_print_progname) ();
  else
    {
      fflush (stdout);
      fprintf (stderr, "%s:", program_name);
    }

  if (file_name != NULL)
    fprintf (stderr, "%s:%d: ", file_name, line_number);

  VA_START (args, message);
  vfprintf (stderr, message, args);
  va_end (args);

  ++error_message_count;
  if (errnum)
    {
#if defined HAVE_STRERROR_R || defined _LIBC
      char errbuf[1024];
      fprintf (stderr, ": %s", __strerror_r (errnum, errbuf, sizeof errbuf));
#else
      fprintf (stderr, ": %s", strerror (errnum));
#endif
    }
  putc ('\n', stderr);
  fflush (stderr);
  if (status)
    exit (status);
}

#ifdef _LIBC
/* Make the weak alias.  */
# undef error
# undef error_at_line
weak_alias (__error, error)
weak_alias (__error_at_line, error_at_line)
#endif
