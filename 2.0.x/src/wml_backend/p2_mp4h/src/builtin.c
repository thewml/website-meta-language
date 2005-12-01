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

/* Code for all builtin macros, initialisation of symbol table, and
   expansion of user defined macros.  */

#define MP4H_MODULE
#include "mp4h.h"
#undef MP4H_MODULE

#include "builtin.h"

#define CHECK_SAFETY_LEVEL(sl)                                  \
  if (safety_level > sl)                                        \
    {                                                           \
      MP4HERROR ((warning_status, 0,                            \
        _("Warning:%s:%d: `<%s>' ignored due to -S flag"),      \
             CURRENT_FILE_LINE, ARG (0)));                      \
      return;                                                   \
    }

/* Initialisation of builtin and predefined macros.  The table
   "builtin_tab" is both used for initialisation, and by the "builtin"
   builtin.  */

DECLARE (mp4h_bp___file__);
DECLARE (mp4h_bp___line__);
DECLARE (mp4h_bp___version__);
DECLARE (mp4h_bp_lb);
DECLARE (mp4h_bp_rb);
DECLARE (mp4h_bp_dq);
DECLARE (mp4h_bp_bs);
DECLARE (mp4h_bp_timer);
#ifdef HAVE_LOCALE_H
DECLARE (mp4h_bp_mp4h_l10n);
#endif
DECLARE (mp4h_bp_mp4h_output_radix);
#if !defined(HAVE_FILE_FUNCS) || !defined (HAVE_LOCALE_H) || !defined(WITH_MODULES)
DECLARE (mp4h_bp_unsupported);
#endif

  /*  debug functions  */
DECLARE (mp4h_bp_debugmode);
DECLARE (mp4h_bp_debugfile);
DECLARE (mp4h_bp_function_def);
DECLARE (mp4h_bp_debugging_off);
DECLARE (mp4h_bp_debugging_on);

  /*  file functions  */
#ifdef HAVE_FILE_FUNCS
DECLARE (mp4h_bp_get_file_properties);
DECLARE (mp4h_bp_directory_contents);
DECLARE (mp4h_bp_file_exists);
DECLARE (mp4h_bp_real_path);
#endif /* HAVE_FILE_FUNCS */
DECLARE (mp4h_bp_date);

  /*  flow functions  */
DECLARE (mp4h_bp_group);
DECLARE (mp4h_bp_compound);
DECLARE (mp4h_bp_disjoin);
DECLARE (mp4h_bp_noexpand);
DECLARE (mp4h_bp_expand);
DECLARE (mp4h_bp_if);
DECLARE (mp4h_bp_ifeq);
DECLARE (mp4h_bp_ifneq);
DECLARE (mp4h_bp_when);
DECLARE (mp4h_bp_while);
DECLARE (mp4h_bp_foreach);
DECLARE (mp4h_bp_var_case);
DECLARE (mp4h_bp_break);
DECLARE (mp4h_bp_return);
DECLARE (mp4h_bp_warning);
DECLARE (mp4h_bp_exit);
DECLARE (mp4h_bp_at_end_of_file);

  /*  macro functions  */
DECLARE (mp4h_bp_define_tag);
DECLARE (mp4h_bp_provide_tag);
DECLARE (mp4h_bp_let);
DECLARE (mp4h_bp_undef);
DECLARE (mp4h_bp_set_hook);
DECLARE (mp4h_bp_get_hook);
DECLARE (mp4h_bp_attributes_quote);
DECLARE (mp4h_bp_attributes_remove);
DECLARE (mp4h_bp_attributes_extract);
DECLARE (mp4h_bp_define_entity);

  /*  math functions  */
DECLARE (mp4h_bp_add);
DECLARE (mp4h_bp_substract);
DECLARE (mp4h_bp_multiply);
DECLARE (mp4h_bp_divide);
DECLARE (mp4h_bp_modulo);
DECLARE (mp4h_bp_min);
DECLARE (mp4h_bp_max);
DECLARE (mp4h_bp_gt);
DECLARE (mp4h_bp_lt);
DECLARE (mp4h_bp_eq);
DECLARE (mp4h_bp_neq);

  /*  page functions  */
DECLARE (mp4h_bp_include);
DECLARE (mp4h_bp___include);
DECLARE (mp4h_bp_use);
#ifdef WITH_MODULES
DECLARE (mp4h_bp_load);
#endif
DECLARE (mp4h_bp_comment);
DECLARE (mp4h_bp_set_eol_comment);
DECLARE (mp4h_bp_set_quotes);
DECLARE (mp4h_bp_dnl);
DECLARE (mp4h_bp_frozen_dump);

  /*  relational operators  */
DECLARE (mp4h_bp_not);
DECLARE (mp4h_bp_and);
DECLARE (mp4h_bp_or);

  /*  string functions  */
DECLARE (mp4h_bp_downcase);
DECLARE (mp4h_bp_upcase);
DECLARE (mp4h_bp_capitalize);
DECLARE (mp4h_bp_string_length);
DECLARE (mp4h_bp_substring);
DECLARE (mp4h_bp_string_eq);
DECLARE (mp4h_bp_string_neq);
DECLARE (mp4h_bp_string_compare);
DECLARE (mp4h_bp_char_offsets);
DECLARE (mp4h_bp_printf);

  /*  regexp functions  */
DECLARE (mp4h_bp_subst_in_string);
DECLARE (mp4h_bp_subst_in_var);
DECLARE (mp4h_bp_match);

  /*  variable functions  */
DECLARE (mp4h_bp_get_var);
DECLARE (mp4h_bp_get_var_once);
DECLARE (mp4h_bp_set_var_x);
DECLARE (mp4h_bp_set_var);
DECLARE (mp4h_bp_unset_var);
DECLARE (mp4h_bp_preserve);
DECLARE (mp4h_bp_restore);
DECLARE (mp4h_bp_var_exists);
DECLARE (mp4h_bp_increment);
DECLARE (mp4h_bp_decrement);
DECLARE (mp4h_bp_symbol_info);
DECLARE (mp4h_bp_copy_var);
DECLARE (mp4h_bp_defvar);

  /*  array functions  */
DECLARE (mp4h_bp_array_size);
DECLARE (mp4h_bp_array_add_unique);
DECLARE (mp4h_bp_array_member);
DECLARE (mp4h_bp_array_push);
DECLARE (mp4h_bp_array_pop);
DECLARE (mp4h_bp_array_topvalue);
DECLARE (mp4h_bp_array_shift);
DECLARE (mp4h_bp_array_concat);
DECLARE (mp4h_bp_sort);

  /*  diversion functions  */
DECLARE (mp4h_bp_divert);
DECLARE (mp4h_bp_divnum);
DECLARE (mp4h_bp_undivert);

#undef DECLARE

static builtin
builtin_tab[] =
{

  /* name             container   expand    function
                                attributes                      */

  { "__file__",         FALSE,    TRUE,   mp4h_bp___file__ },
  { "__line__",         FALSE,    TRUE,   mp4h_bp___line__ },
  { "__version__",      FALSE,    TRUE,   mp4h_bp___version__ },
  { "lb",               FALSE,    TRUE,   mp4h_bp_lb },
  { "rb",               FALSE,    TRUE,   mp4h_bp_rb },
  { "dq",               FALSE,    TRUE,   mp4h_bp_dq },
  { "bs",               FALSE,    TRUE,   mp4h_bp_bs },
  { "timer",            FALSE,    TRUE,   mp4h_bp_timer },
#ifdef HAVE_LOCALE_H
  { "mp4h-l10n",        FALSE,    TRUE,   mp4h_bp_mp4h_l10n },
#else
  { "mp4h-l10n",        FALSE,    TRUE,   mp4h_bp_unsupported },
#endif
  { "mp4h-output-radix",FALSE,    TRUE,   mp4h_bp_mp4h_output_radix },
  { "date",             FALSE,    TRUE,   mp4h_bp_date },

      /*  debug functions  */
  { "debugmode",        FALSE,    TRUE,   mp4h_bp_debugmode },
  { "debugfile",        FALSE,    TRUE,   mp4h_bp_debugfile },
  { "function-def",     FALSE,    TRUE,   mp4h_bp_function_def },
  { "debugging-off",    FALSE,    TRUE,   mp4h_bp_debugging_off },
  { "debugging-on",     FALSE,    TRUE,   mp4h_bp_debugging_on },

  /*  file functions  */
#ifdef HAVE_FILE_FUNCS
  { "get-file-properties", FALSE, TRUE,   mp4h_bp_get_file_properties },
  { "directory-contents",  FALSE, TRUE,   mp4h_bp_directory_contents },
  { "file-exists",         FALSE, TRUE,   mp4h_bp_file_exists },
  { "real-path",           FALSE, TRUE,   mp4h_bp_real_path },
#else
  { "get-file-properties", FALSE, TRUE,   mp4h_bp_unsupported },
  { "directory-contents",  FALSE, TRUE,   mp4h_bp_unsupported },
  { "file-exists",         FALSE, TRUE,   mp4h_bp_unsupported },
  { "real-path",           FALSE, TRUE,   mp4h_bp_unsupported },
#endif /* HAVE_FILE_FUNCS */

      /*  flow functions  */
  { "group",            FALSE,    TRUE,   mp4h_bp_group },
  { "compound",          TRUE,    TRUE,   mp4h_bp_compound },
  { "disjoin",          FALSE,    TRUE,   mp4h_bp_disjoin },
  { "noexpand",         FALSE,    FALSE,  mp4h_bp_noexpand },
  { "expand",           FALSE,    TRUE,   mp4h_bp_expand },
  { "if",               FALSE,    FALSE,  mp4h_bp_if },
  { "ifeq",             FALSE,    FALSE,  mp4h_bp_ifeq },
  { "ifneq",            FALSE,    FALSE,  mp4h_bp_ifneq },
  { "when",             TRUE,     TRUE,   mp4h_bp_when },
  { "while",            TRUE,     FALSE,  mp4h_bp_while },
  { "foreach",          TRUE,     TRUE,   mp4h_bp_foreach },
  { "var-case",         FALSE,    FALSE,  mp4h_bp_var_case },
  { "break",            FALSE,    TRUE,   mp4h_bp_break },
  { "return",           FALSE,    TRUE,   mp4h_bp_return },
  { "warning",          FALSE,    TRUE,   mp4h_bp_warning },
  { "exit",             FALSE,    TRUE,   mp4h_bp_exit },
  { "at-end-of-file",   TRUE,     TRUE,   mp4h_bp_at_end_of_file },

      /*  macro functions  */
  { "define-tag",       TRUE,     TRUE,   mp4h_bp_define_tag },
  { "provide-tag",      TRUE,     TRUE,   mp4h_bp_provide_tag },
  { "let",              FALSE,    TRUE,   mp4h_bp_let },
  { "undef",            FALSE,    TRUE,   mp4h_bp_undef },
  { "set-hook",         TRUE,     TRUE,   mp4h_bp_set_hook },
  { "get-hook",         FALSE,    TRUE,   mp4h_bp_get_hook },
  { "attributes-quote",    FALSE, TRUE,   mp4h_bp_attributes_quote },
  { "attributes-remove",   FALSE, TRUE,   mp4h_bp_attributes_remove },
  { "attributes-extract",  FALSE, TRUE,   mp4h_bp_attributes_extract },
  { "define-entity",    TRUE,     TRUE,   mp4h_bp_define_entity },

      /*  numerical relational operators  */
  { "gt",               FALSE,    TRUE,   mp4h_bp_gt },
  { "lt",               FALSE,    TRUE,   mp4h_bp_lt },
  { "eq",               FALSE,    TRUE,   mp4h_bp_eq },
  { "neq",              FALSE,    TRUE,   mp4h_bp_neq },

      /*  math functions  */
  { "add",              FALSE,    TRUE,   mp4h_bp_add },
  { "substract",        FALSE,    TRUE,   mp4h_bp_substract },
  { "multiply",         FALSE,    TRUE,   mp4h_bp_multiply },
  { "divide",           FALSE,    TRUE,   mp4h_bp_divide },
  { "modulo",           FALSE,    TRUE,   mp4h_bp_modulo },
  { "min",              FALSE,    TRUE,   mp4h_bp_min },
  { "max",              FALSE,    TRUE,   mp4h_bp_max },

      /*  page functions  */
  { "include",          FALSE,   FALSE,   mp4h_bp_include },
  { "__include",        TRUE,     TRUE,   mp4h_bp___include },
  { "use",              FALSE,    TRUE,   mp4h_bp_use },
#ifdef WITH_MODULES
  { "load",             FALSE,    TRUE,   mp4h_bp_load },
#else
  { "load",             FALSE,    TRUE,   mp4h_bp_unsupported },
#endif
  { "comment",          TRUE,     TRUE,   mp4h_bp_comment },
  { "set-eol-comment",  FALSE,    TRUE,   mp4h_bp_set_eol_comment },
  { "set-quotes",       FALSE,    TRUE,   mp4h_bp_set_quotes },
  { "dnl",              FALSE,    TRUE,   mp4h_bp_dnl },
  { "frozen-dump",      FALSE,    TRUE,   mp4h_bp_frozen_dump },

      /*  relational operators  */
  { "not",              FALSE,    TRUE,   mp4h_bp_not },
  { "and",              FALSE,    TRUE,   mp4h_bp_and },
  { "or",               FALSE,    TRUE,   mp4h_bp_or },

      /*  string functions  */
  { "downcase",         FALSE,    TRUE,   mp4h_bp_downcase },
  { "upcase",           FALSE,    TRUE,   mp4h_bp_upcase },
  { "capitalize",       FALSE,    TRUE,   mp4h_bp_capitalize },
  { "string-length",    FALSE,    TRUE,   mp4h_bp_string_length },
  { "substring",        FALSE,    TRUE,   mp4h_bp_substring },
  { "string-eq",        FALSE,    TRUE,   mp4h_bp_string_eq },
  { "string-neq",       FALSE,    TRUE,   mp4h_bp_string_neq },
  { "string-compare",   FALSE,    TRUE,   mp4h_bp_string_compare },
  { "char-offsets",     FALSE,    TRUE,   mp4h_bp_char_offsets },
  { "printf",           FALSE,    TRUE,   mp4h_bp_printf },
  
      /*  regexp functions  */
  { "subst-in-string",  FALSE,    TRUE,   mp4h_bp_subst_in_string },
  { "subst-in-var",     FALSE,    TRUE,   mp4h_bp_subst_in_var },
  { "match",            FALSE,    TRUE,   mp4h_bp_match },

      /*  variable functions  */
  { "get-var",          FALSE,    TRUE,   mp4h_bp_get_var },
  { "get-var-once",     FALSE,    TRUE,   mp4h_bp_get_var_once },
  { "set-var-x",        TRUE,     TRUE,   mp4h_bp_set_var_x },
  { "set-var",          FALSE,    TRUE,   mp4h_bp_set_var },
  { "set-var-verbatim", FALSE,    FALSE,  mp4h_bp_set_var },
  { "unset-var",        FALSE,    TRUE,   mp4h_bp_unset_var },
  { "preserve",         FALSE,    TRUE,   mp4h_bp_preserve },
  { "restore",          FALSE,    TRUE,   mp4h_bp_restore },
  { "var-exists",       FALSE,    TRUE,   mp4h_bp_var_exists },
  { "increment",        FALSE,    TRUE,   mp4h_bp_increment },
  { "decrement",        FALSE,    TRUE,   mp4h_bp_decrement },
  { "symbol-info",      FALSE,    TRUE,   mp4h_bp_symbol_info },
  { "copy-var",         FALSE,    TRUE,   mp4h_bp_copy_var },
  { "defvar",           FALSE,    TRUE,   mp4h_bp_defvar },

      /*  array functions  */
  { "array-size",       FALSE,    TRUE,   mp4h_bp_array_size },
  { "array-add-unique", FALSE,    TRUE,   mp4h_bp_array_add_unique },
  { "array-member",     FALSE,    TRUE,   mp4h_bp_array_member },
  { "array-push",       FALSE,    TRUE,   mp4h_bp_array_push },
  { "array-pop",        FALSE,    TRUE,   mp4h_bp_array_pop },
  { "array-topvalue",   FALSE,    TRUE,   mp4h_bp_array_topvalue },
  { "array-shift",      FALSE,    TRUE,   mp4h_bp_array_shift },
  { "array-concat",     FALSE,    TRUE,   mp4h_bp_array_concat },
  { "sort",             FALSE,    TRUE,   mp4h_bp_sort },

      /*  diversion functions  */
  { "divert",           FALSE,    TRUE,   mp4h_bp_divert },
  { "divnum",           FALSE,    TRUE,   mp4h_bp_divnum },
  { "undivert",         FALSE,    TRUE,   mp4h_bp_undivert },

  { 0,                  FALSE,    FALSE,  0 },
};

/*   Local functions  */
static void push_builtin_table __P ((builtin *));
static void set_trace __P ((symbol *, const char *));
static void generic_set_hook __P ((MP4H_BUILTIN_PROTO, boolean, int));
static void math_relation __P ((MP4H_BUILTIN_PROTO, mathrel_type));
static void mathop_functions __P ((MP4H_BUILTIN_PROTO, mathop_type));
static void updowncase __P ((struct obstack *, int, token_data **, boolean));
static void varstack_check __P ((void));

static boolean safe_strtod __P ((const char *, const char *, double *));
static boolean safe_strtol __P ((const char *, const char *, long int *));
static void quote_name_value __P ((struct obstack *, char *));
static void matching_attributes __P ((struct obstack *, int, token_data **, boolean, char *));
static char * utf8char_skip __P ((char *, int));
static int utf8char_strlen __P ((char *));
static int encoding_strlen __P ((char *));
static void substitute __P ((struct obstack *, const char *, const char *, int *));
static void string_regexp __P ((struct obstack *, int, token_data **, int, const char *));
static void subst_in_string __P ((struct obstack *, int, token_data **, int));
static void generic_variable_lookup __P ((MP4H_BUILTIN_PROTO, boolean));
static int array_size __P ((symbol *));
static char *array_value __P ((symbol *, int, int *));
static int array_member __P ((const char *, symbol *, boolean));
static int sort_function __P ((const void *, const void *));
static void logical_to_physical_paths __P ((char **));

/*  This symbol controls breakings of flow statements.  */
static symbol varbreak;

/*  Stack preserve/restore variables.  */
static var_stack *vs = NULL;

/*  Global variables needed by sort algorithms.  */
static boolean sort_caseless;
static boolean sort_sortorder;
static boolean sort_numeric;

/*  Localization  */
struct lconv *my_locale;

/*  Table of characters used by PCRE with non-C locales  */
static const unsigned char *re_tableptr = NULL;

/*  Timer  */
static struct tms elapsed_time;

/*  Pointer to a string containig the decimal point used
    with locales.  */
#ifdef HAVE_LOCALE_H
static const char *decimal_point;
#else
#define decimal_point "."
#endif

/*  Output radix  */
static int output_radix = 6;


