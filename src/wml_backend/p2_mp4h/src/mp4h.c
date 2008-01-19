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

#include "mp4h.h"
#include <error.h>

#include <getopt.h>

/* Enable sync output for /lib/cpp (-s).  */
int sync_output = 0;

/* Debug (-d[flags]).  */
int debug_level = 0;

/* Hash table size (should be a prime) (-Hsize).  */
int hash_table_size = HASHMAX;

/* Max length of arguments in trace output (-lsize).  */
int max_debug_argument_length = 0;

/* Suppress warnings about missing arguments.  */
int suppress_warnings = 0;

/* If not zero, then value of exit status for warning diagnostics.  */
int warning_status = 0;

/* Artificial limit for expansion_level in macro.c.  */
int nesting_limit = 250;

/* Security level */
int safety_level = 0;

/* Document encoding */
encoding_type document_encoding = ENCODING_8BIT;

/* Flags to control how expansion is performed.  */
#define DEFAULT_EXPANSION_FLAGS 3114

int exp_flags = DEFAULT_EXPANSION_FLAGS;

/* Name of frozen file to digest after initialization.  */
const char *frozen_file_to_read = NULL;

/* Name of frozen file to produce near completion.  */
const char *frozen_file_to_write = NULL;

/* True when -F flag is passed.  */
int frozen_dump = 0;

/* The name this program was run with (needed by ../lib/error.c). */
const char *program_name = PACKAGE_NAME;

/* If nonzero, display usage information and exit.  */
static int show_help = 0;

/* If nonzero, print the version on standard output and exit.  */
static int show_version = 0;

struct macro_definition
{
  struct macro_definition *next;
  int code;                     /* D, U or t */
  const char *macro;
};
typedef struct macro_definition macro_definition;


/*---------------------------------------------.
| Print a usage message and exit with STATUS.  |
`---------------------------------------------*/

#define HELP_EXP_FLAGS(n,str) \
    do                        \
      {                       \
        printf(" %c %4d %s\n", \
                ((exp_flags & n) ? '*' : ' '), n, str);  \
      }                       \
    while (0)

static void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             PACKAGE_NAME);
  else
    {
      printf (_("Usage: %s [OPTION]... [FILE]...\n"), PACKAGE_NAME);
      fputs (_("\
Mandatory or optional arguments to long options are mandatory or optional\n\
for short options too.\n\
\n\
Operation modes:\n\
      --help                   display this help and exit\n\
      --version                output version information and exit\n\
  -E, --fatal-warnings         stop execution after first warning\n\
  -Q, --quiet, --silent        suppress some warnings for builtins\n\
  -S, --safety-level=NUMBER    disable hazardous functions\n"),
             stdout);
      fputs (_("\
\n\
Preprocessor features:\n\
  -I, --include=DIRECTORY      search this directory second for includes\n\
  -D, --define=NAME[=VALUE]    enter NAME has having VALUE, or empty\n\
  -U, --undefine=NAME          delete builtin NAME\n\
  -s, --synclines              generate `#line NO \"FILE\"' lines\n"),
             stdout);
      fputs (_("\
\n\
Parser features:\n\
  -c, --caseless=NUMBER        set tags (1), variables (2) or entities (4)\n\
                               case insensitive.  Default value is 3, i.e.\n\
                               only entities are case sensitive\n\
  -e, --encoding=NAME          specify document encoding\n\
  -X, --expansion=NUMBER       set parser behaviour according to the bits\n\
                               of NUMBER, with (star marks current flags)\n\
"), stdout);
      HELP_EXP_FLAGS(    1, "do not parse unknown tags");
      HELP_EXP_FLAGS(    2, "unknown tags are assumed being simple");
      HELP_EXP_FLAGS(    4, "trailing star in tag name do not make this tag simple");
      HELP_EXP_FLAGS(    8, "an unmatched end tag closes all previous unmatched begin tags");
      HELP_EXP_FLAGS(   16, "interpret backslashes as printf");
      HELP_EXP_FLAGS(   32, "remove trailing slash in tag attributes");
      HELP_EXP_FLAGS(   64, "do not remove trailing star in tag name");
      HELP_EXP_FLAGS(  128, "do not remove leading star in tag name");
      HELP_EXP_FLAGS(  256, "do not add a space before trailing slash in tag attributes");
      HELP_EXP_FLAGS( 1024, "suppress warnings about bad nested tags");
      HELP_EXP_FLAGS( 2048, "suppress warnings about missing trailing slash");

      fputs (_("\
\n\
Limits control:\n\
  -H, --hashsize=PRIME         set symbol lookup hash table size\n\
  -L, --nesting-limit=NUMBER   change artificial nesting limit\n"),
             stdout);
      fputs (_("\
\n\
Frozen state files:\n\
  -F, --freeze-state=FILE      produce a frozen state on FILE at end\n\
  -R, --reload-state=FILE      reload a frozen state from FILE at start\n"),
             stdout);
      fputs (_("\
\n\
Debugging:\n\
  -d, --debug=[FLAGS]          set debug level (no FLAGS implies `ae')\n\
  -t, --trace=NAME             trace NAME when it will be defined\n\
  -l, --arglength=NUM          restrict macro tracing size\n\
  -o, --error-output=FILE      redirect debug and trace output\n"),
             stdout);
      fputs (_("\
\n\
FLAGS is any of:\n\
  t   trace for all macro calls, not only debugging-on'ed\n\
  a   show actual arguments\n\
  e   show expansion\n\
  c   show before collect, after collect and after call\n\
  x   add a unique macro call id, useful with c flag\n\
  f   say current input file name\n\
  l   say current input line number\n\
  p   show results of path searches\n\
  m   show results of module operations\n\
  i   show changes in input files\n\
  V   shorthand for all of the above flags\n"),
             stdout);
      fputs (_("\
\n\
If no FILE or if FILE is `-', standard input is read.\n"),
             stdout);

    }
  exit (status);
}

