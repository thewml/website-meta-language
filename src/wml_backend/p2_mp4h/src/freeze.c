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
   Copyright (C) 1989, 90, 91, 92, 93, 94 Free Software Foundation, Inc.

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

/* This module handles frozen files.  */

#include "mp4h.h"

static int lineno;

/*-------------------------------------------------------------------.
| Destructively reverse a symbol list and return the reversed list.  |
`-------------------------------------------------------------------*/

static symbol *
reverse_symbol_list (symbol *sym)
{
  symbol *result;
  symbol *next;

  result = NULL;
  while (sym)
    {
      next = SYMBOL_NEXT (sym);
      SYMBOL_NEXT (sym) = result;
      result = sym;
      sym = next;
    }
  return result;
}

/*------------------------------------------------.
| Produce a frozen state to the given file NAME.  |
`------------------------------------------------*/

void
produce_frozen_state (const char *name)
{
  FILE *file;
  int h;
  symbol *sym;
  const builtin *bp;
  int number[2];

  if (file = fopen (name, "w"), !file)
    {
      MP4HERROR ((warning_status, errno, name));
      return;
    }

  /* Write a recognizable header.  */

  fprintf (file, "# This is a frozen state file generated by %s %s\n",
           PACKAGE, VERSION);
  fprintf (file, "V1\n");

  /* Dump comment delimiters.  */
  if (strcmp (eolcomm.string, DEF_EOLCOMM))
    {
      fprintf (file, "C%d\n", (int) eolcomm.length);
      fputs (eolcomm.string, file);
      fputc ('\n', file);
    }


  /* Dump all symbols.  */

  for (h = 0; h < hash_table_size; h++)
    {

      /* Process all entries in one bucket, from the last to the first.
         This order ensures that, at reload time, pushdef's will be
         executed with the oldest definitions first.  */

      symtab[h] = reverse_symbol_list (symtab[h]);
      for (sym = symtab[h]; sym; sym = SYMBOL_NEXT (sym))
        {
          switch (SYMBOL_TYPE (sym))
            {
            case TOKEN_TEXT:
              if (*SYMBOL_NAME (sym) == '<')
                {
                  fprintf (file, "A%d,%d\n",
                       (int) strlen (SYMBOL_NAME (sym)) - 1,
                       (int) strlen (SYMBOL_TEXT (sym)));
                  fputs (SYMBOL_NAME (sym) + 1, file);
                }
              else
                {
                  fprintf (file, "T%d,%d\n",
                       (int) strlen (SYMBOL_NAME (sym)),
                       (int) strlen (SYMBOL_TEXT (sym)));
                  fputs (SYMBOL_NAME (sym), file);
                }
              fputs (SYMBOL_TEXT (sym), file);
              fputc ('\n', file);
              if (*SYMBOL_NAME (sym) != '<')
                {
                  fputc (SYMBOL_CONTAINER (sym) ? '1' : '0', file);
                  fputc (SYMBOL_EXPAND_ARGS (sym) ? '1' : '0', file);
                  fputc ('\n', file);
                  number[0] = (SYMBOL_HOOK_BEGIN (sym) ?
                          strlen (SYMBOL_HOOK_BEGIN (sym)) : 0);
                  number[1] = (SYMBOL_HOOK_END (sym) ?
                          strlen (SYMBOL_HOOK_END (sym)) : 0);
                  fprintf (file, "%d,%d\n", number[0], number[1]);
                  if (SYMBOL_HOOK_BEGIN (sym))
                    fputs (SYMBOL_HOOK_BEGIN (sym), file);
                  if (SYMBOL_HOOK_END (sym))
                    fputs (SYMBOL_HOOK_END (sym), file);
                  fputc ('\n', file);
                }
              break;

            case TOKEN_FUNC:
              bp = find_builtin_by_addr (SYMBOL_FUNC (sym));
              if (bp == NULL)
                {
                  MP4HERROR ((warning_status, 0, "\
INTERNAL ERROR: Built-in not found in builtin table!"));
                  exit (1);
                }
              fprintf (file, "F%d,%d\n",
                       (int) strlen (SYMBOL_NAME (sym)),
                       (int) strlen (bp->name));
              fputs (SYMBOL_NAME (sym), file);
              fputs (bp->name, file);
              fputc ('\n', file);
              break;

            default:
              MP4HERROR ((warning_status, 0, "\
INTERNAL ERROR: Bad token data type in freeze_one_symbol ()"));
              exit (1);
              break;
            }
        }

      /* Reverse the bucket once more, putting it back as it was.  */

      symtab[h] = reverse_symbol_list (symtab[h]);
    }

  /* All done.  */

  fputs ("# End of frozen state file\n", file);
  fclose (file);
}