/*------------------------------------------------------------------.
| If dynamic modules are enabled, more builtin tables can be active |
| at a time.  This implements a list of tables of builtins.         |
`------------------------------------------------------------------*/

struct builtin_table
{
  struct builtin_table *next;
  builtin *table;
};

typedef struct builtin_table builtin_table;

static builtin_table *builtin_tables = NULL;

static void
push_builtin_table (builtin *table)
{
  builtin_table *bt;

  bt = (builtin_table *) xmalloc (sizeof (struct builtin_table));
  bt->next = builtin_tables;
  bt->table = table;
  builtin_tables = bt;
}

/*----------------------------------------.
| Find the builtin, which lives on ADDR.  |
`----------------------------------------*/

const builtin *
find_builtin_by_addr (builtin_func *func)
{
  const builtin_table *bt;
  const builtin *bp;

  for (bt = builtin_tables; bt != NULL; bt = bt->next)
    for (bp = bt->table; bp->name != NULL; bp++)
      if (bp->func == func)
        return bp;
  return NULL;
}

/*-----------------------------------.
| Find the builtin, which has NAME.  |
`-----------------------------------*/

const builtin *
find_builtin_by_name (const char *name)
{
  const builtin_table *bt;
  const builtin *bp;

  /* This is necessary to load frozen files */
  if (builtin_tables == NULL)
    push_builtin_table (builtin_tab);

  for (bt = builtin_tables; bt != NULL; bt = bt->next)
    for (bp = bt->table; bp->name != NULL; bp++)
      if (strcasecmp (bp->name, name) == 0)
        return bp;
  return NULL;
}


/*-----------------------.
| Initialize a symbol.   |
`-----------------------*/

void
initialize_builtin (symbol *sym)
{
  SYMBOL_TYPE (sym)        = TOKEN_VOID;
  SYMBOL_TRACED (sym)      = FALSE;
  SYMBOL_CONTAINER (sym)   = FALSE;
  SYMBOL_EXPAND_ARGS (sym) = FALSE;
  SYMBOL_HOOK_BEGIN (sym)  = NULL;
  SYMBOL_HOOK_END (sym)    = NULL;
}

/*-------------------------------------------------------------------------.
| Install a builtin macro with name NAME, bound to the C function given in |
| BP.  TRACED defines whether  NAME is to be traced.                       |
`-------------------------------------------------------------------------*/

void
define_builtin (const char *name, const builtin *bp, boolean traced)
{
  symbol *sym;

  sym = lookup_symbol (name, SYMBOL_INSERT);
  if (SYMBOL_TYPE (sym) == TOKEN_TEXT)
    xfree ((voidstar) SYMBOL_TEXT (sym));
  SYMBOL_TYPE (sym)        = TOKEN_FUNC;
  SYMBOL_FUNC (sym)        = bp->func;
  SYMBOL_TRACED (sym)      = traced;
  SYMBOL_CONTAINER (sym)   = bp->container;
  SYMBOL_EXPAND_ARGS (sym) = bp->expand_args;
  SYMBOL_HOOK_BEGIN (sym)  = NULL;
  SYMBOL_HOOK_END (sym)    = NULL;
}


/*------------------------------.
| Install a new builtin_table.  |
`------------------------------*/
void
install_builtin_table (builtin *table)
{
  const builtin *bp;

  push_builtin_table (table);

  for (bp = table; bp->name != NULL; bp++)
    define_builtin (bp->name, bp, FALSE);
}

void
break_init (void) {
  initialize_builtin (&varbreak);
  SYMBOL_TYPE (&varbreak) = TOKEN_TEXT;
  SYMBOL_TEXT (&varbreak) = xstrdup ("");
}

void
break_deallocate (void) {
  xfree ((voidstar) SYMBOL_TEXT (&varbreak));
  SYMBOL_TYPE (&varbreak) = TOKEN_VOID;
}

/*-------------------------------------------------------------------------.
| Define a predefined or user-defined macro, with name NAME, and expansion |
| TEXT.  MODE destinguishes between the "define" and the "pushdef" case.   |
| It is also used from main ().                                            |
`-------------------------------------------------------------------------*/

void
define_user_macro (const char *name, char *text, symbol_lookup mode,
    boolean container, boolean expand_args, boolean space_delete)
{
  symbol *s;
  char *begin, *cp;
  int offset, bracket_level = 0;

  s = lookup_symbol (name, mode);
  xfree ((voidstar) SYMBOL_HOOK_BEGIN (s));
  xfree ((voidstar) SYMBOL_HOOK_END (s));
  if (SYMBOL_TYPE (s) == TOKEN_TEXT)
    xfree ((voidstar) SYMBOL_TEXT (s));

  initialize_builtin (s);

  SYMBOL_TYPE (s) = TOKEN_TEXT;
  if (space_delete)
    {
      for (begin=text; *begin != '\0' && IS_SPACE(*begin); begin++)
        ;
      SYMBOL_TEXT (s) = xstrdup (begin);
      offset = 0;
      for (cp=SYMBOL_TEXT (s); *cp != '\0'; cp++)
        {
          switch (*cp)
            {
              case '<':
                bracket_level++;
                *(cp-offset) = *cp;
                break;

              case '>':
                bracket_level--;
                *(cp-offset) = *cp;
                break;

              case '\\':
                *(cp-offset) = *cp;
                if (*(cp+1) != '\0')
                  {
                    cp++;
                    *(cp-offset) = *cp;
                  }
                break;

              case '\n':
                if (bracket_level>0)
                  *(cp-offset) = *cp;
                else
                  {
                  offset++;
                  while (IS_SPACE(*(cp+1)))
                    {
                      cp++;
                      offset++;
                    }
                  }
                break;

              default:
                *(cp-offset) = *cp;
                break;
            }
        }
      *(cp-offset) = '\0';
    }
  else
    SYMBOL_TEXT (s) = xstrdup (text);

  SYMBOL_CONTAINER (s)   = container;
  SYMBOL_EXPAND_ARGS (s) = expand_args;
#ifdef DEBUG_INPUT
  fprintf (stderr, "Define: %s\nText: %s\nContainer: %d\n",
    SYMBOL_NAME (s), SYMBOL_TEXT (s), SYMBOL_CONTAINER (s));
#endif
}

/*--------------------------------------------------------------------------.
| Define a predefined or user-defined entity, with name NAME, and expansion |
| TEXT.  MODE destinguishes between the "define" and the "pushdef" case.    |
| It is also used from main ().                                             |
`--------------------------------------------------------------------------*/

static void
define_user_entity (const char *name, char *text, symbol_lookup mode)
{
  symbol *s;

  s = lookup_entity (name, mode);
  if (SYMBOL_TYPE (s) == TOKEN_TEXT)
    {
      xfree ((voidstar) SYMBOL_HOOK_BEGIN (s));
      xfree ((voidstar) SYMBOL_HOOK_END (s));
      xfree ((voidstar) SYMBOL_TEXT (s));
    }
  initialize_builtin (s);

  SYMBOL_TYPE (s) = TOKEN_TEXT;
  SYMBOL_TEXT (s) = xstrdup (text);
#ifdef DEBUG_INPUT
  fprintf (stderr, "Define: %s\nText: %s\n",
    SYMBOL_NAME (s), SYMBOL_TEXT (s));
#endif
}


/*-----------------------------------------------.
| Initialise all builtin and predefined macros.  |
`-----------------------------------------------*/

void
builtin_init (void)
{
  install_builtin_table (builtin_tab);
  pcre_malloc = xmalloc;
  pcre_free   = xfree;
}

/*-----------------------------------------------.
| Deallocate all builtin and predefined macros.  |
`-----------------------------------------------*/

void
builtin_deallocate (void)
{
  builtin_table *bt, *bt_next;

  for (bt = builtin_tables; bt != NULL; )
    {
      bt_next = bt->next;
      xfree ((voidstar) bt);
      bt = bt_next;
    }
  varstack_check ();
}

static pcre *
xre_compile (const char *pattern, int cflags)
{
  pcre *patcomp;
  const char *errbuf;
  int erroffset;

  if (document_encoding == ENCODING_UTF8)
      cflags |= PCRE_UTF8;
  patcomp = pcre_compile (pattern, cflags, &errbuf, &erroffset, re_tableptr);
  if (patcomp == 0)
    {
      MP4HERROR ((warning_status, 0,
        _("Warning:%s:%d: Bad regular expression `%s' at position %d: %s"),
             CURRENT_FILE_LINE, pattern, erroffset, errbuf));
    }
  return patcomp;
}

static boolean
safe_strtod (const char *name, const char *nptr, double *value)
{
  char *endp;
  double result;

  result = strtod (nptr, &endp);
  if (nptr == NULL || *endp != '\0')
    {
      if (nptr == NULL)
        nptr = endp;
      MP4HERROR ((warning_status, 0,
        _("Warning:%s:%d: Argument `%s' non-numeric in <%s>"),
             CURRENT_FILE_LINE, nptr, name));
      return FALSE;
    }
  *value = result;
  return TRUE;
}

static boolean
safe_strtol (const char *name, const char *nptr, long int *value)
{
  char *endp;
  long int result;

  result = strtol (nptr, &endp, 10);
  if (nptr == NULL || *endp != '\0')
    {
      if (nptr == NULL)
        nptr = endp;
      MP4HERROR ((warning_status, 0,
        _("Warning:%s:%d: Argument `%s' non-numeric in <%s>"),
             CURRENT_FILE_LINE, nptr, name));
      return FALSE;
    }
  *value = result;
  return TRUE;
}

/*----------------------------------------------------------------.
| Quote name=value pair.  This function must be called only when  |
| text is sent to output.                                         |
`----------------------------------------------------------------*/

static void
quote_name_value (struct obstack *obs, char *pair)
{
  char *equal_ptr, *cp;
  int special_chars;
  boolean valid;

  equal_ptr = strchr (pair, '=');
  if (equal_ptr == NULL)
    {
      obstack_grow (obs, pair, strlen (pair));
      return;
    }

  /*  Skip special characters */
  special_chars = 0;
  cp = pair;
  while (IS_GROUP (*cp))
    {
      cp++;
      special_chars++;
    }

  valid = TRUE;
  while (cp < equal_ptr)
    {
      if (IS_SPACE (*cp) || IS_CLOSE (*cp))
        {
          valid = FALSE;
          break;
        }
      cp++;
    }
  if (!valid)
    {
      obstack_grow (obs, pair, strlen (pair));
      return;
    }

  if (special_chars)
    obstack_grow (obs, pair, special_chars);
  *equal_ptr = '\0';
  obstack_grow (obs, pair+special_chars, strlen (pair)-special_chars);
  *equal_ptr = '=';
  obstack_1grow (obs, '=');
  if (*(equal_ptr+1) != '"' && *(equal_ptr+1) != CHAR_QUOTE)
    obstack_1grow (obs, CHAR_QUOTE);
  obstack_grow (obs, equal_ptr+1, strlen (equal_ptr+1)-special_chars);
  if (*(equal_ptr+1) != '"' && *(equal_ptr+1) != CHAR_QUOTE)
    obstack_1grow (obs, CHAR_QUOTE);
  if (special_chars)
    obstack_grow (obs, equal_ptr+1+strlen (equal_ptr+1)-special_chars,
            special_chars);
}

/*---------------------------------------------------------------.
| Extract from ARGV attributes from a list of attributes names.  |
| When MATCH is true, matching attributes are printed, otherwise |
| non matching attributes are printed.                           |
| First argument is a comma-separated list of attributes names,  |
| which may contain regular expressions.                         |
`---------------------------------------------------------------*/

static void
matching_attributes (struct obstack *obs, int argc, token_data **argv,
        boolean match, char *list)
{
  register char *cp;
  char *name;
  int i, j, special_chars;
  pcre *re;
  pcre_extra *re_extra = NULL;
  const char *errptr = NULL;
  int rc, max_subexps;
  int *match_ptr = NULL;
  boolean first = TRUE;

  /*  Replace comma-separated list by a regular expression  */
  for (cp=list; *cp != '\0'; cp++)
    {
      if (*cp == ',')
        *cp = '|';
    }
  name = (char *) xmalloc (strlen (list) + 3);
  sprintf (name, "^%s$", list);

  /*  Compile this expression once  */
  re = xre_compile (name, PCRE_CASELESS);
  xfree ((voidstar) name);
  if (re == NULL)
    return;

  if (argc>2)
    {
      re_extra = pcre_study (re, 0, &errptr);
      if (errptr != NULL)
        {
          MP4HERROR ((warning_status, 0,
            _("Error:%s:%d: Bad regular expression `%s': %s"),
                 CURRENT_FILE_LINE, list, errptr));
          return;
        }
    }

  for (i=1; i<argc; i++)
    {
      special_chars = 0;
      cp = ARG (i);
      while (IS_GROUP (*cp))
        {
          cp++;
          special_chars++;
        }

      if (*(ARG (i)+special_chars) != '\0')
        {
          cp = strchr (ARG (i)+special_chars, '=');
          if (cp)
            *cp  = '\0';
        }

      /*  Ensure that all subexpressions are stored  */
      max_subexps = 0;
      do
        {
          max_subexps += 10;
          match_ptr = (int *)
             xrealloc ((voidstar) match_ptr, sizeof (int) * max_subexps * 3);
          rc = pcre_exec (re, re_extra, ARG (i)+special_chars,
                  strlen (ARG (i)+special_chars), 0,
                  0, match_ptr, max_subexps * 3);
        }
      while (rc == PCRE_ERROR_NOMEMORY);
      max_subexps -= 10;

      if (rc > 0 && match)
        {
          if (!first)
            obstack_1grow (obs, ' ');
          first = FALSE;
          obstack_1grow (obs, CHAR_BGROUP);
          if (special_chars)
            obstack_grow (obs, ARG (i), special_chars);

          if (rc == 1)
            {
              obstack_grow (obs, ARG (i)+special_chars,
                       strlen (ARG (i)+special_chars));
            }
          else
            {
              for (j=1; j<rc; j++)
                obstack_grow (obs, ARG (i)+special_chars+match_ptr[j*2],
                        match_ptr[j*2+1] - match_ptr[j*2]);
            }

          if (cp)
            {
              obstack_1grow (obs, '=');
              obstack_grow (obs, cp+1, strlen (cp+1));
            }
          obstack_1grow (obs, CHAR_EGROUP);
        }
      else if (rc == PCRE_ERROR_NOMATCH && !match)
        {
          if (cp)
            *cp  = '=';
          if (!first)
            obstack_1grow (obs, ' ');
          first = FALSE;
          obstack_1grow (obs, CHAR_BGROUP);
          obstack_grow (obs, ARG (i), strlen (ARG (i)));
          obstack_1grow (obs, CHAR_EGROUP);
        }
    }
  pcre_free (re);
  pcre_free (re_extra);
  xfree ((voidstar) match_ptr);
}


/*
   The rest of this file is the code for builtins and expansion of user
   defined macros.  All the functions for builtins have a prototype as:
   
     void mp4h_bp_MACRONAME (
        struct obstack *obs,
        int argc,
        char *argv[],
        read_type expansion
     );
   
   The function are expected to leave their expansion on the obstack OBS,
   as an unfinished object.  ARGV is a table of ARGC pointers to the
   individual arguments to the macro.  Please note that in general
   argv[argc] != NULL.
   For container tags, argv[argc] contains the body function.
*/

/* Notes for hackers :
    o  Execution must not depend on argv[0]. Indeed, user may define
       synonyms with <let myfunc primitive>
       
    o  Last argument is removed by collect_arguments () if it is empty.
       For this reason, it does not make sense to define a mimimal
       number of arguments.
*/


/* Miscellaneous builtins -- "__file__" and "__line__".  */

static void
mp4h_bp___file__ (MP4H_BUILTIN_ARGS)
{
  if (argc == 1)
    shipout_string (obs, current_file, 0);
  else
    {
      xfree ((voidstar) current_file);
      current_file = xstrdup (ARG (1));
    }
}

static void
mp4h_bp___line__ (MP4H_BUILTIN_ARGS)
{
  if (argc == 1)
    shipout_int (obs, current_line);
  else
    numeric_arg (argv[0], ARG (1), FALSE, &current_line);
}

/*-----------------------------.
| Initialize character table.  |
`-----------------------------*/

void
pcre_init (void)
{
  re_tableptr = pcre_maketables ();
}

void
pcre_deallocate (void)
{
  pcre_free ((voidstar) re_tableptr);
}

/*---------------------.
| Initialize locales.  |
`---------------------*/

#ifdef HAVE_LOCALE_H
void
locale_init (int category, char *value)
{
  setlocale (category, value);
  my_locale = localeconv ();
  decimal_point = my_locale->decimal_point;
}

/*---------------------------.
| Change locale attributes.  |
`---------------------------*/

static void
mp4h_bp_mp4h_l10n (MP4H_BUILTIN_ARGS)
{
  char *cp;
  int i;
  int category;

  for (i=1; i<argc; i++)
    {
      category = -1;
      cp = strchr (ARG (i), '=');
      if (cp)
        {
          *cp = '\0';
          if (strcmp (ARG (i), "LC_ALL") == 0)
            category = LC_ALL;
          else if (strcmp (ARG (i), "LC_COLLATE") == 0)
            category = LC_COLLATE;
          else if (strcmp (ARG (i), "LC_CTYPE") == 0)
            category = LC_CTYPE;
          else if (strcmp (ARG (i), "LC_MESSAGES") == 0)
            category = LC_MESSAGES;
          else if (strcmp (ARG (i), "LC_MONETARY") == 0)
            category = LC_MONETARY;
          else if (strcmp (ARG (i), "LC_NUMERIC") == 0)
            category = LC_NUMERIC;
          else if (strcmp (ARG (i), "LC_TIME") == 0)
            category = LC_TIME;
        }

      if (category == -1)
        MP4HERROR ((warning_status, 0,
        _("Warning:%s:%d: Unknown locale `%s'"), CURRENT_FILE_LINE, ARG (i)));
      else
        {
          locale_init (category, cp+1);
          pcre_deallocate ();
          pcre_init ();
        }
    }

  /*  Compute syntax table */
  syntax_init ();
}

#endif /* HAVE_LOCALE_H */

/*----------------------------------------------.
| Set output radix used when printing numbers.  |
`----------------------------------------------*/

static void
mp4h_bp_mp4h_output_radix (MP4H_BUILTIN_ARGS)
{
  if (bad_argc (argv[0], argc, 0, 2))
    return;

  safe_strtol (ARG (0), ARG (1), (long int *) &output_radix);
}

static void
mp4h_bp_timer (MP4H_BUILTIN_ARGS)
{
  char buf[128];

  times(&elapsed_time);
  sprintf (buf, "user %d\nsys %d\n", (int) elapsed_time.tms_utime,
           (int) elapsed_time.tms_stime);
  obstack_grow (obs, buf, strlen (buf));
}

#if !defined(HAVE_FILE_FUNCS) || !defined (HAVE_LOCALE_H) || !defined(WITH_MODULES)
static void
mp4h_bp_unsupported (MP4H_BUILTIN_ARGS)
{
  MP4HERROR ((warning_status, 0,
    _("Warning:%s:%d: The <%s> tag is not implemented on your OS\n"),
         CURRENT_FILE_LINE, ARG (0)));
}
#endif