/*--------------------------------------.
| Decode options and launch execution.  |
`--------------------------------------*/

static const struct option long_options[] =
{
  {"arglength", required_argument, NULL, 'l'},
  {"debug", optional_argument, NULL, 'd'},
  {"error-output", required_argument, NULL, 'o'},
  {"expansion", required_argument, NULL, 'X'},
  {"fatal-warnings", no_argument, NULL, 'E'},
  {"freeze-state", required_argument, NULL, 'F'},
  {"hashsize", required_argument, NULL, 'H'},
  {"include", required_argument, NULL, 'I'},
  {"nesting-limit", required_argument, NULL, 'L'},
  {"quiet", no_argument, NULL, 'Q'},
  {"reload-state", required_argument, NULL, 'R'},
  {"silent", no_argument, NULL, 'Q'},
  {"synclines", no_argument, NULL, 's'},
  {"safety-level", required_argument, NULL, 'S'},
  {"encoding", required_argument, NULL, 'e'},
  {"caseless", required_argument, NULL, 'c'},

  {"help", no_argument, NULL, 'h'},
  {"version", no_argument, NULL, 'V'},

  /* These are somewhat troublesome.  */
  { "define", required_argument, NULL, 'D' },
  { "undefine", required_argument, NULL, 'U' },
  { "trace", required_argument, NULL, 't' },

  { 0, 0, 0, 0 },
};

#define OPTSTRING "c:D:e:EF:H:I:L:QR:U:X:d:hl:o:sS:Ot:V"