/*----------------------------------------------------------------------.
| Issue a message saying that some character is an EXPECTED character.  |
`----------------------------------------------------------------------*/

static void
issue_expect_message (int expected)
{
  if (expected == '\n')
    MP4HERROR ((EXIT_FAILURE, 0, _("%d: Expecting line feed in frozen file"),
                lineno));
  else
    MP4HERROR ((EXIT_FAILURE, 0, _("%d: Expecting character `%c' in frozen file"),
                lineno, expected));
}

/*-------------------------------------------------.
| Reload a frozen state from the given file NAME.  |
`-------------------------------------------------*/

/* We are seeking speed, here.  */

#define GET_CHARACTER \
  (character = getc (file))

#define GET_NUMBER(Number) \
  do                                                            \
    {                                                           \
      (Number) = 0;                                             \
      while (isdigit (character))                               \
        {                                                       \
          (Number) = 10 * (Number) + character - '0';           \
          GET_CHARACTER;                                        \
        }                                                       \
    }                                                           \
  while (0)

#define VALIDATE(Expected) \
  do                                                            \
    {                                                           \
      if (character != (Expected))                              \
        issue_expect_message ((Expected));                      \
    }                                                           \
  while (0)

void
reload_frozen_state (const char *name)
{
  FILE *file;
  int character;
  int operation;
  char *string[2];
  int allocated[2];
  int number[2];
  const builtin *bp;
  symbol *sym, *var;
  boolean container, expand_args;

  file = path_search (name, (char **)NULL);
  if (file == NULL)
    MP4HERROR ((EXIT_FAILURE, errno, _("Cannot open %s"), name));
  lineno = 1;

  allocated[0] = 100;
  string[0] = xmalloc ((size_t) allocated[0]);
  allocated[1] = 100;
  string[1] = xmalloc ((size_t) allocated[1]);

  while (GET_CHARACTER, character != EOF)
    {
    switch (character)
      {
      default:
        MP4HERROR ((EXIT_FAILURE, 0, _("Ill-formated frozen file")));

      case '\n':

        /* Skip empty lines.  */

        lineno++;
        break;

      case '#':

        /* Comments are introduced by `#' at beginning of line, and are
           ignored.  */

        while (character != EOF && character != '\n')
          GET_CHARACTER;
        VALIDATE ('\n');
        lineno++;
        break;

      case 'C':

        /* Change comment strings.  */

        GET_CHARACTER;
        GET_NUMBER (number[0]);
        GET_CHARACTER;
        VALIDATE ('\n');
        lineno++;
        if (number[0] + 1 > allocated[0])
          {
            free (string[0]);
            allocated[0] = number[0] + 1;
            string[0] = xmalloc ((size_t) allocated[0]);
          }

        if (number[0] > 0)
          if (!fread (string[0], (size_t) number[0], 1, file))
            MP4HERROR ((EXIT_FAILURE, 0, _("Premature end of frozen file")));

        string[0][number[0]] = '\0';
        GET_CHARACTER;
        VALIDATE ('\n');
        lineno++;

        eolcomm.string = string[0];
        eolcomm.length = strlen (eolcomm.string);
        break;

      case 'A':
      case 'F':
      case 'T':
        operation = character;
        GET_CHARACTER;

        /* Get string lengths.  Accept a negative diversion number.  */

        number[1] = 0;
        GET_NUMBER (number[0]);
        VALIDATE (',');
        GET_CHARACTER;
        GET_NUMBER (number[1]);
        VALIDATE ('\n');
        lineno++;

        /* Get first string contents.  */

        if (number[0] + 1 > allocated[0])
          {
            free (string[0]);
            allocated[0] = number[0] + 1;
            string[0] = xmalloc ((size_t) allocated[0]);
          }

        if (number[0] > 0)
          if (!fread (string[0], (size_t) number[0], 1, file))
            MP4HERROR ((EXIT_FAILURE, 0, _("Premature end of frozen file")));

        string[0][number[0]] = '\0';

        /* Get second string contents.  */

        if (number[1] + 1 > allocated[1])
          {
            free (string[1]);
            allocated[1] = number[1] + 1;
            string[1] = xmalloc ((size_t) allocated[1]);
          }

        if (number[1] > 0)
          if (!fread (string[1], (size_t) number[1], 1, file))
            MP4HERROR ((EXIT_FAILURE, 0, _("Premature end of frozen file")));

        string[1][number[1]] = '\0';

        GET_CHARACTER;
        VALIDATE ('\n');
        lineno++;

        /* Act according to operation letter.  */

        switch (operation)
          {
          case 'A':

            /* Define a variable.  */

            var = lookup_variable (string[0], SYMBOL_INSERT);
            SYMBOL_TYPE (var) = TOKEN_TEXT;
            SYMBOL_TEXT (var) = xstrdup (string[0]);
            break;

          case 'F':

            /* Enter a macro having a builtin function as a definition.  */

            bp = find_builtin_by_name (string[1]);
            if (bp)
              define_builtin (string[0], bp, 0);
            else
              MP4HERROR ((warning_status, 0, _("\
`%s' from frozen file not found in builtin table!"),
                        string[0]));
            break;

          case 'T':

            GET_CHARACTER;
            container = (character == '1');
            GET_CHARACTER;
            expand_args = (character == '1');
            GET_CHARACTER;
            VALIDATE ('\n');
            lineno++;

            /* Enter a macro having an expansion text as a definition.  */

            define_user_macro (string[0], string[1], SYMBOL_INSERT,
                    container, expand_args, FALSE);

            sym = lookup_symbol (string[0], SYMBOL_LOOKUP);

            /* Add hooks.  */

            GET_CHARACTER;
            GET_NUMBER (number[0]);
            VALIDATE (',');
            GET_CHARACTER;
            GET_NUMBER (number[1]);
            VALIDATE ('\n');
            lineno++;

            if (number[0] > 0)
              {
                if (number[0] + 1 > allocated[0])
                  {
                    free (string[0]);
                    allocated[0] = number[0] + 1;
                    string[0] = xmalloc ((size_t) allocated[0]);
                  }
                if (!fread (string[0], (size_t) number[0], 1, file))
                  MP4HERROR ((EXIT_FAILURE, 0, _("Premature end of frozen file")));

                string[0][number[0]] = '\0';

                SYMBOL_HOOK_BEGIN (sym) = xstrdup (string[0]);

              }

            if (number[1] > 0)
              {
                if (number[1] + 1 > allocated[1])
                  {
                    free (string[1]);
                    allocated[1] = number[1] + 1;
                    string[1] = xmalloc ((size_t) allocated[1]);
                  }
                if (!fread (string[1], (size_t) number[1], 1, file))
                  MP4HERROR ((EXIT_FAILURE, 0, _("Premature end of frozen file")));

                string[1][number[1]] = '\0';

                SYMBOL_HOOK_END (sym) = xstrdup (string[1]);

              }
            GET_CHARACTER;
            VALIDATE ('\n');
            lineno++;

            break;

          default:

            /* Cannot happen.  */

            break;
          }
        break;

      case 'V':

        /* Validate format version.  Only `1' is acceptable for now.  */

        GET_CHARACTER;
        VALIDATE ('1');
        GET_CHARACTER;
        VALIDATE ('\n');
        lineno++;
        break;

      }
    }

  free (string[0]);
  free (string[1]);
  fclose (file);

#undef GET_CHARACTER
#undef GET_NUMBER
#undef VALIDATE
}