static void
mp4h_bp___version__ (MP4H_BUILTIN_ARGS)
{
  obstack_grow (obs, PACKAGE_VERSION, strlen (PACKAGE_VERSION));
}

static void
add_1char_protected (struct obstack *obs, char ch)
{
  obstack_1grow (obs, CHAR_LQUOTE);
  obstack_1grow (obs, ch);
  obstack_1grow (obs, CHAR_RQUOTE);
}

static void
mp4h_bp_lb (MP4H_BUILTIN_ARGS)
{
  add_1char_protected (obs, '<');
}

static void
mp4h_bp_rb (MP4H_BUILTIN_ARGS)
{
  add_1char_protected (obs, '>');
}

static void
mp4h_bp_dq (MP4H_BUILTIN_ARGS)
{
  obstack_1grow (obs, CHAR_QUOTE);
}

static void
mp4h_bp_bs (MP4H_BUILTIN_ARGS)
{
  add_1char_protected (obs, '\\');
}


/* Enable tracing of all specified macros, or all, if none is specified.
   Tracing is disabled by default, when a macro is defined.  This can be
   overridden by the "t" debug flag.  */

/*-----------------------------------------------------------------------.
| Set_trace () is used by "debugging-on" and "debugging-off" to enable   |
| and disable tracing of a macro.  It disables tracing if DATA is NULL,  |
| otherwise it enable tracing.                                           |
`-----------------------------------------------------------------------*/

static void
set_trace (symbol *sym, const char *data)
{
  SYMBOL_TRACED (sym) = (boolean) (data != NULL);
}

static void
mp4h_bp_debugging_on (MP4H_BUILTIN_ARGS)
{
  symbol *s;
  int i;

  if (argc == 1)
    hack_all_symbols (set_trace, (char *) obs);
  else
    for (i = 1; i < argc; i++)
      {
        s = lookup_symbol (ARG (i), SYMBOL_LOOKUP);
        if (s != NULL)
          set_trace (s, (char *) obs);
        else
          MP4HERROR ((warning_status, 0,
            _("Warning:%s:%d: Undefined name %s"),
                 CURRENT_FILE_LINE, ARG (i)));
      }
}

/*------------------------------------------------------------------------.
| Disable tracing of all specified macros, or all, if none is specified.  |
`------------------------------------------------------------------------*/

static void
mp4h_bp_debugging_off (MP4H_BUILTIN_ARGS)
{
  symbol *s;
  int i;

  if (argc == 1)
    hack_all_symbols (set_trace, NULL);
  else
    for (i = 1; i < argc; i++)
      {
        s = lookup_symbol (ARG (i), SYMBOL_LOOKUP);
        if (s != NULL)
          set_trace (s, NULL);
        else
          MP4HERROR ((warning_status, 0,
            _("Warning:%s:%d: Undefined name %s"),
                 CURRENT_FILE_LINE, ARG (i)));
      }
}

/*----------------------------------------------------------------------.
| On-the-fly control of the format of the tracing output.  It takes one |
| argument, which is a character string like given to the -d option, or |
| none in which case the debug_level is zeroed.                         |
`----------------------------------------------------------------------*/
static void
mp4h_bp_debugmode (MP4H_BUILTIN_ARGS)
{
  int new_debug_level;
  int change_flag;

  if (bad_argc (argv[0], argc, 0, 2))
    return;

  if (argc == 1)
    debug_level = 0;
  else
    {
      if (ARG (1)[0] == '+' || ARG (1)[0] == '-')
        {
          change_flag = ARG (1)[0];
          new_debug_level = debug_decode (ARG (1) + 1);
        }
      else
        {
          change_flag = 0;
          new_debug_level = debug_decode (ARG (1));
        }

      if (new_debug_level < 0)
        MP4HERROR ((warning_status, 0,
          _("Warning:%s:%d: Bad debug flags: `%s'"),
               CURRENT_FILE_LINE, ARG (1)));
      else
        {
          switch (change_flag)
            {
            case 0:
              debug_level = new_debug_level;
              break;

            case '+':
              debug_level |= new_debug_level;
              break;

            case '-':
              debug_level &= ~new_debug_level;
              break;
            }
        }
    }
}

/*-------------------------------------------------------------------------.
| Specify the destination of the debugging output.  With one argument, the |
| argument is taken as a file name, with no arguments, revert to stderr.   |
`-------------------------------------------------------------------------*/

static void
mp4h_bp_debugfile (MP4H_BUILTIN_ARGS)
{
  if (bad_argc (argv[0], argc, 1, 2))
    return;

  if (argc == 1)
    debug_set_output (NULL);
  else if (!debug_set_output (ARG (1)))
    MP4HERROR ((warning_status, errno,
      _("Warning:%s:%d: Cannot set error file: %s"),
           CURRENT_FILE_LINE, ARG (1)));
}

/*---------------------------------------------------------.
| Print the replacement text for a builtin or user macro.  |
`---------------------------------------------------------*/
static void
mp4h_bp_function_def (MP4H_BUILTIN_ARGS)
{
  symbol *s;

  if (bad_argc (argv[0], argc, 2, 2))
    return;

  s = lookup_symbol (ARG (1), SYMBOL_LOOKUP);
  if (s == NULL)
    return;

  switch (SYMBOL_TYPE (s))
    {
    case TOKEN_TEXT:
      obstack_1grow (obs, CHAR_LQUOTE);
      shipout_string (obs, SYMBOL_TEXT (s), strlen (SYMBOL_TEXT (s)));
      obstack_1grow (obs, CHAR_RQUOTE);
      break;

    case TOKEN_FUNC:
      push_macro (SYMBOL_FUNC (s), SYMBOL_TRACED (s));
      break;

    case TOKEN_VOID:
      break;

    default:
      MP4HERROR ((warning_status, 0,
        "INTERNAL ERROR: Bad symbol type in mp4h_bp_function_def ()"));
      exit (1);
    }
}


/*  file functions  */

#ifdef HAVE_FILE_FUNCS

/*-----------------------------------------------------------------.
| Informations on a file.  A newline separated string is printed:  |
|    Line 1: file size                                             |
|    Line 2: file type                                             |
|    Line 3: time of last change                                   |
|    Line 4: time of last modification                             |
|    Line 5: time of last modification                             |
|    Line 6: time of last access                                   |
|    Line 7: name of the owner of this file                        |
|    Line 8: name of the group of this file                        |
`-----------------------------------------------------------------*/
static void
mp4h_bp_get_file_properties (MP4H_BUILTIN_ARGS)
{
  struct stat buf;
  struct passwd *user;
  struct group *group;

  CHECK_SAFETY_LEVEL(1);

  if (bad_argc (argv[0], argc, 2, 2))
    return;
  
  if (stat (ARG (1), &buf))
    {
      MP4HERROR ((warning_status, errno,
        _("Warning:%s:%d: Could not stat `%s'"),
             CURRENT_FILE_LINE, ARG (1)));
      return;
    }
  shipout_long (obs, (long) buf.st_size);
  obstack_1grow (obs, '\n');
  if (S_ISLNK(buf.st_mode))
    obstack_grow (obs, "LINK", 4);
  else if (S_ISREG(buf.st_mode))
    obstack_grow (obs, "FILE", 4);
  else if (S_ISDIR(buf.st_mode))
    obstack_grow (obs, "DIR", 3);
  else
    obstack_grow (obs, "UNKNOWN", 7);

  obstack_1grow (obs, '\n');
  shipout_long (obs, (long) buf.st_ctime);
  obstack_1grow (obs, '\n');
  shipout_long (obs, (long) buf.st_mtime);
  obstack_1grow (obs, '\n');
  shipout_long (obs, (long) buf.st_atime);
  obstack_1grow (obs, '\n');
  user = getpwuid (buf.st_uid);
  if (user)
    obstack_grow (obs, user->pw_name, strlen (user->pw_name));
  else
    obstack_grow (obs, "(UNKNOWN)", 9);
  obstack_1grow (obs, '\n');
  group = getgrgid (buf.st_gid);
  if (group)
    obstack_grow (obs, group->gr_name, strlen (group->gr_name));
  else
    obstack_grow (obs, "(UNKNOWN)", 9);
  obstack_1grow (obs, '\n');
}

/*-----------------------------------------------------------------.
| Prints a directory contents.  A pattern can be specified with    |
| the ``matching'' attribute.  This pattern is a regexp one,       |
| not a shell expansion.                                           |
`-----------------------------------------------------------------*/
static void
mp4h_bp_directory_contents (MP4H_BUILTIN_ARGS)
{
  DIR *dir;
  struct dirent *entry;
  int length;
  const char *matching;
  pcre *re = NULL;
  pcre_extra *re_extra = NULL;
  const char *errptr = NULL;
  int *match_ptr = NULL;
  boolean first = TRUE;

  CHECK_SAFETY_LEVEL(1);

  matching = predefined_attribute ("matching", &argc, argv, FALSE);
  if (bad_argc (argv[0], argc, 2, 2))
    return;

  /*  First, opens directory.  */
  dir = opendir (ARG (1));

  if (dir == NULL)
    return;

  if (matching)
    {
      re = xre_compile (matching, 0);
      if (re == NULL)
        return;
      match_ptr = (int *)
         xrealloc ((voidstar) match_ptr, sizeof (int) * 3);
      re_extra = pcre_study (re, 0, &errptr);
      if (errptr != NULL)
        {
          MP4HERROR ((warning_status, 0,
            _("Error:%s:%d: Bad regular expression `%s': %s"),
                 CURRENT_FILE_LINE, matching, errptr));
          return;
        }
    }

  while ((entry = readdir (dir)) != (struct dirent *)NULL)
    {
      length = strlen (entry->d_name);
      if (!matching ||
          (pcre_exec (re, re_extra, entry->d_name, length, 0,
                    0, match_ptr, 3) > 0
           && match_ptr[1] - match_ptr[0] == length))
        {
          if (!first)
            obstack_1grow (obs, '\n');
          obstack_grow (obs, entry->d_name, length);
          first = FALSE;
        }
    }

  if (matching)
    {
      pcre_free (re);
      pcre_free (re_extra);
      xfree ((voidstar) match_ptr);
    }
  closedir (dir);
}

/*--------------------------------.
| Returns "true" if file exists.  |
`--------------------------------*/
static void
mp4h_bp_file_exists (MP4H_BUILTIN_ARGS)
{
  struct stat file;

  CHECK_SAFETY_LEVEL(1);

  if (bad_argc (argv[0], argc, 2, 2))
    return;

  if ((stat (ARG (1), &file)) != -1)
    obstack_grow (obs, "true", 4);
}

/*------------------------------------.
| Returns a real (absolute) path name |
`------------------------------------*/
static void
mp4h_bp_real_path (MP4H_BUILTIN_ARGS)
{
  const char *pathname;
  char resolvedname[MAXPATHLEN];

  CHECK_SAFETY_LEVEL(1);

  pathname = predefined_attribute ("pathname", &argc, argv, FALSE);
  if (!pathname)
    MP4HERROR ((warning_status, 0,
      _("Warning:%s:%d: In <%s>, required attribute `%s' is not specified"),
           CURRENT_FILE_LINE, ARG (0), "pathname"));
  else
    if (!realpath(pathname, resolvedname))
      MP4HERROR ((warning_status, errno,
        _("Error:%s:%d: Cannot form real path for `%s'"),
          CURRENT_FILE_LINE, pathname));
    else
      obstack_grow(obs, resolvedname, strlen(resolvedname));
}
#endif /* HAVE_FILE_FUNCS */

/*------------------------------------------------------------.
| Converts an epoch to a readable string.                     |
| If no argument is given, current date and time is printed.  |
`------------------------------------------------------------*/
static void
mp4h_bp_date (MP4H_BUILTIN_ARGS)
{
  time_t epoch_time = (time_t)0;
  char *ascii_time;
  const char *format, *timespec;
  struct tm *timeptr;
  char *endp = NULL;

  format = predefined_attribute ("format", &argc, argv, FALSE);
  timespec = predefined_attribute ("time", &argc, argv, FALSE);
  
  if (!format && !timespec)
    /* backwards compatible... */
    if (argc > 1)
      timespec = ARG (1);

  if (timespec)
    {
      epoch_time = strtol (timespec, &endp, 10);
      if (!endp)
        {
          MP4HERROR ((warning_status, 0,
            _("Warning:%s:%d: Invalid value in date: %s"),
                 CURRENT_FILE_LINE, ARG (1)));
          return;
        }
    }
  else
    epoch_time = time ((time_t *)NULL);

  timeptr = (struct tm *) localtime (&epoch_time);
  if (format) 
    {
      ascii_time = (char *)xmalloc(1000);
      strftime(ascii_time, 1000, format, timeptr);
      obstack_grow (obs, ascii_time, strlen (ascii_time));
      xfree((voidstar) ascii_time);
    }
  else
    {
      ascii_time = (char *) asctime (timeptr);
      /*  Remove newline.  */
      LAST_CHAR (ascii_time) = '\0';
      obstack_grow (obs, ascii_time, strlen (ascii_time));
    }
}


/*  Flow functions: these functions allow complex structures
    There are 2 different kinds of conditions : numerical and
    string tests.  A numerical test is false if result is null
    and right if non null.  String-based tests are false if
    followed by a null string, and right otherwise.  */

/*----------------------------------------------------------.
| Group multiple tags into one.  This is useful when        |
| combined with tests like <if>, <ifeq>, ....               |
`----------------------------------------------------------*/
static void
mp4h_bp_group (MP4H_BUILTIN_ARGS)
{
  const char *separator;

  separator = predefined_attribute ("separator", &argc, argv, FALSE);

  obstack_1grow (obs, CHAR_BGROUP);
  /*  separator = NULL is handled by dump_args */
  dump_args (obs, argc, argv, separator);
  obstack_1grow (obs, CHAR_EGROUP);
}

/*------------------------------------------------.
| Like <group>, but this one is a container tag.  |
`------------------------------------------------*/
static void
mp4h_bp_compound (MP4H_BUILTIN_ARGS)
{
  const char *separator;

  separator = predefined_attribute ("separator", &argc, argv, FALSE);

  obstack_1grow (obs, CHAR_BGROUP);
  /*  separator = NULL is handled by dump_args */
  dump_args (obs, argc, argv, separator);
  obstack_grow (obs, ARGBODY, strlen (ARGBODY));
  obstack_1grow (obs, CHAR_EGROUP);
}

/*------------------------------------------------.
| Allow breaking an object into several pieces.   |
`------------------------------------------------*/
static void
mp4h_bp_disjoin (MP4H_BUILTIN_ARGS)
{
  obstack_1grow (obs, CHAR_EGROUP);
  obstack_grow (obs, ARG(1), strlen (ARG(1)));
  obstack_1grow (obs, CHAR_BGROUP);
}

/*-------------------------------------------.
| Prevent further expansion of attributes.   |
`-------------------------------------------*/
static void
mp4h_bp_noexpand (MP4H_BUILTIN_ARGS)
{
  obstack_1grow (obs, CHAR_LQUOTE);
  dump_args (obs, argc, argv, NULL);
  obstack_1grow (obs, CHAR_RQUOTE);
}

/*----------------------------------------------------------.
| Remove special characters which forbid expansion.         |
`----------------------------------------------------------*/
static void
mp4h_bp_expand (MP4H_BUILTIN_ARGS)
{
  int i, offset;
  register char *cp;
  
  for (i=1; i<argc; i++)
    {
      offset = 0;
      for (cp=ARG (i); *cp != '\0'; cp++)
        {
          if (*cp == CHAR_LQUOTE || *cp == CHAR_RQUOTE)
            offset++;
          else
            *(cp-offset) = *cp;
        }
      *(cp-offset) = '\0';
    }

  dump_args (obs, argc, argv, NULL);
}

/*--------------------------------------------------------------.
| If followed by a non-empty string, second argument is         |
| expanded, otherwise 3rd argument is read.                     |
`--------------------------------------------------------------*/
static void
mp4h_bp_if (MP4H_BUILTIN_ARGS)
{
  symbol *var;

  if (bad_argc (argv[0], argc, 0, 4))
    return;
  if (argc == 1)
    return;

  /*  The ``natural'' way to implement <if/> is with
        obstack_grow (obs, "<when ", 6);
        obstack_grow (obs, ARG (1), strlen (ARG (1)));
        obstack_grow (obs, " >", 2);
        obstack_grow (obs, ARG (2), strlen (ARG (2)));
        obstack_grow (obs, "</when>", 7);
      and a similar else-clause.
      But this is broken, because ARG(2) may change the condition, and
      then else-clause could be run too.
      For this reason, the following implementation is preferred.
  */
  var = lookup_variable ("__cond_cnt", SYMBOL_LOOKUP);
  if (var == NULL)
    {
      var = lookup_variable ("__cond_cnt", SYMBOL_INSERT);
      SYMBOL_TEXT (var) = (char *) xstrdup ("0");
      SYMBOL_TYPE (var) = TOKEN_TEXT;
    }

  obstack_grow (obs, "<increment __cond_cnt />", 24);
  obstack_grow (obs, "<set-var __cond_var<get-var __cond_cnt />=", 42);
  obstack_1grow (obs, CHAR_BGROUP);
  obstack_grow (obs, ARG (1), strlen (ARG (1)));
  obstack_1grow (obs, CHAR_EGROUP);
  obstack_grow (obs, " />", 3);
  obstack_grow (obs, "<when <get-var __cond_var<get-var __cond_cnt /> /> >", 52);
  obstack_grow (obs, ARG (2), strlen (ARG (2)));
  obstack_grow (obs, "</when>", 7);
  if (argc>3)
    {
      obstack_grow (obs, "<when <not <get-var __cond_var<get-var __cond_cnt /> /> /> >", 60);
      obstack_grow (obs, ARG (3), strlen (ARG (3)));
      obstack_grow (obs, "</when>", 7);
    }
  obstack_grow (obs, "<decrement __cond_cnt />", 24);
}