int
main (int argc, char *const *argv)
{
  macro_definition *head;       /* head of deferred argument list */
  macro_definition *tail;
  macro_definition *new;
  int optchar;                  /* option character */

  macro_definition *defines;
  FILE *fp;
  char *filename;
  int caseless = CASELESS_DEFAULT;

  debug_init ();
  include_init ();

  /* First, we decode the arguments, to size up tables and stuff.  */

  head = tail = NULL;

  while (optchar = getopt_long (argc, argv, OPTSTRING, long_options, NULL),
         optchar != EOF)
    switch (optchar)
      {
      default:
        usage (EXIT_FAILURE);

      case 0:
        break;

      case 'c':
        caseless = atoi (optarg);
        if (caseless <= 0)
          caseless = CASELESS_DEFAULT;
        break;

      case 'D':
      case 'U':
      case 't':

        /* Arguments that cannot be handled until later are accumulated.  */

        new = (macro_definition *) xmalloc (sizeof (macro_definition));
        new->code = optchar;
        new->macro = optarg;
        new->next = NULL;

        if (head == NULL)
          head = new;
        else
          tail->next = new;
        tail = new;

        break;

      case 'e':
        if (strcasecmp(optarg, "8bit") == 0)
            document_encoding = ENCODING_8BIT;
        else if (strcasecmp(optarg, "utf8") == 0)
            document_encoding = ENCODING_UTF8;
        else
            error (0, 0, _("Bad encoding: `%s'"), optarg);
        break;

      case 'E':
        warning_status = EXIT_FAILURE;
        break;

      case 'F':
        frozen_file_to_write = optarg;
        frozen_dump = 1;
        break;

      case 'H':
        hash_table_size = atoi (optarg);
        if (hash_table_size <= 0)
          hash_table_size = HASHMAX;
        break;

      case 'I':
        add_include_directory (optarg);
        break;

      case 'L':
        nesting_limit = atoi (optarg);
        break;

      case 'Q':
        suppress_warnings = 1;
        break;

      case 'R':
        frozen_file_to_read = optarg;
        break;

      case 'd':
        debug_level = debug_decode (optarg);
        if (debug_level < 0)
          {
            error (0, 0, _("Bad debug flags: `%s'"), optarg);
            debug_level = 0;
          }
        break;

      case 'X':
        exp_flags = atoi (optarg);
        if (exp_flags < 0)
          {
            error (0, 0, _("Bad expansion flags: `%s'"), optarg);
            exp_flags = DEFAULT_EXPANSION_FLAGS;
          }
        break;

      case 'l':
        max_debug_argument_length = atoi (optarg);
        if (max_debug_argument_length <= 0)
          max_debug_argument_length = 0;
        break;

      case 'o':
        if (!debug_set_output (optarg))
          error (0, errno, optarg);
        break;

      case 's':
        sync_output = 1;
        break;

      case 'S':
        safety_level = atoi (optarg);
        break;

      case 'V':
        show_version = 1;
        break;

      case 'h':
        show_help = 1;
        break;

      }

  if (show_version)
    {
      printf ("%s (%s)\n", PACKAGE_STRING, PACKAGE_DATE);
      exit (EXIT_SUCCESS);
    }

  if (show_help)
    usage (EXIT_SUCCESS);

  /* Do the basic initialisations.  */

  input_init ();
  output_init ();
  include_env_init ();
  symtab_init ();
  break_init ();
  caseless_init (caseless);
#ifdef HAVE_LOCALE_H
  locale_init (LC_ALL, NULL);
#endif
#ifdef WITH_MODULES
  module_init ();
#endif
  pcre_init ();

  if (frozen_file_to_read)
    reload_frozen_state (frozen_file_to_read);
  else
    builtin_init ();

  /* Handle deferred command line macro definitions.  Must come after
     initialisation of the symbol table.  */

  defines = head;

  while (defines != NULL)
    {
      macro_definition *next;
      char *macro_value;
      symbol *sym;

      switch (defines->code)
        {
        case 'D':
          macro_value = strchr (defines->macro, '=');
          if (macro_value == NULL)
            macro_value = "";
          else
            *macro_value++ = '\0';
          sym = lookup_variable (defines->macro, SYMBOL_INSERT);
          initialize_builtin (sym);
          SYMBOL_TYPE (sym) = TOKEN_TEXT;
          SYMBOL_TEXT (sym) = xstrdup (macro_value);
          break;

        case 'U':
          lookup_symbol (defines->macro, SYMBOL_DELETE);
          break;

        case 't':
          sym = lookup_symbol (defines->macro, SYMBOL_INSERT);
          SYMBOL_TRACED (sym) = TRUE;
          break;

        default:
          MP4HERROR ((warning_status, 0,
            "INTERNAL ERROR: Bad code in deferred arguments"));
          exit (1);
        }

      next = defines->next;
      xfree ((voidstar) defines);
      defines = next;
    }

  /* Handle the various input files.  Each file is pushed on the input,
     and the input read.  Wrapup text is handled separately later.  */

  if (optind == argc)
    {
      push_file (stdin, "stdin");
      expand_input ();
    }
  else
    for (; optind < argc; optind++)
      {
        if (strcmp (argv[optind], "-") == 0)
          push_file (stdin, "stdin");
        else
          {
            fp = path_search (argv[optind], &filename);
            if (fp == NULL)
              {
                error (0, errno, argv[optind]);
                MP4HERROR ((warning_status, 0,
                   _("%s: file skipped"), argv[optind]));
                continue;
              }
            else
              {
                xfree ((voidstar) current_file);
                current_file = xstrdup (filename);
                push_file (fp, filename);
                xfree ((voidstar) filename);
              }
          }
        expand_input ();
      }
#undef NEXTARG

  /* Now handle wrapup text.  */
  
  while (pop_wrapup ())
    expand_input ();

  if (frozen_file_to_write)
    produce_frozen_state (frozen_file_to_write);

  /* Free memory */
  input_deallocate ();
  debug_deallocate ();
  include_deallocate ();
  output_deallocate ();
  symtab_deallocate ();
  break_deallocate ();
  builtin_deallocate ();
  pcre_deallocate ();

  xfree ((voidstar) current_file);

  exit (EXIT_SUCCESS);
}