/*------------------------------------------------------.
| String comparisons.                                   |
| If 2nd and 3rd arguments are equal,  4th argument is  |
| expanded, otherwise 5th argument is expanded.         |
`------------------------------------------------------*/
static void
mp4h_bp_ifeq (MP4H_BUILTIN_ARGS)
{
  symbol *var;

  if (bad_argc (argv[0], argc, 0, 5))
    return;

  var = lookup_variable ("__cond_cnt", SYMBOL_LOOKUP);
  if (var == NULL)
    {
      var = lookup_variable ("__cond_cnt", SYMBOL_INSERT);
      SYMBOL_TEXT (var) = (char *) xstrdup ("0");
      SYMBOL_TYPE (var) = TOKEN_TEXT;
    }
  obstack_grow (obs, "<increment __cond_cnt />", 24);
  obstack_grow (obs, "<set-var __cond_var<get-var __cond_cnt />=<string-eq ", 53);
  obstack_1grow (obs, CHAR_BGROUP);
  obstack_grow (obs, ARG (1), strlen (ARG (1)));
  obstack_1grow (obs, CHAR_EGROUP);
  obstack_1grow (obs, ' ');
  obstack_1grow (obs, CHAR_BGROUP);
  obstack_grow (obs, ARG (2), strlen (ARG (2)));
  obstack_1grow (obs, CHAR_EGROUP);
  obstack_grow (obs, " /> />", 6);
  obstack_grow (obs, "<when <get-var __cond_var<get-var __cond_cnt /> /> >", 52);
  obstack_grow (obs, ARG (3), strlen (ARG (3)));
  obstack_grow (obs, "</when>", 7);
  if (argc>4)
    {
      obstack_grow (obs, "<when <not <get-var __cond_var<get-var __cond_cnt /> /> /> >", 60);
      obstack_grow (obs, ARG (4), strlen (ARG (4)));
      obstack_grow (obs, "</when>", 7);
    }
  obstack_grow (obs, "<decrement __cond_cnt />", 24);
}

/*----------------------------------------------------------.
| String comparisons.                                       |
| If 2nd and 3rd arguments are not equal, 4th argument is   |
| expanded, otherwise 5th argument is expanded.             |
`----------------------------------------------------------*/
static void
mp4h_bp_ifneq (MP4H_BUILTIN_ARGS)
{
  symbol *var;

  if (bad_argc (argv[0], argc, 0, 5))
    return;

  var = lookup_variable ("__cond_cnt", SYMBOL_LOOKUP);
  if (var == NULL)
    {
      var = lookup_variable ("__cond_cnt", SYMBOL_INSERT);
      SYMBOL_TEXT (var) = (char *) xstrdup ("0");
      SYMBOL_TYPE (var) = TOKEN_TEXT;
    }
  obstack_grow (obs, "<increment __cond_cnt />", 24);
  obstack_grow (obs, "<set-var __cond_var<get-var __cond_cnt />=<string-neq ", 54);
  obstack_1grow (obs, CHAR_BGROUP);
  obstack_grow (obs, ARG (1), strlen (ARG (1)));
  obstack_1grow (obs, CHAR_EGROUP);
  obstack_1grow (obs, ' ');
  obstack_1grow (obs, CHAR_BGROUP);
  obstack_grow (obs, ARG (2), strlen (ARG (2)));
  obstack_1grow (obs, CHAR_EGROUP);
  obstack_grow (obs, " /> />", 6);
  obstack_grow (obs, "<when <get-var __cond_var<get-var __cond_cnt /> /> >", 52);
  obstack_grow (obs, ARG (3), strlen (ARG (3)));
  obstack_grow (obs, "</when>", 7);
  if (argc>4)
    {
      obstack_grow (obs, "<when <not <get-var __cond_var<get-var __cond_cnt /> /> /> >", 60);
      obstack_grow (obs, ARG (4), strlen (ARG (4)));
      obstack_grow (obs, "</when>", 7);
    }
  obstack_grow (obs, "<decrement __cond_cnt />", 24);
}

/*----------------------------------------------------------------.
| This one is a complex tag ; its body is evalled only if first   |
| argument is empty.                                              |
`----------------------------------------------------------------*/
static void
mp4h_bp_when (MP4H_BUILTIN_ARGS)
{
  /*
       This test succeeds if one of these 2 clauses is true
          a) there are at least 2 arguments
          b) first argument is non empty.
  */
  if (argc > 2 || strlen (ARG (1)) > 0)
    obstack_grow (obs, ARGBODY, strlen (ARGBODY));
}

/*--------------------------------.
| Loops while condition is true.  |
`--------------------------------*/
static void
mp4h_bp_while (MP4H_BUILTIN_ARGS)
{
  if (bad_argc (argv[0], argc, 0, 2))
    return;
  if (argc == 1)
    return;

  /* ``<while test>expr</while>'' is replaced by
     ``<when test>expr<while test>expr</while></when>''.  */

  if (strlen (ARG (1)) > 0)
    {
      /*  The ``varbreak '' variable is global and modified by <break>.  */
      if (strcmp (SYMBOL_TEXT (&varbreak), "true") == 0)
        {
          xfree ((voidstar) SYMBOL_TEXT (&varbreak));
          SYMBOL_TEXT (&varbreak) = xstrdup ("");
          return;
        }

      obstack_grow (obs, "<when ", 6);
      dump_args (obs, argc, argv, " ");
      obstack_grow (obs, " >", 2);
      obstack_grow (obs, ARGBODY, strlen (ARGBODY));
      obstack_grow (obs, "<while ", 7);
      dump_args (obs, argc, argv, " ");
      obstack_grow (obs, " >", 2);
      obstack_grow (obs, ARGBODY, strlen (ARGBODY));
      obstack_grow (obs, "</while>", 8);
      obstack_grow (obs, "</when>", 7);
    }
}

/*---------------------------------------------------------------------.
| This is a container tag.  First argument is the name of a variable,  |
| second is the name of an array.  Body function is evaluated for each |
| element of this array, this element being put into the variable.     |
`---------------------------------------------------------------------*/
static void
mp4h_bp_foreach (MP4H_BUILTIN_ARGS)
{
  symbol *var;
  char *value;
  const char *start, *end, *step;
  int ind_start, ind_end, ind_step;
  int length;
  int i;

  start  = predefined_attribute ("start", &argc, argv, FALSE);
  end    = predefined_attribute ("end", &argc, argv, FALSE);
  step   = predefined_attribute ("step", &argc, argv, FALSE);

  if (bad_argc (argv[0], argc, 3, 3))
    return;

  var = lookup_variable (ARG (2), SYMBOL_LOOKUP);
  if (var == NULL)
    return;

  if (start)
    numeric_arg (argv[0], start, TRUE, &ind_start);
  else
    ind_start = 0;

  if (end)
    numeric_arg (argv[0], end, TRUE, &ind_end);
  else
    ind_end = array_size (var);

  if (step)
    numeric_arg (argv[0], step, TRUE, &ind_step);
  else
    ind_step = 1;

  if (ind_step > 0)
    {
      for (i=ind_start; i<ind_end; i+=ind_step)
        {
          value = array_value (var, i, &length);
          obstack_grow (obs, "<set-var ", 9);
          obstack_grow (obs, ARG (1), strlen (ARG (1)));
          obstack_1grow (obs, '=');
          obstack_1grow (obs, CHAR_LQUOTE);
          obstack_grow (obs, value, length);
          obstack_1grow (obs, CHAR_RQUOTE);
          obstack_grow (obs, "/>", 2);
          obstack_grow (obs, ARGBODY, strlen (ARGBODY));
        }
    }
  else if (ind_step < 0)
    {
      for (i=ind_end-1; i>=ind_start; i+=ind_step)
        {
          value = array_value (var, i, &length);
          obstack_grow (obs, "<set-var ", 9);
          obstack_grow (obs, ARG (1), strlen (ARG (1)));
          obstack_1grow (obs, '=');
          obstack_1grow (obs, CHAR_LQUOTE);
          obstack_grow (obs, value, length);
          obstack_1grow (obs, CHAR_RQUOTE);
          obstack_grow (obs, "/>", 2);
          obstack_grow (obs, ARGBODY, strlen (ARGBODY));
        }
    }
  else
    {
      MP4HERROR ((warning_status, 0,
        _("Warning:%s:%d: Null step is ignored in <%s> loop"),
             CURRENT_FILE_LINE, ARG (0)));
      return;
    }
}

/*---------------------------------------------------------------.
| This function implements a ``case'' statement.  Its syntax is  |
|   <var-case                                                    |
|         var=value    expression                                |
|           ...............                                      |
|         var=value    expression>                               |
`---------------------------------------------------------------*/
static void
mp4h_bp_var_case (MP4H_BUILTIN_ARGS)
{
  char *cp;
  int i;

  for (i=1; i<argc-1; i+=2)
    {
      cp = strchr (ARG (i), '=');
      if (cp)
        {
          *cp = '\0';
          cp++;
          obstack_grow (obs, "<when <string-eq <get-var ", 26);
          obstack_grow (obs, ARG (i), strlen (ARG (i)));
          obstack_grow (obs, "/> ", 3);
          obstack_1grow (obs, '"');
          obstack_grow (obs, cp, strlen (cp));
          obstack_1grow (obs, '"');
          obstack_grow (obs, "/>>", 3);
          obstack_grow (obs, ARG (i+1), strlen (ARG (i+1)));
          obstack_grow (obs, "</when>", 7);
        }
      else
        {
          MP4HERROR ((warning_status, 0,
            _("Warning:%s:%d: Syntax error in <var-case>"),
                 CURRENT_FILE_LINE));
        }
    }
}

/*------------------------------------.
| Immediately exits from inner loop.  |
`------------------------------------*/
static void
mp4h_bp_break (MP4H_BUILTIN_ARGS)
{
  xfree ((voidstar) SYMBOL_TEXT (&varbreak));
  SYMBOL_TEXT (&varbreak) = xstrdup ("true");
}

/*------------------------------------.
| Immediately exits from inner macro.  |
`------------------------------------*/
static void
mp4h_bp_return (MP4H_BUILTIN_ARGS)
{
  int i, levels=1;
  const char *up;
  struct obstack *st;

  up = predefined_attribute ("up", &argc, argv, FALSE);
  if (up)
    numeric_arg (argv[0], up, TRUE, &levels);

  if (levels < 0)
    levels = expansion_level + 1;
  else if (levels == 0)
    levels = expansion_level;
  else if (levels > expansion_level)
    levels = expansion_level;

  for (i=0; i<levels; i++)
    skip_buffer ();

  if (argc>1)
    {
      st = push_string_init ();
      obstack_grow (st, ARG (1), strlen (ARG (1)));
      push_string_finish (0);
    }
}

/*-----------------------------.
| Writes a message to stderr.  |
`-----------------------------*/
static void
mp4h_bp_warning (MP4H_BUILTIN_ARGS)
{
  MP4HERROR ((warning_status, 0,
      _("Warning:%s:%d: %s"),
           CURRENT_FILE_LINE, ARG (1)));
}

/*--------------------.
| Immediately exits.  |
`--------------------*/
static void
mp4h_bp_exit (MP4H_BUILTIN_ARGS)
{
  const char *status, *message;
  int rc = -1;

  status = predefined_attribute ("status", &argc, argv, FALSE);
  if (status)
    numeric_arg (argv[0], status, FALSE, &rc);
  message = predefined_attribute ("message", &argc, argv, FALSE);
  if (message)
    MP4HERROR ((warning_status, 0, "%s", message));
  exit (rc);
}

/*-------------------------------------------------------------------------.
| Save the argument text until EOF has been seen, allowing for user        |
| specified cleanup action.  GNU version saves all arguments, the standard |
| version only the first.                                                  |
`-------------------------------------------------------------------------*/

static void
mp4h_bp_at_end_of_file (MP4H_BUILTIN_ARGS)
{
  dump_args (obs, 2, argv, " ");
  obstack_1grow (obs, '\0');
  push_wrapup (obstack_finish (obs));
}



/*  Macro functions: defining, undefining, examining or changing
    user defined macros.  */

/*-------------------------------------.
| Define new tags, simple or complex.  |
`-------------------------------------*/

static void
mp4h_bp_define_tag (MP4H_BUILTIN_ARGS)
{
  const builtin *bp;
  const char *attributes, *endtag, *whitespace;
  symbol *sym;
  boolean expand_args = TRUE;
  boolean container = FALSE;
  boolean space_delete = FALSE;

  attributes = predefined_attribute ("attributes", &argc, argv, TRUE);
  if (attributes && strcmp (attributes, "verbatim") == 0)
    expand_args = FALSE;
  endtag = predefined_attribute ("endtag", &argc, argv, TRUE);
  if (endtag && strcmp (endtag, "required") == 0)
    container = TRUE;
  whitespace = predefined_attribute ("whitespace", &argc, argv, TRUE);
  if (whitespace && strcmp (whitespace, "delete") == 0)
    space_delete = TRUE;

  if (bad_argc (argv[0], argc, 2, 3))
    return;

  if (TOKEN_DATA_TYPE (argv[1]) != TOKEN_TEXT)
    return;

  if (argc == 1)
    {
      define_user_macro (ARG (1), "", SYMBOL_INSERT, container,
              expand_args, space_delete);
      return;
    }

  switch (TOKEN_DATA_TYPE (argv[argc]))
    {
    case TOKEN_TEXT:
      define_user_macro (ARG (1), ARGBODY, SYMBOL_INSERT, container,
              expand_args, space_delete);
      break;

    case TOKEN_FUNC:
      bp = find_builtin_by_addr (TOKEN_DATA_FUNC (argv[argc]));
      if (bp == NULL)
        return;
      else
        define_builtin (ARG (1), bp, TOKEN_DATA_FUNC_TRACED (argv[argc]));
      break;

    default:
      MP4HERROR ((warning_status, 0,
        "INTERNAL ERROR: Bad token data type in mp4h_bp_define_tag ()"));
      exit (1);
    }

  /*  Clear hooks  */
  sym = lookup_symbol (ARG (1), SYMBOL_LOOKUP);
  if (!sym)
    return;

  xfree ((voidstar) SYMBOL_HOOK_BEGIN (sym));
  xfree ((voidstar) SYMBOL_HOOK_END (sym));
}

/*-------------------------------------.
| Define tags if not already defined   |
`-------------------------------------*/

static void
mp4h_bp_provide_tag (MP4H_BUILTIN_ARGS)
{
  symbol *sym;

  sym = lookup_symbol (ARG (1), SYMBOL_LOOKUP);
  if (sym == NULL)
    mp4h_bp_define_tag (MP4H_BUILTIN_RECUR);
}

/*-------------------.
| Define a synonym.  |
`-------------------*/
static void
mp4h_bp_let (MP4H_BUILTIN_ARGS)
{
  const builtin *bp;
  symbol *s;
  char *cp;
  int i;

  for (i = 1; i < argc; i++)
    {
      cp = strchr (ARG (i), '=');
      if (cp)
        {
          *cp = '\0';
          cp++;
          s = lookup_symbol (cp, SYMBOL_LOOKUP);
          if (s == NULL)
            {
              MP4HERROR ((warning_status, 0,
                _("Warning:%s:%d: Macro `%s' not defined in <%s>"),
                     CURRENT_FILE_LINE, cp, ARG (0)));
              continue;
            }

          switch (SYMBOL_TYPE (s))
            {
            case TOKEN_FUNC:
              bp = find_builtin_by_addr (SYMBOL_FUNC (s));
              if (bp)
                define_builtin (ARG (i), bp, SYMBOL_TRACED (s));
              break;
            case TOKEN_TEXT:
              define_user_macro (ARG (i), SYMBOL_TEXT (s), SYMBOL_INSERT,
                    SYMBOL_CONTAINER (s), SYMBOL_EXPAND_ARGS (s), FALSE);
              break;

            default:
              MP4HERROR ((warning_status, 0,
                "INTERNAL ERROR: Bad token data type in mp4h_bp_let ()"));
              exit (1);
            }
        }
      else
        {
          MP4HERROR ((warning_status, 0,
            _("Warning:%s:%d: Unknown syntax `%s' in <%s>"),
                 CURRENT_FILE_LINE, ARG (i), ARG (0)));
        }
    }
}

/*-------------------.
| Undefine symbols.  |
`-------------------*/
static void
mp4h_bp_undef (MP4H_BUILTIN_ARGS)
{
  register int i;

  if (bad_argc (argv[0], argc, 2, 0))
    return;

  for (i = 1; i < argc; i++)
    {
      lookup_symbol (ARG (i), SYMBOL_DELETE);
    }
}

static void
generic_set_hook (MP4H_BUILTIN_ARGS, boolean before, int action)
{
  symbol *sym;
  char *text;
  char *hook;

  sym = lookup_symbol (ARG (1), SYMBOL_LOOKUP);
  if (sym == NULL)
    return;

  if (before)
    hook = SYMBOL_HOOK_BEGIN (sym);
  else
    hook = SYMBOL_HOOK_END (sym);
  
  if (!hook)
    action = 0;

  switch (action)
    {
      /*  Replace current hooks */
      case 0:
        text = xstrdup (ARGBODY);
        break;

      /*  Insert before the current hooks */
      case 1:
        text = (char *) xmalloc (strlen (hook) + strlen (ARGBODY) + 1);
        strcpy (text, ARGBODY);
        strcat (text, hook);
        break;

      /*  Append after the current hooks */
      case 2:
        text = (char *) xmalloc (strlen (hook) + strlen (ARGBODY) + 1);
        strcpy (text, hook);
        strcat (text, ARGBODY);
        break;

      default:
        MP4HERROR ((warning_status, 0,
          "INTERNAL ERROR: Illegal value in generic_set_hook ()"));
        exit (1);
    }

  xfree ((voidstar) hook);
  if (before)
    SYMBOL_HOOK_BEGIN (sym) = text;
  else
    SYMBOL_HOOK_END (sym) = text;
}

static void
mp4h_bp_set_hook (MP4H_BUILTIN_ARGS)
{
  const char *action, *position;
  symbol *sym;
  boolean before = TRUE;
  int iaction = 0;

  action = predefined_attribute ("action", &argc, argv, TRUE);
  position = predefined_attribute ("position", &argc, argv, TRUE);
  if (bad_argc (argv[0], argc, 2, 2))
    return;
  sym = lookup_symbol (ARG (1), SYMBOL_LOOKUP);
  if (sym == NULL)
    return;

  before = !(position && strcmp (position, "after") == 0);
  if (action)
    {
      if (strcmp (action, "insert") == 0)
        iaction = 1;
      else if (strcmp (action, "append") == 0)
        iaction = 2;
      else
        iaction = 0;
    }
  else
    iaction = 0;
  generic_set_hook (MP4H_BUILTIN_RECUR, before, iaction);
}

/*-----------------.
| Retrieve hooks.  |
`-----------------*/
static void
mp4h_bp_get_hook (MP4H_BUILTIN_ARGS)
{
  symbol *sym;
  const char *position;
  char *hook;

  position = predefined_attribute ("position", &argc, argv, TRUE);
  if (bad_argc (argv[0], argc, 2, 2))
    return;

  sym = lookup_symbol (ARG (1), SYMBOL_LOOKUP);
  if (sym == NULL)
    return;

  if (position && strcmp (position, "after") == 0)
    hook = SYMBOL_HOOK_END (sym);
  else
    hook = SYMBOL_HOOK_BEGIN (sym);

  if (hook)
    obstack_grow (obs, hook, strlen (hook));
}

/*----------------------------------------------------------.
| These functions allow any manipulations of attributes.    |
| There is no need to define similar routines for           |
| %Aattributes and %Uattributes, because they can be        |
| emulated via mp4h builtins.                               |
`----------------------------------------------------------*/
static void
mp4h_bp_attributes_quote (MP4H_BUILTIN_ARGS)
{
  int i;

  if (argc < 2)
    return;

  for (i = 1; i < argc; i++)
    {
      if (*(ARG (i)) != '\0')
        {
          obstack_1grow (obs, ' ');
          quote_name_value (obs, ARG (i));
        }
    }
}

static void
mp4h_bp_attributes_remove (MP4H_BUILTIN_ARGS)
{
  if (bad_argc (argv[0], argc, 2, 0))
    return;

  /*  Dirty hack to prevent aggregation of attributes into
      a single argument.  When a group is begun in expand_macro (),
      we finish it here.  */
  if (expansion_level > 1 && (
        expansion == READ_NORMAL || expansion == READ_ATTRIBUTE
        || expansion == READ_ATTR_QUOT))
      obstack_1grow (obs, CHAR_EGROUP);

  matching_attributes (obs, argc-1, argv+1, FALSE, ARG (1));

  if (expansion_level > 1 && (
        expansion == READ_NORMAL || expansion == READ_ATTRIBUTE
        || expansion == READ_ATTR_QUOT))
      obstack_1grow (obs, CHAR_BGROUP);
}

static void
mp4h_bp_attributes_extract (MP4H_BUILTIN_ARGS)
{
  if (bad_argc (argv[0], argc, 2, 0))
    return;

  /*  Dirty hack to prevent aggregation of attributes into
      a single argument.  When a group is begun in expand_macro (),
      we finish it here.  */
  if (expansion_level > 1 && (
        expansion == READ_NORMAL || expansion == READ_ATTRIBUTE
        || expansion == READ_ATTR_QUOT))
      obstack_1grow (obs, CHAR_EGROUP);

  matching_attributes (obs, argc-1, argv+1, TRUE, ARG (1));

  if (expansion_level > 1 && (
        expansion == READ_NORMAL || expansion == READ_ATTRIBUTE
        || expansion == READ_ATTR_QUOT))
      obstack_1grow (obs, CHAR_BGROUP);
}

/*----------------------.
| Define new entities.  |
`----------------------*/

static void
mp4h_bp_define_entity (MP4H_BUILTIN_ARGS)
{
  symbol *sym;

  if (bad_argc (argv[0], argc, 2, 2))
    return;

  if (TOKEN_DATA_TYPE (argv[1]) != TOKEN_TEXT)
    return;

  if (argc == 1)
    define_user_entity (ARG (1), "", SYMBOL_INSERT);
  else
    define_user_entity (ARG (1), ARGBODY, SYMBOL_INSERT);

  /*  Clear hooks  */
  sym = lookup_entity (ARG (1), SYMBOL_LOOKUP);
  if (!sym)
    return;

  xfree ((voidstar) SYMBOL_HOOK_BEGIN (sym));
  xfree ((voidstar) SYMBOL_HOOK_END (sym));
}



/*  Math functions  */

/*--------------------------------------------------.
| This function is called by relational operators.  |
| These operators are binary operators, operands    |
| are either numbers or variable names.             |
`--------------------------------------------------*/
static void
math_relation (MP4H_BUILTIN_ARGS, mathrel_type mathrel)
{
  boolean result;
  symbol *var;
  double val1, val2;

  if (bad_argc (argv[0], argc, 2, 3))
    return;
  
  if (isdigit ((int) ARG (1)[0]) || *(ARG (1)) == '-' || *(ARG (1)) == '+' ||
          strncmp (ARG (1), decimal_point, strlen (decimal_point)))
    {
      if (!safe_strtod (ARG (0), ARG (1), &val1))
        return;
    }
  else
    {
      var = lookup_variable (ARG (1), SYMBOL_LOOKUP);
      if (var == NULL)
        {
          MP4HERROR ((warning_status, 0,
            _("Warning:%s:%d: Argument `%s' non-numeric in <%s>"),
                 CURRENT_FILE_LINE, ARG (1), ARG (0)));
          return;
        }
      if (!safe_strtod (ARG (0), SYMBOL_TEXT (var), &val1))
        return;
    }
  
  if (argc == 2)
    val2 = 0.;
  else if (isdigit ((int) ARG (2)[0]) ||
          *(ARG (2)) == '-' || *(ARG (2)) == '+' ||
          strncmp (ARG (2), decimal_point, strlen (decimal_point)))
    {
      if (!safe_strtod (ARG (0), ARG (2), &val2))
        return;
    }
  else
    {
      var = lookup_variable (ARG (2), SYMBOL_LOOKUP);
      if (var == NULL)
        {
          MP4HERROR ((warning_status, 0,
            _("Warning:%s:%d: Argument `%s' non-numeric in <%s>"),
                 CURRENT_FILE_LINE, ARG (2), ARG (0)));
          return;
        }
      if (!safe_strtod (ARG (0), SYMBOL_TEXT (var), &val2))
        return;
    }

  switch (mathrel)
    {
      case MATHREL_EQ:
        result = (val1 == val2 ? TRUE : FALSE);
        break;

      case MATHREL_NEQ:
        result = (val1 != val2 ? TRUE : FALSE);
        break;

      case MATHREL_GT:
        result = (val1 > val2 ? TRUE : FALSE);
        break;

      case MATHREL_LT:
        result = (val1 < val2 ? TRUE : FALSE);
        break;
        
      default:
        MP4HERROR ((warning_status, 0,
          "INTERNAL ERROR: Illegal operator in math_relation ()"));
        exit (1);
    }

  if (result)
    obstack_grow (obs, "true", 4);
}

static void
mp4h_bp_gt (MP4H_BUILTIN_ARGS)
{
  math_relation (MP4H_BUILTIN_RECUR, MATHREL_GT);
}

static void
mp4h_bp_lt (MP4H_BUILTIN_ARGS)
{
  math_relation (MP4H_BUILTIN_RECUR, MATHREL_LT);
}

static void
mp4h_bp_eq (MP4H_BUILTIN_ARGS)
{
  math_relation (MP4H_BUILTIN_RECUR, MATHREL_EQ);
}

static void
mp4h_bp_neq (MP4H_BUILTIN_ARGS)
{
  math_relation (MP4H_BUILTIN_RECUR, MATHREL_NEQ);
}

/*------------------------------------------------------.
| This definition is used to simplify the writings of   |
| arithmetic operators, an operation is performed on    |
| every arguments.                                      |
| If all arguments are integer values, round-offs must  |
| be performed at each step.                            |
`------------------------------------------------------*/
#define MATH_ARG_LOOP(ops) for (i=2; i<argc; i++) \
    {if (safe_strtod (ARG (0), ARG (i), &val)) ops;\
     if (result_int) result = floor (result);}

static void
mathop_functions (MP4H_BUILTIN_ARGS, mathop_type mathop)
{
  double val, result;
  int i;
  char svalue[128];
  char *pos_radix;
  char sformat[32];
  boolean result_int = TRUE;

  if (bad_argc (argv[0], argc, 3, 0))
    return;
  
  /*  If all operands are integers, an integer must be returned.  */
  for (i=1; i<argc; i++)
    if (strstr (ARG (i), decimal_point) != NULL)
      result_int = FALSE;

  if (mathop == MATHOP_MOD)
    {
      /* Modulus is a special case since operands must be integers,
         and having more than 2 operands is illegal.  */

      if (bad_argc (argv[0], argc, 3, 3))
        return;
      if (!result_int)
        {
          MP4HERROR ((warning_status, 0,
            _("Warning:%s:%d: Mathop `mod' has non-integer operands"),
                 CURRENT_FILE_LINE));
        }
      else
        {
          int val1, val2;

          val1 = strtol (ARG (1), (char **)NULL, 10);
          val2 = strtol (ARG (2), (char **)NULL, 10);
          val1 %= val2;
          shipout_int (obs, val1);
        }
      return;
    }

  /*  Initialization.  */
  if (!safe_strtod (ARG (0), ARG (1), &result))
    result = 0.;

  /*  Loop on operands.  */
  switch (mathop)
    {
    case MATHOP_ADD:
      MATH_ARG_LOOP(result += val)
      break;
    case MATHOP_SUB:
      MATH_ARG_LOOP(result -= val)
      break;
    case MATHOP_MUL:
      MATH_ARG_LOOP(result *= val)
      break;
    case MATHOP_DIV:
      MATH_ARG_LOOP(result /= val)
      break;
    case MATHOP_MIN:
      MATH_ARG_LOOP(if (val<result) result=val)
      break;
    case MATHOP_MAX:
      MATH_ARG_LOOP(if (val>result) result=val)
      break;
    default:
        MP4HERROR ((warning_status, 0,
          "INTERNAL ERROR: Illegal mathop in mp4h_bp_mathop ()"));
    }

  sprintf (sformat, "%%.%df", output_radix);
  sprintf (svalue, sformat, result);
  if (result_int)
    {
      pos_radix = strstr (svalue, decimal_point);
      if (pos_radix)
        *pos_radix = '\0';
    }

  shipout_text (obs, svalue);
}
#undef MATH_ARG_LOOP

static void
mp4h_bp_add (MP4H_BUILTIN_ARGS)
{
  mathop_functions (MP4H_BUILTIN_RECUR, MATHOP_ADD);
}

static void
mp4h_bp_substract (MP4H_BUILTIN_ARGS)
{
  mathop_functions (MP4H_BUILTIN_RECUR, MATHOP_SUB);
}

static void
mp4h_bp_multiply (MP4H_BUILTIN_ARGS)
{
  mathop_functions (MP4H_BUILTIN_RECUR, MATHOP_MUL);
}

static void
mp4h_bp_divide (MP4H_BUILTIN_ARGS)
{
  mathop_functions (MP4H_BUILTIN_RECUR, MATHOP_DIV);
}

static void
mp4h_bp_modulo (MP4H_BUILTIN_ARGS)
{
  mathop_functions (MP4H_BUILTIN_RECUR, MATHOP_MOD);
}

static void
mp4h_bp_min (MP4H_BUILTIN_ARGS)
{
  mathop_functions (MP4H_BUILTIN_RECUR, MATHOP_MIN);
}

static void
mp4h_bp_max (MP4H_BUILTIN_ARGS)
{
  mathop_functions (MP4H_BUILTIN_RECUR, MATHOP_MAX);
}


/*  Page functions  */
/*------------------------------------------------------.
| Read and parse a file.  This file is searched in the  |
| directories specified by the -I option.               |
| Because the `alt' attribute must be read unexpanded,  |
| this tag calls <__include> whose arguments can be     |
| expanded since `alt' has been passed into this tag    |
| body.                                                 |
`------------------------------------------------------*/
static void
mp4h_bp_include (MP4H_BUILTIN_ARGS)
{
  const char *alt, *verbatim;

  CHECK_SAFETY_LEVEL(1);

  alt = predefined_attribute ("alt", &argc, argv, FALSE);
  verbatim = predefined_attribute ("verbatim", &argc, argv, TRUE);

  obstack_grow (obs, "<__include ", 11);
  dump_args (obs, argc, argv, " ");
  if (verbatim && strcmp (verbatim, "true") == 0)
    obstack_grow (obs, " verbatim=true", 14);
  obstack_grow (obs, " >", 2);
  if (alt)
    obstack_grow (obs, alt, strlen (alt));
  obstack_grow (obs, "</__include>", 12);
}

static void
mp4h_bp___include (MP4H_BUILTIN_ARGS)
{
  const char *verbatim, *command, *file;
  FILE *fp;
  char *filename = NULL;

  CHECK_SAFETY_LEVEL(1);

  verbatim = predefined_attribute ("verbatim", &argc, argv, TRUE);
  file = predefined_attribute ("file", &argc, argv, FALSE);
  command = predefined_attribute ("command", &argc, argv, FALSE);

  if (file && command)
    {
      command = NULL;
      MP4HERROR ((warning_status, 0, _("\
Warning:%s:%d: `file' and `command' attributes in <include> tag are mutually exclusive, using `file'"),
                CURRENT_FILE_LINE));
    }

  if (command)
    {
      /*  Reads input from command output  */

      CHECK_SAFETY_LEVEL(0);

      filename = xstrdup(command);
      remove_special_chars (filename, TRUE);
      fp = popen(filename, "r");
      if (fp == NULL)
        {
          MP4HERROR ((warning_status, errno,
            _("Warning:%s:%d: Cannot execute %s"),
                 CURRENT_FILE_LINE, filename));
          return;
        }
    }
  else
    {
      if (file)
        {
          /*  New syntax  */
          if (bad_argc (argv[0], argc, 1, 1))
            return;
        }
      else
        {
          /*  Old syntax  */
          if (bad_argc (argv[0], argc, 2, 2))
            return;
          file = ARG(1);
        }
      if (!file)
        {
          MP4HERROR ((warning_status, 0, _("\
Error:%s:%d: <include> must be invoked with one of the `file' and `command' attributes; skipped"),
                    CURRENT_FILE_LINE));
          return;
        }

      fp = path_search (file, &filename);
      if (fp == NULL)
        {
          if (*(ARGBODY) != '\0')
            shipout_string (obs, ARGBODY, 0);
          else
            {
              MP4HERROR ((warning_status, errno,
                _("Warning:%s:%d: Cannot open %s"),
                     CURRENT_FILE_LINE, file));
            }
          return;
        }
    }

  push_file (fp, filename);

  if (verbatim && strcmp (verbatim, "true") == 0)
    read_file_verbatim (obs);

  xfree ((voidstar) filename);
}

/*-------------------------------------------------.
| Transform a module or library name foo:bar into  |
| physical path `foo/bar'.                         |
`-------------------------------------------------*/
static void
logical_to_physical_paths (char **name)
{
  register char *cp;

  for (cp=*name; *cp != '\0'; cp++)
    {
      if (*cp == ':')
        *cp = '/';
    }
}

/*-------------------------------------------------.
| Read and parse a package if not already loaded.  |
`-------------------------------------------------*/
static void
mp4h_bp_use (MP4H_BUILTIN_ARGS)
{
  FILE *fp;
  const char *name = NULL;
  char *filename = NULL, *real_filename = NULL;
  symbol *sym;

  if (bad_argc (argv[0], argc, 2, 2))
    return;

  name = predefined_attribute ("name", &argc, argv, FALSE);
  if (name == NULL)
    {
      MP4HERROR ((warning_status, 0,
        _("Error:%s:%d: In <%s>, required attribute `%s' is not specified"),
             CURRENT_FILE_LINE, ARG (0), "name"));
      return;
    }

  sym = lookup_file (name, SYMBOL_LOOKUP);
  if (sym)
    {
      if (debug_level & DEBUG_TRACE_MODULES)
	DEBUG_MESSAGE1("module `%s' handle already seen", name);
      return;
    }

  real_filename = xmalloc (strlen (name) + 7);
  sprintf (real_filename, "%s.mp4hp", name);
  logical_to_physical_paths (&real_filename);

  fp = path_search (real_filename, &filename);
  if (fp == NULL)
    {
      MP4HERROR ((warning_status, errno,
        _("Warning:%s:%d: Cannot open %s"),
             CURRENT_FILE_LINE, name));
      xfree ((voidstar) real_filename);
      return;
    }
  else
    if (debug_level & DEBUG_TRACE_MODULES)
      DEBUG_MESSAGE2("module `%s': loaded ok (=%s)", name, filename);

  lookup_file (name, SYMBOL_INSERT);

  push_file (fp, filename);

  xfree ((voidstar) filename);
}

/*-------------------------------------.
| Loading external module at runtime.  |
`-------------------------------------*/
#ifdef WITH_MODULES

static void
mp4h_bp_load (MP4H_BUILTIN_ARGS)
{
  const char *module, *library;
  char *realname = NULL;
  library = predefined_attribute ("library", &argc, argv, FALSE);
  module = predefined_attribute ("module", &argc, argv, FALSE);
  if (!library && !module)
    {
      MP4HERROR ((warning_status, 0,
        _("Error:%s:%d: In <%s>, at least one of '%s' and '%s' attributes must be specified"),
           CURRENT_FILE_LINE, ARG (0), "module", "library"));
      return;
    }
  if (library)
    {
      realname = xstrdup (library);
      logical_to_physical_paths (&realname);
      library_load (realname, obs);
      xfree ((voidstar) realname);
    }
  if (module)
    {
      realname = xstrdup (module);
      logical_to_physical_paths (&realname);
      module_load (realname, obs);
      xfree ((voidstar) realname);
    }
}

#endif

/*-----------------------------.
| Discards its body contents.  |
`-----------------------------*/
static void
mp4h_bp_comment (MP4H_BUILTIN_ARGS)
{
}

/*----------------------------.
| Set EOL comment delimiter.  |
`----------------------------*/
static void
mp4h_bp_set_eol_comment (MP4H_BUILTIN_ARGS)
{
  bad_argc (argv[0], argc, 0, 2);
  xfree ((voidstar) eolcomm.string);
  eolcomm.string = xstrdup (ARG (1));
  eolcomm.length = strlen (eolcomm.string);
}

/*--------------------------------.
| Set quotes to disable parsing.  |
`--------------------------------*/
static void
mp4h_bp_set_quotes (MP4H_BUILTIN_ARGS)
{
  const char *display;

  display = predefined_attribute ("display", &argc, argv, TRUE);
  visible_quotes = (display && strcmp (display, "visible") == 0);

  if (argc == 1)
    {
      xfree ((voidstar) lquote.string);
      xfree ((voidstar) rquote.string);
      lquote.string = NULL;
      lquote.length = 0;
      rquote.string = NULL;
      rquote.length = 0;
    }
  else if (argc == 3)
    {
      if (*ARG (1) != '<' || LAST_CHAR (ARG (2)) != '>')
        {
          MP4HERROR ((warning_status, 0,
            _("Warning:%s:%d: <%s> ignored, invalid arguments: %s %s"),
                 CURRENT_FILE_LINE, ARG (0), ARG (1), ARG (2)));
          return;
        }
      xfree ((voidstar) lquote.string);
      xfree ((voidstar) rquote.string);
      lquote.string = xstrdup (ARG (1));
      lquote.length = strlen (lquote.string);

      rquote.string = xstrdup (ARG (2));
      rquote.length = strlen (rquote.string);
    }
  else
    MP4HERROR ((warning_status, 0,
      _("Warning:%s:%d: `<%s>' must have 0 or 2 arguments, but got %d"),
           CURRENT_FILE_LINE, ARG (0), argc-1));
}

/*------------------------------------------------------------------------.
| Delete all subsequent whitespace from input.  The function skip_line () |
| lives in input.c.                                                       |
`------------------------------------------------------------------------*/

static void
mp4h_bp_dnl (MP4H_BUILTIN_ARGS)
{
  if (bad_argc (argv[0], argc, 1, 1))
    return;

  skip_line ();
}

static void
mp4h_bp_frozen_dump (MP4H_BUILTIN_ARGS)
{
  if (frozen_dump)
    input_close ();
}


/*  Relational operators : arguments are strings.  */

/*----------------------------------------------------------------.
| If first argument is an empty string, returns "true" otherwise  |
| returns nothing.                                                |
`----------------------------------------------------------------*/
static void
mp4h_bp_not (MP4H_BUILTIN_ARGS)
{
  int i;

  if (argc < 2)
    {
      obstack_grow (obs, "true", 4);
      return;
    }

  for (i=1; i<argc; i++)
    {
      remove_special_chars (ARG (i), FALSE);
      if (*(ARG (i)) == '\0')
        {
          obstack_grow (obs, "true", 4);
          return;
        }
    }
}

/*-----------------------------------------------------------.
| When at least one argument is empty, nothing is returned.  |
| Otherwise, the last string is returned.                    |
`-----------------------------------------------------------*/
static void
mp4h_bp_and (MP4H_BUILTIN_ARGS)
{
  int i;
  char *cp;

  if (argc == 1)
    return;

  for (i=1; i<argc; i++)
    {
      for (cp=ARG (i); IS_GROUP (*cp); cp++ )
        ;

      if (*cp == '\0')
        break;
    }
  
  if (i == argc)
    obstack_grow (obs, ARG (argc-1), strlen (ARG (argc-1)));
}

/*--------------------------------------------.
| "or" returns the first non-empty argument.  |
`--------------------------------------------*/
static void
mp4h_bp_or (MP4H_BUILTIN_ARGS)
{
  int i;
  char *cp;

  for (i=1; i<argc; i++)
    {
      for (cp=ARG (i); IS_GROUP (*cp); cp++ )
        ;

      if (*cp != '\0')
        {
          obstack_grow (obs, ARG (i), strlen (ARG (i)));
          return;
        }
    }
}


/*  String functions  */

/*-----------------------------------.
| Returns the length of the string.  |
`-----------------------------------*/
static void
mp4h_bp_string_length (MP4H_BUILTIN_ARGS)
{
  if (bad_argc (argv[0], argc, 0, 2))
    return;

  if (argc == 1)
    shipout_int (obs, 0);
  else
    shipout_int (obs, encoding_strlen (ARG (1)));
}

/*---------------------------------------------------------------.
| This routine converts its argument to uppercase or lowercase   |
| letters, depending on the last argument.                       |
`---------------------------------------------------------------*/
static void
updowncase (struct obstack *obs, int argc, token_data **argv, boolean upcase)
{
  char *text;
  char *cp;
  int i;

  if (argc == 1)
    return;

  for (i=1; i<argc; i++)
    {
      if (i > 1)
        obstack_1grow (obs, ' ');

      text = xstrdup (ARG (i));
      if (upcase)
        {
          for (cp = text; *cp != '\0'; cp++)
            *cp = toupper (*cp);
        }
      else
        {
          for (cp = text; *cp != '\0'; cp++)
            *cp = tolower (*cp);
        }
      obstack_grow (obs, text, strlen (text));
      xfree ((voidstar) text);
    }
}

static void
mp4h_bp_downcase (MP4H_BUILTIN_ARGS)
{
  updowncase (obs, argc, argv, FALSE);
}

static void
mp4h_bp_upcase (MP4H_BUILTIN_ARGS)
{
  updowncase (obs, argc, argv, TRUE);
}

static void
mp4h_bp_capitalize (MP4H_BUILTIN_ARGS)
{
  char *text;
  register char *cp;
  register int i;
  boolean next;

  if (argc == 1)
    return;

  for (i=1; i<argc; i++)
    {
      if (i > 1)
        obstack_1grow (obs, ' ');

      next = TRUE;
      text = xstrdup (ARG (i));
      for (cp = text; *cp != '\0'; cp++)
        {
          if (next)
            *cp = toupper (*cp);
          if (isspace (*cp))
            next = TRUE;
          else
            next = FALSE;
        }
      obstack_grow (obs, text, strlen (text));
      xfree ((voidstar) text);
    }
}


/*---------------------------------------------------------------.
| Extracts some portion of a string.  Optional attributes are    |
| start and end position.  Zero is the position of the first     |
| character.                                                     |
`---------------------------------------------------------------*/
static void
mp4h_bp_substring (MP4H_BUILTIN_ARGS)
{
  char *text, *cp;
  int start, end;

  if (bad_argc (argv[0], argc, 3, 4))
    return;

  text = ARG (1);
  if (argc>3)
    {
      if (!numeric_arg (argv[0], ARG (3), TRUE, &end))
        return;
      if (document_encoding == ENCODING_UTF8)
        {
          cp = utf8char_skip(text, end);
          if (!cp)
            return;
          *cp = '\0';
        }
      else
        *(text+end) = '\0';
    }
  if (argc>2)
    {
      if (!numeric_arg (argv[0], ARG (2), TRUE, &start))
        return;
      if (document_encoding == ENCODING_UTF8)
        {
          text = utf8char_skip(text, start);
          if (!text)
            return;
        }
      else
        text += start;
    }
  obstack_grow (obs, text, strlen (text));
}

/*----------------------------------------------------------------.
| Compares strings.  When caseless=true is specified, comparison  |
| is performed without consideration of case.                     |
`----------------------------------------------------------------*/
static void
mp4h_bp_string_eq (MP4H_BUILTIN_ARGS)
{
  const char *caseless;

  caseless = predefined_attribute ("caseless", &argc, argv, TRUE);
  if (bad_argc (argv[0], argc, 0, 3))
    return;

  if (argc < 3)
    {
      if (*(ARG (1)) == '\0')
        obstack_grow (obs, "true", 4);
      return;
    }
  if (caseless && strcmp (caseless, "true") == 0)
    {
      if (strcasecmp (ARG (1), ARG (2)) == 0)
        obstack_grow (obs, "true", 4);
    }
  else
    {
      if (strcmp (ARG (1), ARG (2)) == 0)
        obstack_grow (obs, "true", 4);
    }
}

static void
mp4h_bp_string_neq (MP4H_BUILTIN_ARGS)
{
  const char *caseless;

  caseless = predefined_attribute ("caseless", &argc, argv, TRUE);
  if (bad_argc (argv[0], argc, 0, 3))
    return;

  if (argc < 3)
    {
      if (*(ARG (1)) != '\0')
        obstack_grow (obs, "true", 4);
      return;
    }
  if (caseless && strcmp (caseless, "true") == 0)
    {
      if (strcasecmp (ARG (1), ARG (2)) != 0)
        obstack_grow (obs, "true", 4);
    }
  else
    {
      if (strcmp (ARG (1), ARG (2)) != 0)
        obstack_grow (obs, "true", 4);
    }
}

/*-----------------------------------------------------------------.
| This function compares two strings and returns this comparison.  |
| If ``caseless=true'' is specified, this comparison is performed  |
| without case.                                                    |
`-----------------------------------------------------------------*/
static void
mp4h_bp_string_compare (MP4H_BUILTIN_ARGS)
{
  const char *caseless;
  int result;

  caseless = predefined_attribute ("caseless", &argc, argv, TRUE);
  if (bad_argc (argv[0], argc, 2, 3))
    return;

  if (caseless && strcmp (caseless, "true") == 0)
    result = strcasecmp (ARG (1), ARG (2));
  else
    result = strcmp (ARG (1), ARG (2));

  if (result < 0)
    obstack_grow (obs, "less", 4);
  else if (result > 0)
    obstack_grow (obs, "greater", 7);
  else
    obstack_grow (obs, "equal", 5);
}

/*----------------------------------------------------------------.
| First argument is a string, 2nd is a character.                 |
| This function returns an array of numbers, which are locations  |
| where character appear in the string.                           |
`----------------------------------------------------------------*/
static void
mp4h_bp_char_offsets (MP4H_BUILTIN_ARGS)
{
  const char *caseless;
  char *cp;
  char c;
  boolean first = TRUE;

  caseless = predefined_attribute ("caseless", &argc, argv, TRUE);
  if (bad_argc (argv[0], argc, 2, 3))
    return;

  if (strlen (ARG (2)) > 1)
    {
      MP4HERROR ((warning_status, 0,
        _("Warning:%s:%d: Second argument of <%s> is not a char"),
             CURRENT_FILE_LINE, ARG (0)));
      return;
    }
  c = ARG (2)[0];
  if (caseless && strcmp (caseless, "true") == 0)
    {
      for (cp = ARG (1); *cp != '\0'; cp++)
        {
          if (tolower(*cp) == tolower(c))
            {
              if (!first)
                obstack_1grow (obs, '\n');
              else
                first = FALSE;
              shipout_int (obs, (int) (cp-ARG (1)));
            }
        }
    }
  else
    {
      for (cp = ARG (1); *cp != '\0'; cp++)
        {
          if (*cp == c)
            {
              if (!first)
                obstack_1grow (obs, '\n');
              else
                first = FALSE;
              shipout_int (obs, (int) (cp-ARG (1)));
            }
        }
    }
}

/*----------------------------.
| Implement printf function.  |
`----------------------------*/
static void
mp4h_bp_printf (MP4H_BUILTIN_ARGS)
{
  const char *text, *save;
  int i, len, count_arg;

  if (bad_argc (argv[0], argc, 2, 0))
    return;

  count_arg = 1;
  for (text = ARG(1); *text != '\0';)
    {
      if (*text != '%')
        {
          save = text;
          len = 1;
          for (text++ ; *text != '%' && *text != '\0'; text++)
            len++;
          obstack_grow (obs, save, len);
          if (*text == '\0')
            break;
        }
      text++;
      if (*text == 's')
        {
          count_arg++;
          obstack_grow (obs, ARG(count_arg), strlen(ARG(count_arg)));
          text++;
        }
      else if (*text == '%')
        {
          obstack_1grow (obs, '%');
          text++;
        }
      else if (isdigit ((int) *text))
        {
          char *endp;
          i = (int)strtol (text, &endp, 10) + 1;
          text = endp;
          if (NULL == text || *text != '$')
            {
              obstack_1grow (obs, '%');
              shipout_int (obs, i - 1);
              continue;
            }
          text++;
          if (*text == 's')
            {
              obstack_grow (obs, ARG(i), strlen(ARG(i)));
              text ++;
            }
          else if (*text == '%')
            {
              obstack_1grow (obs, '%');
              text++;
            }
          else
            {
              obstack_1grow (obs, '%');
              shipout_int (obs, i - 1);
            }
        }
      else
        obstack_1grow (obs, '%');
    }
}


/*
     Regular expression support is provided by the PCRE library package,
     which is open source software, copyright by the University of
     Cambridge.
     Latest sources are available from
        ftp://ftp.csx.cam.ac.uk/pub/software/programming/pcre/
*/

static char *
utf8char_skip (char *utf, int offset)
{
  if (!utf)
    return(NULL);
  while (offset > 0) {
    offset--;
    if (utf[0] & 0x80) {
      if ((utf[1] & 0xc0) != 0x80)
        return(NULL);
      if ((utf[0] & 0xe0) == 0xe0) {
        if ((utf[2] & 0xc0) != 0x80)
          return(NULL);
        if ((utf[0] & 0xf0) == 0xf0) {
          if ((utf[0] & 0xf8) != 0xf0 || (utf[3] & 0xc0) != 0x80)
            return(NULL);
          utf += 4;
        } else {
          utf += 3;
        }
      } else {
        utf += 2;
      }
    } else {
      utf++;
    }
  }
  return(utf);
}

static int
utf8char_strlen(char *utf)
{
  int ret = 0;

  if (utf == NULL)
    return(-1);

  while (*utf != 0) {
    utf = utf8char_skip(utf, 1);
    if (!utf)
      return(-1);
    ret++;
  }
  return(ret);
}

static int
encoding_strlen(char *str)
{
  if (document_encoding == ENCODING_UTF8)
    return(utf8char_strlen (str));
  else
    return(strlen (str));
}

/*-------------------------------------------------------------------------.
| Function to perform substitution by regular expressions.  Used by the    |
| builtins regexp and patsubst.  The changed text is placed on the         |
| obstack.  The substitution is REPL, with \& substituted by this part of  |
| VICTIM matched by the last whole regular expression, taken from REGS[0], |
| and \N substituted by the text matched by the Nth parenthesized          |
| sub-expression, taken from REGS[N].                                      |
`-------------------------------------------------------------------------*/

static int substitute_warned = 0;

static void
substitute (struct obstack *obs, const char *victim, const char *repl,
            int *regs)
{
  register unsigned int ch;

  for (;;)
    {
      while ((ch = *repl++) != '\\')
        {
          if (ch == '\0')
            return;
          obstack_1grow (obs, ch);
        }

      switch ((ch = *repl++))
        {
        case '0':
          if (!substitute_warned)
            {
              MP4HERROR ((warning_status, 0, _("\
Warning:%s:%d: \\0 will disappear, use \\& instead in replacements"),
                    CURRENT_FILE_LINE));
              substitute_warned = 1;
            }
          /* Fall through.  */

        case '&':
          obstack_grow (obs, victim + regs[0],
                        regs[1] - regs[0]);
          break;

        case '1': case '2': case '3': case '4': case '5': case '6':
        case '7': case '8': case '9': 
          ch -= '0';
          if (regs[ch*2+1] > 0)
            obstack_grow (obs, victim + regs[ch*2],
                          regs[ch*2+1] - regs[ch*2]);
          break;

        default:
          obstack_1grow (obs, ch);
          break;
        }
    }
}

/*--------------------------------------------------------------------------.
| Regular expression version of index.  Given two arguments, expand to the  |
| index of the first match of the second argument (a regexp) in the first.  |
| Expand to -1 if here is no match.                                         |
`--------------------------------------------------------------------------*/

static void
string_regexp (struct obstack *obs, int argc, token_data **argv,
               int options, const char *action)
{
  char *victim;                 /* first argument */
  char *regexp;                 /* regular expression */

  int length;                   /* length of first argument */
  int startpos = -1;            /* start position of match */
  int match_length = 0;         /* length of pattern match */

  pcre *re;
  int *match_ptr = NULL;
  int max_subexps = 0;
  int rc;

  victim = ARG (1);
  remove_special_chars (victim, FALSE);
  length = strlen (victim);

  regexp = ARG (2);
  remove_special_chars (regexp, FALSE);

  re = xre_compile (regexp, options);
  if (re == NULL)
    return;

  /* Accept any number of subexpressions */
  do
    {
      max_subexps += 10;
      match_ptr = (int *)
         xrealloc ((voidstar) match_ptr, sizeof (int) * max_subexps * 3);
      rc  = pcre_exec (re, NULL, victim, length, 0,
                          0, match_ptr, max_subexps * 3);
    }
  while (rc == PCRE_ERROR_NOMEMORY);

  if (rc > 0)
    {
      match_length = match_ptr[1] - match_ptr[0];
      startpos = match_ptr[0];
    }

  if (strcmp(action, "startpos") == 0)
    {
      if (startpos >= 0)
        shipout_int (obs, startpos);
    }
  else if (strcmp(action, "endpos") == 0)
    {
      if (startpos >= 0)
        shipout_int (obs, startpos + match_length);
    }
  else if (strcmp(action, "length") == 0)
    {
      if (startpos >= 0)
        shipout_int (obs, match_length);
    }
  else if (strcmp(action, "delete") == 0)
    {
      if (startpos >= 0)
        {
          obstack_grow (obs, ARG (1), startpos);
          obstack_grow (obs, ARG (1) + startpos + match_length,
              strlen (ARG (1)) - startpos - match_length);
        }
      else
        obstack_grow (obs, ARG (1), strlen (ARG (1)));
    }
  else if (strcmp(action, "extract") == 0)
    {
      if (startpos >= 0)
        obstack_grow (obs, ARG (1) + startpos, match_length);
    }
  else
    {
      if (startpos >= 0)
        obstack_grow (obs, "true", 4);
    }

  pcre_free (re);
  xfree ((voidstar) match_ptr);
}

/*----------------------------------------------.
| Substitutes regular expressions in a string.  |
| A string is returned.                         |
`----------------------------------------------*/
static void
subst_in_string (struct obstack *obs, int argc, token_data **argv,
        int extra_re_flags)
{
  char *victim;                 /* first argument */
  char *regexp;                 /* regular expression */

  pcre *re;
  pcre_extra *re_extra = NULL;
  const char *errptr;

  int *match_ptr = NULL;
  int max_subexps = 0;
  int options = 0;
  int rc;

  int matchpos;                 /* start position of match */
  int offset;                   /* current match offset */
  int length;                   /* length of first argument */

  if (bad_argc (argv[0], argc, 2, 4))
    return;

  victim = ARG (1);
  remove_special_chars (victim, FALSE);
  length = strlen (victim);

  regexp = ARG (2);
  remove_special_chars (regexp, FALSE);

  re = xre_compile (regexp, extra_re_flags);
  if (re == NULL)
    return;

  re_extra = pcre_study (re, 0, &errptr);
  if (errptr != NULL)
    {
      MP4HERROR ((warning_status, 0,
        _("Error:%s:%d: Bad regular expression `%s': %s"),
             CURRENT_FILE_LINE, regexp, errptr));
      return;
    }

  offset = 0;
  matchpos = 0;
  while (offset < length)
    {
      /* Accept any number of subexpressions */
      do
        {
          max_subexps += 10;
          match_ptr = (int *)
            xrealloc ((voidstar) match_ptr, sizeof (int) * max_subexps * 3);
          rc = pcre_exec (re, re_extra, victim+offset, length-offset, 0,
                          options, match_ptr, max_subexps * 3);
        }
      while (rc == PCRE_ERROR_NOMEMORY);
      max_subexps -= 10;

      /*  Subsequent calls to regexec do not match beginning o line */
      options |= PCRE_NOTBOL;

      if (rc < 0)
        {

          /* Match failed -- either error or there is no match in the
             rest of the string, in which case the rest of the string is
             copied verbatim.  */

          if (rc != PCRE_ERROR_NOMATCH)
            MP4HERROR ((warning_status, 0,
              _("Warning:%s:%d: Error matching regular expression `%s'"),
                   CURRENT_FILE_LINE, regexp));
          else if (offset < length)
            obstack_grow (obs, victim + offset, length - offset);
          break;
        }

      /* Copy the part of the string that was skipped by regex ().  */
      matchpos = match_ptr[0];

      if (matchpos > 0)
        obstack_grow (obs, victim + offset, matchpos);

      /* Handle the part of the string that was covered by the match.  */
      substitute (obs, victim+offset, ARG (3), match_ptr);

      /* Update the offset to the end of the match.  If the regexp
         matched a null string, advance offset one more, to avoid
         infinite loops.  */

      offset += match_ptr[1];
      if (match_ptr[0] == match_ptr[1])
        obstack_1grow (obs, victim[offset++]);
    }

  pcre_free (re);
  pcre_free (re_extra);
  xfree ((voidstar) match_ptr);
}

/*------------------------------------------------.
| Routine parsing attributes to set regex flags.  |
`------------------------------------------------*/
static int
regex_attributes (int *ptr_argc, token_data **argv)
{
  const char *singleline, *caseless, *reflags;
  const char *cp;
  int re_flags = 0;

  singleline = predefined_attribute ("singleline", ptr_argc, argv, TRUE);
  if (singleline)
    {
      if (strcmp (singleline, "true") == 0)
        re_flags |= PCRE_DOTALL;
      else
        re_flags |= PCRE_MULTILINE;
    }

  caseless = predefined_attribute ("caseless", ptr_argc, argv, TRUE);
  if (caseless && strcmp (caseless, "true") == 0)
    re_flags |= PCRE_CASELESS;

  /*  This one overrides previous options  */
  reflags = predefined_attribute ("reflags", ptr_argc, argv, TRUE);
  if (reflags)
    {
      re_flags = 0;
      for (cp = reflags; *cp != '\0'; cp++)
        {
          switch (*cp)
            {
              case '-': break;
              case 'i': re_flags |= PCRE_CASELESS; break;
              case 'm': re_flags |= PCRE_MULTILINE; break;
              case 's': re_flags |= PCRE_DOTALL; break;
              case 'x': re_flags |= PCRE_EXTENDED; break;
              default:
                        MP4HERROR ((warning_status, 0, _("\
Warning:%s:%d: Unknown modifier `%c' in reflags"),
                             CURRENT_FILE_LINE, *cp));
                        break;
            }
        }
    }

  return re_flags;
}

static void
mp4h_bp_subst_in_string (MP4H_BUILTIN_ARGS)
{
  int extra_re_flags;

  extra_re_flags = regex_attributes (&argc, argv);
  subst_in_string (obs, argc, argv, extra_re_flags);
}

/*------------------------------------------------.
| Substitutes regular expressions in a variable.  |
`------------------------------------------------*/
static void
mp4h_bp_subst_in_var (MP4H_BUILTIN_ARGS)
{
  symbol *var;
  token_data td;
  int extra_re_flags;
  char *text;
  struct obstack temp_obs;

  extra_re_flags = regex_attributes (&argc, argv);

  if (bad_argc (argv[0], argc, 3, 4))
    return;

  var = lookup_variable (ARG (1), SYMBOL_LOOKUP);
  if (var == NULL)
    return;

  TOKEN_DATA_TYPE (&td) = TOKEN_TEXT;
  TOKEN_DATA_TEXT (&td) = SYMBOL_TEXT (var);

  obstack_init (&temp_obs);

  argv[1] = &td;
  subst_in_string (&temp_obs, argc, argv, extra_re_flags);
  obstack_1grow (&temp_obs, '\0');
  text = obstack_finish (&temp_obs);
  xfree ((voidstar) SYMBOL_TEXT (var));
  SYMBOL_TEXT (var) = xstrdup (text);
  obstack_free (&temp_obs, NULL);
}

/*-------------------------------------------------------------------.
| This function compares a string and a regular expression.          |
| Attribute ``action'' can be                                        |
|   report : returns "true" if string match the regular expression.  |
|  extract : returns the portion of the string matched by the        |
|            regular expression.                                     |
|   delete : deletes the portion of the string matched by the        |
|            regular expression and prints resulting string.         |
| startpos : returns index of the first character matching the       |
|            regular expression, -1 if no match.                     |
|   endpos : returns index of the last character matching the        |
|            regular expression, -1 if no match.                     |
|   length : returns length of the matched string.                   |
`-------------------------------------------------------------------*/
static void
mp4h_bp_match (MP4H_BUILTIN_ARGS)
{
  const char *action;
  int extra_re_flags;

  extra_re_flags = regex_attributes (&argc, argv);

  action = predefined_attribute ("action", &argc, argv, TRUE);
  if (!action)
    action = "report";

  if (bad_argc (argv[0], argc, 3, 4))
    return;

  string_regexp (obs, argc, argv, extra_re_flags, action);
}


/* Operation on variables: define, undefine, search, insert,...
   Variables are either strings or array of strings.  */

/*--------------------------------------------------------------.
| The function generic_variable_lookup is the generic function  |
| used by mp4h_bp_get_var and mp4h_bp_get_var_once.             |
`--------------------------------------------------------------*/

static void
generic_variable_lookup (MP4H_BUILTIN_ARGS, boolean verbatim)
{
  char *cp, *ptr_index;
  symbol *var, *index_var;
  int i;
  int length;
  int array_index;

  if (argc < 2)
    return;

  for (i = 1; i < argc; i++)
    {
      array_index = -1;
      ptr_index = strchr (ARG (i), ']');
      if (ptr_index != NULL)
        {
          if (*(ptr_index-1) == '[')
            {
              *(ptr_index-1) = '\0';
              ptr_index = NULL;
            }
          else
            {
              *ptr_index = '\0';
              ptr_index = strchr (ARG (i), '[');
              if (!ptr_index)
                {
                  MP4HERROR ((warning_status, 0, _("\
Warning:%s:%d: Wrong index declaration in <%s>"),
                       CURRENT_FILE_LINE, ARG (0)));
                  return;
                }
              *ptr_index = '\0';
              ptr_index++;
              if (!numeric_arg (argv[0], ptr_index, FALSE, &array_index))
                {
                  /*   Maybe there is an implicit index like in
                         <get-var foo[i]>.  */
                  index_var = lookup_variable (ptr_index, SYMBOL_LOOKUP);
                  if (index_var)
                    {
                      if (!numeric_arg (argv[0], SYMBOL_TEXT (index_var),
                                  FALSE, &array_index))
                          index_var = NULL;
                    }
                  if (!index_var)
                    {
                      MP4HERROR ((warning_status, 0, _("\
Warning:%s:%d: Wrong index declaration in <%s>"),
                           CURRENT_FILE_LINE, ARG (0)));
                      return;
                    }
                }
              if (array_index < 0)
                continue;
            }
        }

      var = lookup_variable (ARG (i), SYMBOL_LOOKUP);
      if (var == NULL)
        return;

      if (array_index < 0)
        {
          cp = SYMBOL_TEXT (var);
          length = strlen (cp);
        }
      else
        cp = array_value (var, array_index, &length);

      if (cp)
        {
          if (verbatim)
            obstack_1grow (obs, CHAR_LQUOTE);
          obstack_grow (obs, cp, length);
          if (verbatim)
            obstack_1grow (obs, CHAR_RQUOTE);
        }
    }
}

/*-------------------------------------------------------.
| Define a variable.  Value is found in the body.        |
| Intentionally, variables cannot be indexed with this   |
| tag, because an entire array can be assigned using     |
| this.                                                  |
`-------------------------------------------------------*/
static void
mp4h_bp_set_var_x (MP4H_BUILTIN_ARGS)
{
  const char *name;
  symbol *var;

  name = predefined_attribute ("name", &argc, argv, FALSE);
  if (name == NULL)
    {
      MP4HERROR ((warning_status, 0,
        _("Error:%s:%d: In <%s>, required attribute `%s' is not specified"),
                CURRENT_FILE_LINE, ARG (0), "name"));
      return;
    }

  var = lookup_variable (name, SYMBOL_INSERT);
  if (SYMBOL_TYPE (var) == TOKEN_TEXT)
    xfree ((voidstar) SYMBOL_TEXT (var));
  SYMBOL_TEXT (var) = xstrdup (ARGBODY);
  SYMBOL_TYPE (var) = TOKEN_TEXT;
}

/*----------------------------------------------------------------------.
| Define a variable.  Argument is evaluated or not depending on whether |
| the 'expand attributes' flag is on in the builtin_tab above.          |
`----------------------------------------------------------------------*/
static void
mp4h_bp_set_var (MP4H_BUILTIN_ARGS)
{
  char *value, *cp, *ptr_index, *old_value, *new_value;
  symbol *var, *index_var;
  register int i;
  register int j;
  int length, istep, size;
  int array_index;

  if (argc < 2)
    return;

  for (i = 1; i < argc; i++)
    {
      array_index = -1;
      istep = 0;
      value = strchr (ARG (i), '=');
      if (value == NULL)
        {
          /*  Look for spaces like in <set-var i = 0> */
          if (i+1 < argc && *(ARG (i+1)) == '=')
            {
              istep++;
              if (i+2 < argc && *(ARG (i+1) +1) == '\0')
                {
                  istep++;
                  value = ARG (i+2);
                }
              else
                value = ARG (i+1) + 1;
            }
          else
            value = ARG (i) + strlen (ARG (i));
        }
      else
        {
          *value = '\0';
          value++;
        }
      
      /*  Remove special quote characters. */
      remove_special_chars (value, FALSE);

      ptr_index = strchr (ARG (i), ']');
      if (ptr_index != NULL)
        {
          if (*(ptr_index-1) == '[')
            {
              *(ptr_index-1) = '\0';
              ptr_index = NULL;
            }
          else
            {
              *ptr_index = '\0';
              ptr_index = strchr (ARG (i), '[');
              if (!ptr_index)
                {
                  MP4HERROR ((warning_status, 0, _("\
Warning:%s:%d: Wrong index declaration in <%s>"),
                       CURRENT_FILE_LINE, ARG (0)));
                  return;
                }
              *ptr_index = '\0';
              ptr_index++;
              if (!numeric_arg (argv[0], ptr_index, FALSE, &array_index))
                {
                  /*   Maybe there is an implicit index like in
                         <set-var foo[i]=bar>.  */
                  index_var = lookup_variable (ptr_index, SYMBOL_LOOKUP);
                  if (index_var)
                    {
                      if (!numeric_arg (argv[0], SYMBOL_TEXT (index_var),
                                  FALSE, &array_index))
                          index_var = NULL;
                    }
                  if (!index_var)
                    {
                      MP4HERROR ((warning_status, 0, _("\
Warning:%s:%d: Wrong index declaration in <%s>"),
                           CURRENT_FILE_LINE, ARG (0)));
                      return;
                    }
                }
            }
        }

      var = lookup_variable (ARG (i), SYMBOL_INSERT);
      if (ptr_index == NULL)
        {
          /*  single value.  */
          if (SYMBOL_TYPE (var) == TOKEN_TEXT)
            xfree ((voidstar) SYMBOL_TEXT (var));
          SYMBOL_TEXT (var) = xstrdup (value);
        }
      else if (array_index >= 0)
        {
          /*  an index has been specified.  */
          if (SYMBOL_TYPE (var) == TOKEN_TEXT)
            old_value = SYMBOL_TEXT (var);
          else
            old_value = "";

          length = strlen (old_value) + strlen (value) + array_index + 1;
          new_value = (char *) xmalloc (length + 1);
          *new_value = '\0';

          if (array_index == 0)
            {
              strcat (new_value, value);
              cp = strchr (old_value, '\n');
              if (cp)
                strcat (new_value, cp);
            }
          else
            {
              size = array_size (var);
              if (size == 0)
                size = 1;
              if (size <= array_index)
                {
                  strcat (new_value, old_value);
                  for (j=size; j<=array_index; j++)
                    strcat (new_value, "\n");
                  strcat (new_value, value);
                }
              else
                {
                  cp = array_value (var, array_index, 0);
                  strncat (new_value, old_value, cp - SYMBOL_TEXT (var));
                  strcat (new_value, value);
                  cp = strchr (cp, '\n');
                  if (cp)
                    strcat (new_value, cp);
                }
            }

          if (SYMBOL_TYPE (var) == TOKEN_TEXT)
            xfree ((voidstar) SYMBOL_TEXT (var));
          SYMBOL_TEXT (var) = new_value;
        }

      SYMBOL_TYPE (var) = TOKEN_TEXT;
      i += istep;
    }
}

/*------------------------------.
| Get the value of a variable.  |
`------------------------------*/
static void
mp4h_bp_get_var (MP4H_BUILTIN_ARGS)
{
  generic_variable_lookup (MP4H_BUILTIN_RECUR, FALSE);
}

/*--------------------------------------------------------------.
| Get the value of a variable without expanding its attribute.  |
`--------------------------------------------------------------*/
static void
mp4h_bp_get_var_once (MP4H_BUILTIN_ARGS)
{
  generic_variable_lookup (MP4H_BUILTIN_RECUR, TRUE);
}

/*----------------------.
| Undefine a variable.  |
`----------------------*/
static void
mp4h_bp_unset_var (MP4H_BUILTIN_ARGS)
{
  int i;

  for (i=1; i<argc; i++)
    lookup_variable (ARG (i), SYMBOL_DELETE);
}

/*-----------------------------------------------------.
| Push a variable to a stack and reset this variable.  |
`-----------------------------------------------------*/
static void
mp4h_bp_preserve (MP4H_BUILTIN_ARGS)
{
  symbol *var;
  var_stack *next;
  int i;

  if (bad_argc (argv[0], argc, 2, 0))
    return;

  for (i=argc-1; i>0; i--)
    {
      next = (var_stack *) xmalloc (sizeof (var_stack));
      var  = lookup_variable (ARG (i), SYMBOL_LOOKUP);
      if (var)
        {
          next->text = xstrdup (SYMBOL_TEXT (var));
          *(SYMBOL_TEXT (var)) = '\0';
        }
      else
        next->text = xstrdup ("");

      next->prev = vs;
      vs = next;
    }
}

/*-----------------------------------------.
| Pop a variable from the variable stack.  |
`-----------------------------------------*/
static void
mp4h_bp_restore (MP4H_BUILTIN_ARGS)
{
  symbol *var;
  var_stack *prev;
  int i;

  if (bad_argc (argv[0], argc, 2, 0))
    return;

  for (i=1; i<argc; i++)
    {
      var = lookup_variable (ARG (i), SYMBOL_INSERT);

      if (!vs)
        {
          MP4HERROR ((warning_status, 0, _("\
Warning:%s:%d: Variable stack empty, it means <%s> already gobbled all data"),
                    CURRENT_FILE_LINE, ARG (0)));
          return;
        }
      if (SYMBOL_TYPE (var) == TOKEN_TEXT)
        xfree ((voidstar) SYMBOL_TEXT (var));
      SYMBOL_TEXT (var) = xstrdup (vs->text);
      SYMBOL_TYPE (var) = TOKEN_TEXT;

      prev = vs->prev;
      xfree ((voidstar) vs->text);
      xfree ((voidstar) vs);
      vs = prev;
    }
}

static void
varstack_check (void)
{
  if (vs)
    {
      MP4HERROR ((warning_status, 0, _("\
Warning:%s:%d: Variable stack not empty, it means <preserve> pushed more items than <restore> popped"),
                    CURRENT_FILE_LINE));
    }
}

/*----------------------------.
| Test if a variable exists.  |
`----------------------------*/
static void
mp4h_bp_var_exists (MP4H_BUILTIN_ARGS)
{
  symbol *var;

  var = lookup_variable (ARG (1), SYMBOL_LOOKUP);
  if (var != NULL)
    shipout_text (obs, "true");
}

/*-----------------------.
| Increment a variable.  |
`-----------------------*/
static void
mp4h_bp_increment (MP4H_BUILTIN_ARGS)
{
  int value, incr;
  symbol *var;
  char buf[128];
  const char *by;

  by = predefined_attribute ("by", &argc, argv, FALSE);
  if (bad_argc (argv[0], argc, 2, 2))
    return;

  if (!by)
    by = "1";

  if (bad_argc (argv[0], argc, 2, 2))
    return;

  var = lookup_variable (ARG (1), SYMBOL_LOOKUP);
  if (var == NULL)
    return;
  if (!numeric_arg (argv[0], SYMBOL_TEXT (var), TRUE, &value))
    return;
  if (!numeric_arg (argv[0], by, TRUE, &incr))
    return;

  if (SYMBOL_TYPE (var) == TOKEN_TEXT)
    xfree ((voidstar) SYMBOL_TEXT (var));

  sprintf (buf, "%d", value+incr);
  SYMBOL_TEXT (var) = xstrdup(buf);
}

static void
mp4h_bp_decrement (MP4H_BUILTIN_ARGS)
{
  int value, incr;
  symbol *var;
  char buf[128];
  const char *by;

  by = predefined_attribute ("by", &argc, argv, FALSE);
  if (bad_argc (argv[0], argc, 2, 2))
    return;

  if (!by)
    by = "1";

  if (bad_argc (argv[0], argc, 2, 2))
    return;

  var = lookup_variable (ARG (1), SYMBOL_LOOKUP);
  if (var == NULL)
    return;
  if (!numeric_arg (argv[0], SYMBOL_TEXT (var), TRUE, &value))
    return;
  if (!numeric_arg (argv[0], by, TRUE, &incr))
    return;

  if (SYMBOL_TYPE (var) == TOKEN_TEXT)
    xfree ((voidstar) SYMBOL_TEXT (var));

  sprintf (buf, "%d", value-incr);
  SYMBOL_TEXT (var) = xstrdup(buf);
}

/*--------------------------------.
| Dumps informations of symbols.  |
`--------------------------------*/
static void
mp4h_bp_symbol_info (MP4H_BUILTIN_ARGS)
{
  symbol *var;
  int size;

  if (bad_argc (argv[0], argc, 0, 2))
    return;

  /*  First look if this variable is defined.  */
  var = lookup_variable (ARG (1), SYMBOL_LOOKUP);
  if (var != NULL)
    {
      size = array_size (var);
      if (size == 0)
        size = 1;
      obstack_grow (obs, "STRING\n", 7);
      shipout_int (obs, size);
      return;
    }

  var = lookup_symbol (ARG (1), SYMBOL_LOOKUP);
  if (var != NULL)
    {
      if (SYMBOL_TYPE (var) == TOKEN_FUNC)
        {
          if (SYMBOL_CONTAINER (var))
            obstack_grow (obs, "PRIM COMPLEX", 12);
          else
            obstack_grow (obs, "PRIM TAG", 8);
        }
      else if (SYMBOL_TYPE (var) == TOKEN_TEXT)
        {
          if (SYMBOL_CONTAINER (var))
            obstack_grow (obs, "USER COMPLEX", 12);
          else
            obstack_grow (obs, "USER TAG", 8);
        }
    }
}

/*--------------------------------.
| Copy a variable to another one. |
`--------------------------------*/
static void
mp4h_bp_copy_var (MP4H_BUILTIN_ARGS)
{
  symbol *var1, *var2;

  if (bad_argc (argv[0], argc, 3, 3))
    return;

  var1 = lookup_variable (ARG (1), SYMBOL_LOOKUP);
  if (var1 == NULL)
    {
      MP4HERROR ((warning_status, 0,
        _("Warning:%s:%d: Variable `%s' not defined in <%s>"),
             CURRENT_FILE_LINE, ARG (1), ARG (0)));
      return;
    }
  var2 = lookup_variable (ARG (2), SYMBOL_INSERT);
  if (SYMBOL_TYPE (var2) == TOKEN_TEXT)
    xfree ((voidstar) SYMBOL_TEXT (var2));

  SYMBOL_TEXT (var2) = xstrdup(SYMBOL_TEXT (var1));
  SYMBOL_TYPE (var2) = TOKEN_TEXT;
}

/*--------------------------------.
| Defines a variable only if it   |
| is not set yet.                 |
`--------------------------------*/
static void
mp4h_bp_defvar (MP4H_BUILTIN_ARGS)
{
  symbol *var;

  if (bad_argc (argv[0], argc, 0, 3))
    return;

  var = lookup_variable (ARG (1), SYMBOL_LOOKUP);
  if (var == NULL)
    {
      var = lookup_variable (ARG (1), SYMBOL_INSERT);
      SYMBOL_TYPE (var) = TOKEN_TEXT;
      SYMBOL_TEXT (var) = xstrdup(ARG (2));
    }
  else if (SYMBOL_TYPE (var) == TOKEN_TEXT && *(SYMBOL_TEXT (var)) == '\0')
    {
      xfree ((voidstar) SYMBOL_TEXT (var));
      SYMBOL_TEXT (var) = xstrdup(ARG (2));
    }
}


/*  Array functions: */
/*------------------------------------------------------------.
| An array is a representation of variables, it is a newline  |
| separated list of strings.                                  |
`------------------------------------------------------------*/

/*-----------------------------------------.
| Returns number of elements of an array.  |
`-----------------------------------------*/
static int
array_size (symbol *var)
{
  char *cp;
  int result = 0;

  if (var != NULL && SYMBOL_TYPE (var) == TOKEN_TEXT
      && SYMBOL_TEXT (var) != NULL && *(SYMBOL_TEXT (var)) != '\0')
    {
      result++;
      for (cp=SYMBOL_TEXT (var); *cp != '\0'; cp++)
        if (*cp == '\n')
          result++;
    }
  return result;
}

/*------------------------------------.
| Returns the nth value of an array.  |
| Returns NULL if array is too small. |
`------------------------------------*/
static char *
array_value (symbol *var, int offset, int *length)
{
  char *cp, *value;
  int i;

  value = NULL;
  if (offset == 0)
    value = SYMBOL_TEXT (var);
  else if (offset > 0)
    {
      value = SYMBOL_TEXT (var);
      for (i=0; i<offset; i++)
        {
          value = strchr (value, '\n');
          if (!value)
            break;
          value++;
        }
    }

  if (length != NULL)
    {
      if (value)
        {
          cp = strchr (value, '\n');
          if (cp != NULL)
            *length = (int) (cp - value);
          else
            *length = strlen (value);
        }
      else
        *length = 0;
    }

  return value;
}

/*----------------------------------------.
| Prints number of elements of an array.  |
`----------------------------------------*/
static void
mp4h_bp_array_size (MP4H_BUILTIN_ARGS)
{
  symbol *var;
  int result = -1;

  if (bad_argc (argv[0], argc, 2, 2))
    return;

  var = lookup_variable (ARG (1), SYMBOL_LOOKUP);
  if (var != NULL)
    result = array_size (var);
  shipout_int (obs, result);
}

/*-------------------------------------------------------------.
| Search an element in an array.  If it is found, returns its  |
| index, otherwise returns -1.                                 |
`-------------------------------------------------------------*/
static int
array_member (const char *text, symbol *var, boolean caseless)
{
  char *cp, *next_item, *value;
  int result = -1;
  boolean found = FALSE;

  value = xstrdup (SYMBOL_TEXT (var));
  cp = value;
  while (1)
    {
      result++;
      next_item = strchr (cp, '\n');
      if (next_item)
        *next_item = '\0';

      if (caseless)
        found = (strcasecmp (cp, text) == 0);
      else
        found = (strcmp (cp, text) == 0);

      if (next_item == NULL || found)
        break;

      cp = next_item + 1;
    }
  xfree ((voidstar) value);
  
  if (found)
    return result;
  else
    return -1;
}

static void
mp4h_bp_array_member (MP4H_BUILTIN_ARGS)
{
  int result = -1;
  symbol *var;
  const char *caseless;

  caseless = predefined_attribute ("caseless", &argc, argv, TRUE);
  if (! bad_argc (argv[0], argc, 3, 3))
    {
      var = lookup_variable (ARG (1), SYMBOL_LOOKUP);
      if (var != NULL)
        result = array_member (ARG (2), var, (caseless != NULL));
    }
  shipout_int (obs, result);
}

/*-------------------------------------------------------------.
| Adds an element to an array only if it is not yet present.   |
`-------------------------------------------------------------*/
static void
mp4h_bp_array_add_unique (MP4H_BUILTIN_ARGS)
{
  symbol *var;
  char *value;
  const char *caseless;
  int exists;

  caseless = predefined_attribute ("caseless", &argc, argv, TRUE);
  if (bad_argc (argv[0], argc, 3, 3))
    return;

  var = lookup_variable (ARG (1), SYMBOL_LOOKUP);
  if (var == NULL)
    {
      var = lookup_variable (ARG (1), SYMBOL_INSERT);
      SYMBOL_TEXT (var) = xstrdup (ARG (2));
      SYMBOL_TYPE (var) = TOKEN_TEXT;
    }
  else if (SYMBOL_TYPE (var) != TOKEN_TEXT || *(SYMBOL_TEXT (var)) == '\0')
    {
      SYMBOL_TEXT (var) = xstrdup (ARG (2));
      SYMBOL_TYPE (var) = TOKEN_TEXT;
    }
  else
    {
      exists = array_member (ARG (2), var, (caseless != NULL));
      if (exists == -1)
        {
          value = (char *) xmalloc (strlen (SYMBOL_TEXT (var)) + strlen (ARG (2)) + 2);
          sprintf (value, "%s\n%s", SYMBOL_TEXT (var), ARG (2));
          xfree ((voidstar) SYMBOL_TEXT (var));
          SYMBOL_TEXT (var) = xstrdup (value);
          xfree ((voidstar) value);
        }
    }
}

/*------------------------------.
| Adds an element to an array.  |
`------------------------------*/
static void
mp4h_bp_array_push (MP4H_BUILTIN_ARGS)
{
  symbol *var;
  char *old_value;

  if (bad_argc (argv[0], argc, 2, 3))
    return;

  var = lookup_variable (ARG (1), SYMBOL_INSERT);
  if (SYMBOL_TYPE (var) != TOKEN_TEXT)
    {
      SYMBOL_TEXT (var) = xstrdup (ARG (2));
      SYMBOL_TYPE (var) = TOKEN_TEXT;
    }
  else
    {
      if (*(SYMBOL_TEXT (var)) == '\0')
        {
          xfree ((voidstar) SYMBOL_TEXT (var));
          SYMBOL_TEXT (var) = xstrdup (ARG (2));
        }
      else
        {
          old_value = (char *) xmalloc (strlen (SYMBOL_TEXT (var)) + strlen (ARG (2)) + 2);
          sprintf (old_value, "%s\n%s", SYMBOL_TEXT (var), ARG (2));
          xfree ((voidstar) SYMBOL_TEXT (var));
          SYMBOL_TEXT (var) = old_value;
        }
    }
}

/*---------------------------------------------------------.
| Remove the top level value from an array and return it.  |
`---------------------------------------------------------*/
static void
mp4h_bp_array_pop (MP4H_BUILTIN_ARGS)
{
  symbol *var;
  char *cp;

  if (bad_argc (argv[0], argc, 2, 2))
    return;

  var = lookup_variable (ARG (1), SYMBOL_LOOKUP);
  if (var == NULL)
    return;

  cp = strrchr (SYMBOL_TEXT (var), '\n');
  if (cp)
    {
      *cp = '\0';
      shipout_string (obs, cp+1, 0);
    }
  else
    {
      shipout_string (obs, SYMBOL_TEXT (var), 0);
      *(SYMBOL_TEXT (var)) = '\0';
    }
}

/*-------------------------------------------.
| Prints the value with the highest index.   |
`-------------------------------------------*/
static void
mp4h_bp_array_topvalue (MP4H_BUILTIN_ARGS)
{
  symbol *var;
  char *cp;

  if (bad_argc (argv[0], argc, 2, 2))
    return;

  var = lookup_variable (ARG (1), SYMBOL_LOOKUP);
  if (var == NULL)
    return;

  cp = strrchr (SYMBOL_TEXT (var), '\n');
  if (cp)
    shipout_string (obs, cp+1, 0);
  else
    shipout_string (obs, SYMBOL_TEXT (var), 0);
}

/*-----------------------------------------------------------------.
| Shifts an array.  If offset is negative, first values are lost.  |
| Otherwise, the array is shifted and null values are inserted at  |
| the beginning of this array.                                     |
| If the "start" attribute is set, elements below this one are     |
| left unchanged.                                                  |
`-----------------------------------------------------------------*/
static void
mp4h_bp_array_shift (MP4H_BUILTIN_ARGS)
{
  int offset, ind_start;
  symbol *var;
  char *cp, *value, *old_value;
  const char *start;
  int i;

  if (bad_argc (argv[0], argc, 3, 4))
    return;

  var = lookup_variable (ARG (1), SYMBOL_LOOKUP);
  if (var == NULL)
    {
      MP4HERROR ((warning_status, 0,
        _("Warning:%s:%d: Variable `%s' not defined in <%s>"),
             CURRENT_FILE_LINE, ARG (1), ARG (0)));
    }

  if (!numeric_arg (argv[0], ARG (2), TRUE, &offset))
    return;

  if (SYMBOL_TYPE (var) != TOKEN_TEXT)
    {
      SYMBOL_TYPE (var) = TOKEN_TEXT;
      SYMBOL_TEXT (var) = xstrdup ("");
    }

  ind_start = 0;
  start = predefined_attribute ("start", &argc, argv, FALSE);
  if (start)
    {
      if (!numeric_arg (argv[0], start, TRUE, &ind_start))
        return;
    }

  if (offset == 0)
    return;

  value = (char *) xmalloc (strlen (SYMBOL_TEXT (var)) + offset + 1);
  if (ind_start > 0)
    {
      old_value = array_value (var, ind_start, 0);
      if (!old_value)
        return;
      *(old_value-1) = '\0';
      strcpy ((char *) value, SYMBOL_TEXT (var));
      cp = value + strlen (SYMBOL_TEXT (var)) + 1;
      *(cp-1) = '\n';
      *(old_value-1) = '\n';
    }
  else
    {
      old_value = SYMBOL_TEXT (var);
      cp = value;
    }
  *cp = '\0';

  if (offset > 0)
    {
      for (i=0; i<offset; i++)
        *(cp+i) = '\n';

      *(cp+offset) = '\0';
      strcpy ((char *) (cp+offset), old_value);
    }
  else
    {
      old_value = array_value (var, ind_start-offset, 0);
      if (!old_value)
        return;
      strcpy ((char *) cp, old_value);
    }
  xfree ((voidstar) SYMBOL_TEXT (var));
  SYMBOL_TEXT (var) = value;
}

/*------------------------------.
| Concatenate multiple arrays.  |
`------------------------------*/
static void
mp4h_bp_array_concat (MP4H_BUILTIN_ARGS)
{
  symbol *var, *varadd;
  char *value;
  int i;

  if (bad_argc (argv[0], argc, 3, 0))
    return;

  var = lookup_variable (ARG (1), SYMBOL_LOOKUP);
  if (var == NULL)
    {
      var = lookup_variable (ARG (1), SYMBOL_INSERT);
      SYMBOL_TEXT (var) = xstrdup ("");
      SYMBOL_TYPE (var) = TOKEN_TEXT;
    }
  for (i=2; i<argc; i++)
    {
      varadd = lookup_variable (ARG (i), SYMBOL_LOOKUP);
      if (varadd == NULL)
        continue;
      if (SYMBOL_TYPE (varadd) != TOKEN_TEXT)
        continue;
      
      value = (char *) xmalloc (strlen (SYMBOL_TEXT (var)) +
                       strlen (SYMBOL_TEXT (varadd)) + 2);
      sprintf (value, "%s\n%s", SYMBOL_TEXT (var), SYMBOL_TEXT (varadd));
      xfree ((voidstar) SYMBOL_TEXT (var));
      SYMBOL_TEXT (var) = xstrdup (value);
      xfree ((voidstar) value);
    }
}

/*---------------------------------------------------------------------.
| This function is used to sort arrays.  Arguments are either strings  |
| or numerical values.                                                 |
`---------------------------------------------------------------------*/
static int
sort_function (const void *item1, const void *item2)
{
  char *string1, *string2;
  int result;
  double val1, val2;
  
  string1 = *(char **)item1;
  string2 = *(char **)item2;

  if (sort_numeric)
    {
      val1 = strtod (string1, 0);
      val2 = strtod (string2, 0);
      if ( val1 == val2 )
        result = 0;
      if ( val1 > val2 )
        result = 1;
      else
        result = -1;
    }
  else
    {
      if (sort_caseless)
        result = strcasecmp (string1, string2);
      else
        result = strcmp (string1, string2);
    }

  if (sort_sortorder)
    result = - result;

  return result;
}

static void
mp4h_bp_sort (MP4H_BUILTIN_ARGS)
{
  symbol *var;
  const char *caseless, *numeric, *sortorder;
  char *cp, *value;
  char **array;
  int length, size, i;

  caseless  = predefined_attribute ("caseless", &argc, argv, TRUE);
  sortorder = predefined_attribute ("sortorder", &argc, argv, TRUE);
  numeric   = predefined_attribute ("numeric", &argc, argv, TRUE);

  if (bad_argc (argv[0], argc, 2, 2))
    return;

  var = lookup_variable (ARG (1), SYMBOL_LOOKUP);
  if (var == NULL)
    return;

  sort_caseless  = (caseless != NULL);
  sort_numeric   = (numeric != NULL);

  if (sortorder && strcmp (sortorder, "reverse") == 0)
    sort_sortorder = TRUE;
  else
    sort_sortorder = FALSE;

  length = strlen (SYMBOL_TEXT (var));
  size   = array_size (var);
  
  /*  Build a pointer to array values.  All newlines are replaced by
      NULL chars to use standard string comparison functions.  */
  array = (char **) xmalloc ((size+1) * sizeof (char *));
  i = 0;
  
  value = xstrdup (SYMBOL_TEXT (var));
  array[i] = value;

  for (cp=value; *cp != '\0'; cp++)
    {
      if (*cp == '\n')
        {
          *cp = '\0';
          i++;
          array[i] = cp + 1;
        }
    }

  qsort ((void *)array, size, sizeof (char *), sort_function);

  strcpy (SYMBOL_TEXT (var), array[0]);
  for (i=1; i<size; i++)
    {
      strcat (SYMBOL_TEXT (var), "\n");
      strcat (SYMBOL_TEXT (var), array[i]);
    }
  xfree ((voidstar) array);
  xfree ((voidstar) value);
}


/* This section contains the macros "divert", "undivert" and "divnum" for
   handling diversion.  The utility functions used lives in output.c.  */

/*-----------------------------------------------------------------------.
| Divert further output to the diversion given by ARGV[1].  Out of range |
| means discard further output.                                          |
`-----------------------------------------------------------------------*/

static void
mp4h_bp_divert (MP4H_BUILTIN_ARGS)
{
  const char *divnum;
  int i = 0;

  divnum = predefined_attribute ("divnum", &argc, argv, TRUE);
  if (!divnum || numeric_arg (argv[0], divnum, TRUE, &i))
    make_diversion (i);
}

/*-----------------------------------------------------.
| Expand to the current diversion number, -1 if none.  |
`-----------------------------------------------------*/

static void
mp4h_bp_divnum (MP4H_BUILTIN_ARGS)
{
  if (bad_argc (argv[0], argc, 1, 1))
    return;
  shipout_int (obs, current_diversion);
}

/*-----------------------------------------------------------------------.
| Bring back the diversion given by the divnum attribute.  If none is    |
| specified, bring back all diversions.                                  |
`-----------------------------------------------------------------------*/

static void
mp4h_bp_undivert (MP4H_BUILTIN_ARGS)
{
  int i;
  const char *divnum;

  divnum = predefined_attribute ("divnum", &argc, argv, FALSE);
  if (!divnum)
    undivert_all ();
  else
    if (numeric_arg (argv[0], divnum, TRUE, &i))
      insert_diversion (i);
}


/*-------------------------------------------------------------------------.
| This function handles all expansion of user defined and predefined       |
| macros.  It is called with an obstack OBS, where the macros expansion    |
| will be placed, as an unfinished object.  SYM points to the macro        |
| definition, giving the expansion text.  ARGC and ARGV are the arguments, |
| as usual.                                                                |
| This function is called by call_macro ().                                |
`-------------------------------------------------------------------------*/

void
expand_user_macro (struct obstack *obs, symbol *sym, int argc,
                   token_data **argv, read_type expansion)
{
  const char *text, *save;
  int i, len;
  boolean unexpanded;
  char sep[2];

  sep[1] = '\0';
  for (text = SYMBOL_TEXT (sym); *text != '\0';)
    {
      if (*text != '%')
        {
          save = text;
          len = 1;
          for (text++ ; *text != '%' && *text != '\0'; text++)
            len++;
          obstack_grow (obs, save, len);
          if (*text == '\0')
            break;
        }
      text++;
      unexpanded = FALSE;
      sep[0] = ' ';
      
      save = text;
      if (*text == '#')
        {
          shipout_int (obs, argc - 1);
          text++;
          continue;
        }
      else if (*text == '%')
        {
          obstack_1grow (obs, '%');
          text++;
          continue;
        }

      while (*text == 'U' || *text == 'A' || *text == 'u' || *text == 'y')
        {
          if (*text == 'U' || *text == 'u')
            unexpanded = TRUE;
          if (*text == 'A' || *text == 'y')
            sep[0] = '\n';
          text++;
        }

      if (isdigit ((int) *text))
        {
          char *endp;
          i = (int)strtol (text, &endp, 10) + 1;
          text = endp;
          obstack_1grow (obs, CHAR_BGROUP);
          if (i < argc)
            obstack_grow (obs, ARG (i), strlen (ARG (i)));
          obstack_1grow (obs, CHAR_EGROUP);
        }
      else if (strncmp (text, "body", 4) == 0
               || strncmp (text, "xbody", 5) == 0
               || strncmp (text, "qbody", 5) == 0)
        {
          if (unexpanded)
            obstack_1grow (obs, CHAR_LQUOTE);
          else
            obstack_1grow (obs, CHAR_BGROUP);

          if (SYMBOL_CONTAINER (sym))
            dump_args (obs, 2, (argv+argc-1), sep);
          else
            dump_args (obs, argc, argv, sep);

          if (unexpanded)
            obstack_1grow (obs, CHAR_RQUOTE);
          else
            obstack_1grow (obs, CHAR_EGROUP);

          if (*text == 'b')
            text += 4;
          else
            text += 5;
        }
      else if (strncmp (text, "attributes", 10) == 0
               || strncmp (text, "xattributes", 11) == 0
               || strncmp (text, "qattributes", 11) == 0)
        {
          if (*text == 'a')
            text += 10;
          else
            text += 11;

            if (unexpanded)
              {
                for (i = 1; i < argc; i++)
                  {
                    if (i > 1)
                      obstack_grow (obs, sep, strlen (sep));

                    obstack_1grow (obs, CHAR_LQUOTE);
                    obstack_grow (obs, ARG (i), strlen (ARG (i)));
                    obstack_1grow (obs, CHAR_RQUOTE);
                  }
              }
            else
              dump_args (obs, argc, argv, sep);
        }
      else if (strncmp (text, "name", 4) == 0)
        {
          obstack_grow (obs, SYMBOL_NAME (sym), strlen (SYMBOL_NAME (sym)));
          text += 4;
        }
      else
        {
          obstack_1grow (obs, '%');
          text = save;
        }
    }
}
