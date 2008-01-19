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

#ifndef MP4H_H
#define MP4H_H

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <sys/types.h>

# define voidstar void *

#include <stdio.h>
#include <ctype.h>

#include "obstack.h"

/* An ANSI string.h and pre-ANSI memory.h might conflict.  */

#if defined (HAVE_STRING_H) || defined (STDC_HEADERS)
# include <string.h>
# if !defined (STDC_HEADERS) && defined (HAVE_MEMORY_H)
#  include <memory.h>
# endif
/* This is for obstack code -- should live in obstack.h.  */
# ifndef bcopy
#  define bcopy(S, D, N) memcpy ((D), (S), (N))
# endif
#else
# include <strings.h>
# ifndef memcpy
#  define memcpy(D, S, N) bcopy((S), (D), (N))
# endif
# ifndef strchr
#  define strchr(S, C) index ((S), (C))
# endif
# ifndef strrchr
#  define strrchr(S, C) rindex ((S), (C))
# endif
# ifndef bcopy
void bcopy ();
# endif
#endif

#ifdef STDC_HEADERS
# include <stdlib.h>
#else /* not STDC_HEADERS */

voidstar malloc ();
voidstar realloc ();
char *getenv ();
double atof ();
long strtol ();

#endif /* STDC_HEADERS */

/* Some systems do not define EXIT_*, even with STDC_HEADERS.  */
#ifndef EXIT_SUCCESS
# define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
# define EXIT_FAILURE 1
#endif

#include <errno.h>
#ifndef errno
extern int errno;
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

/* If FALSE is defined, we presume TRUE is defined too.  In this case,
   merely typedef boolean as being int.  Or else, define these all.  */
#ifndef FALSE
# define FALSE 0
# define TRUE 1
#endif
typedef int boolean;

char *mktemp ();

#ifndef __P
# ifdef PROTOTYPES
#  define __P(Args) Args
# else
#  define __P(Args) ()
# endif
#endif

#if HAVE_LOCALE_H
# include <locale.h>
#else
# define setlocale(Category, Locale)
#endif

#ifdef ENABLE_NLS
#include <libintl.h>
#define _(Text) gettext ((Text))
#else
#define _(Text) (Text)
#endif

/*  Last character of a string.  */
#define LAST_CHAR(Text) *(Text + strlen (Text) - 1)


/* Various declarations.  */

struct _string
  {
    char *string;               /* characters of the string */
    size_t length;              /* length of the string */
  };
typedef struct _string STRING;

/* Those must come first.  */
typedef void builtin_func ();

/* Various different token types.  */
enum _token_type
{
  TOKEN_EOF,                    /* end of file */
  TOKEN_NONE,                   /* discardable token */
  TOKEN_STRING,                 /* a string */
  TOKEN_QUOTED,                 /* a quoted string */
  TOKEN_QUOTE,                  /* begin delimiter of a quoted string
                                   to expand */
  TOKEN_BGROUP,                 /* begin group */
  TOKEN_EGROUP,                 /* end group */
  TOKEN_SPACE,                  /* whitespace */
  TOKEN_WORD,                   /* an identifier */
  TOKEN_ENTITY,                 /* an entity */
  TOKEN_SIMPLE,                 /* a single character */
  TOKEN_MACDEF                  /* a macros definition (see "defn") */
};
typedef enum _token_type token_type;

/* The data for a token, a macro argument, and a macro definition.  */
enum _token_data_type
{
  TOKEN_VOID,
  TOKEN_TEXT,
  TOKEN_FUNC
};
typedef enum _token_data_type token_data_type;

struct _token_data
{
  token_data_type type;
  union
    {
      struct
        {
          char *text;
        }
      u_t;
      struct
        {
          builtin_func *func;
          boolean traced;
        }
      u_f;
    }
  u;
};
typedef struct _token_data token_data;

/* Memory allocation.  */
voidstar xmalloc __P ((size_t));
voidstar xcalloc __P ((size_t, size_t));
voidstar xrealloc __P ((voidstar , size_t));
void xfree __P ((voidstar));
char *xstrdup __P ((const char *));
#define obstack_chunk_alloc     xmalloc
#define obstack_chunk_free      xfree

/* Other library routines.  */
void error __P ((int, int, const char *, ...));


/* File: mp4h.c  --- global definitions.  */

/* Option flags.  */
extern int interactive;                 /* -e */
extern int sync_output;                 /* -s */
extern int debug_level;                 /* -d */
extern int hash_table_size;             /* -H */
extern int max_debug_argument_length;   /* -l */
extern int suppress_warnings;           /* -Q */
extern int warning_status;              /* -E */
extern int nesting_limit;               /* -L */
extern int frozen_dump;                 /* -F */


/* Error handling.  */
#define MP4HERROR(Arglist) (error Arglist)


/* File: debug.c  --- debugging and tracing function.  */

extern FILE *debug;

/* The value of debug_level is a bitmask of the following.  */

/* a: show arglist in trace output */
#define DEBUG_TRACE_ARGS 1
/* e: show expansion in trace output */
#define DEBUG_TRACE_EXPANSION 2
/* t: trace all macros -- overrides trace{on,off} */
#define DEBUG_TRACE_ALL 8
/* l: add line numbers to trace output */
#define DEBUG_TRACE_LINE 16
/* f: add file name to trace output */
#define DEBUG_TRACE_FILE 32
/* p: trace path search of include files */
#define DEBUG_TRACE_PATH 64
/* c: show macro call before args collection */
#define DEBUG_TRACE_CALL 128
/* i: trace changes of input files */
#define DEBUG_TRACE_INPUT 256
/* x: add call id to trace output */
#define DEBUG_TRACE_CALLID 512
/* m: trace module loading */
#define DEBUG_TRACE_MODULES 1024

/* V: very verbose --  print everything */
#define DEBUG_TRACE_VERBOSE 1023
/* default flags -- equiv: ae */
#define DEBUG_TRACE_DEFAULT 3

#define DEBUG_PRINT1(Fmt, Arg1) \
  do                                                            \
    {                                                           \
      if (debug != NULL)                                        \
        fprintf (debug, Fmt, Arg1);                             \
    }                                                           \
  while (0)

#define DEBUG_PRINT3(Fmt, Arg1, Arg2, Arg3) \
  do                                                            \
    {                                                           \
      if (debug != NULL)                                        \
        fprintf (debug, Fmt, Arg1, Arg2, Arg3);                 \
    }                                                           \
  while (0)

#define DEBUG_MESSAGE(Fmt) \
  do                                                            \
    {                                                           \
      if (debug != NULL)                                        \
        {                                                       \
          debug_message_prefix ();                              \
          fprintf (debug, Fmt);                                 \
          putc ('\n', debug);                                   \
        }                                                       \
    }                                                           \
  while (0)

#define DEBUG_MESSAGE1(Fmt, Arg1) \
  do                                                            \
    {                                                           \
      if (debug != NULL)                                        \
        {                                                       \
          debug_message_prefix ();                              \
          fprintf (debug, Fmt, Arg1);                           \
          putc ('\n', debug);                                   \
        }                                                       \
    }                                                           \
  while (0)

#define DEBUG_MESSAGE2(Fmt, Arg1, Arg2) \
  do                                                            \
    {                                                           \
      if (debug != NULL)                                        \
        {                                                       \
          debug_message_prefix ();                              \
          fprintf (debug, Fmt, Arg1, Arg2);                     \
          putc ('\n', debug);                                   \
        }                                                       \
    }                                                           \
  while (0)

void debug_init __P ((void));
void debug_deallocate __P ((void));
int debug_decode __P ((const char *));
void debug_flush_files __P ((void));
boolean debug_set_output __P ((const char *));
void debug_message_prefix __P ((void));

void trace_prepre __P ((const char *, int));
void trace_pre __P ((const char *, int, int, token_data **));
void trace_post __P ((const char *, int, int, token_data **, const char *));


/* File: input.c  --- lexical definitions.  */

#define TOKEN_DATA_TYPE(Td)         ((Td)->type)
#define TOKEN_DATA_TEXT(Td)         ((Td)->u.u_t.text)
#define TOKEN_DATA_FUNC(Td)         ((Td)->u.u_f.func)
#define TOKEN_DATA_FUNC_TRACED(Td)  ((Td)->u.u_f.traced)

/* The status of processing. */
#define READ_NORMAL    (1 << 0)  /* normal expansion of macros */
#define READ_ATTRIBUTE (1 << 1)  /* when reading macro arguments */
#define READ_ATTR_QUOT (1 << 2)  /* like READ_ATTRIBUTE, but quotes
                                    are preserved */
#define READ_ATTR_VERB (1 << 3)  /* inside macros with attributes=verbatim */
#define READ_ATTR_ASIS (1 << 4)  /* attributes are read without any
           modification, main difference with READ_ATTR_VERB is that quotes
           and backslashes are not removed and are part of this attribute  */
#define READ_BODY      (1 << 5)  /* when reading body function */

/* Flags which determine how expansion is done  */
#define EXP_NO_HTMLTAG   (1 << 0)  /* do not parse unknown tags */
#define EXP_DFT_SIMPLE   (1 << 1)  /* HTML tags are simple */
#define EXP_STAR_COMPLEX (1 << 2)  /* HTML tags whose last char is an asterisk
                                      are by default simple tags, they become
                                      complex when this flag is set.  */
#define EXP_UNM_BREAK    (1 << 3)  /* An unmatched end tag closes all previous
                                      unmatched begin tags.  */
#define EXP_STD_BSLASH   (1 << 4)  /* By default, only 'n', 'r', 't', '"' and
                                      '\\' are escaped.  When this flag is
				      set, backslashes are interpreted as in
				      printf.  */
#define EXP_REMOVE_TRAILING_SLASH \
                         (1 << 5)  /* Remove trailing slash in simple tag attributes */
#define EXP_LEAVE_TRAILING_STAR \
                         (1 << 6)  /* Do not remove trailing slash in simple tag attributes */
#define EXP_LEAVE_LEADING_STAR \
                         (1 << 7)  /* Do not remove trailing slash in simple tag attributes */
#define EXP_NOSPACE_BSLASH \
                         (1 << 8)  /* Do not add space before trailing slash in simple tag attributes */
#define EXP_NOWARN_NEST  (1 << 10) /* Suppress warning about bad nested tags */
#define EXP_NOWARN_SLASH (1 << 11) /* Suppress warning about missing trailing slash */

extern int exp_flags;

typedef int read_type;

void input_init __P ((void));
void input_deallocate __P ((void));
void syntax_init __P ((void));
int peek_input __P ((void));
token_type next_token __P ((token_data *, read_type, boolean));
void skip_line __P ((void));
void skip_buffer __P ((void));
void input_close __P ((void));

/* push back input */
void push_file __P ((FILE *, const char *));
void push_macro __P ((builtin_func *, boolean));
void push_single __P ((int));
struct obstack *push_string_init __P ((void));
const char *push_string_finish __P ((read_type));
void push_wrapup __P ((const char *));
boolean pop_wrapup __P ((void));
void unget_string __P ((char *));

/* read a file verbatim */
void read_file_verbatim __P ((struct obstack *));

/* current input file, and line */
extern char *current_file;
extern int current_line;
extern char **array_current_file;
extern int *array_current_line;
#define CURRENT_FILE_LINE                                               \
        (expansion_level == 0 ? current_file :                          \
                                array_current_file[expansion_level]),   \
        (expansion_level == 0 ? current_line :                          \
                                array_current_line[expansion_level])

/* Begin and end quote */
extern STRING lquote, rquote;

/* Eof-of-line comment  */
extern STRING eolcomm;

/* Special characters used for grouping  */
#define CHAR_LQUOTE '\1'
#define CHAR_RQUOTE '\2'
#define CHAR_BGROUP '\3'
#define CHAR_EGROUP '\4'

/* Some characters are replaced during input/output phases */
#define CHAR_QUOTE  '\5'
#define CHAR_SLASH  '\6'

/* Default eof-of-line comment  */
#define DEF_EOLCOMM  ";;;"

/* Default quotes  */
#define DEF_LQUOTE  "<@["
#define DEF_RQUOTE  "]@>"

/* Syntax table definitions. */
/* Please read the comment at the top of input.c for details */
extern unsigned short syntax_table[256];

/* These are simple values, not bit masks.  There is no overlap. */
#define SYNTAX_OTHER    (0x0000)

#define SYNTAX_IGNORE   (0x0001)
#define SYNTAX_SPACE    (0x0002)
#define SYNTAX_GROUP    (0x0009)

/* These are values to be assigned to syntax table entries, but they are
   used as bit masks with IS_ALNUM.*/
#define SYNTAX_ALPHA    (0x0010)
#define SYNTAX_NUM      (0x0020)
#define SYNTAX_ALNUM    (SYNTAX_ALPHA|SYNTAX_NUM)

/* These bits define the syntax code of a character */
#define SYNTAX_VALUE    (0x00FF)
#define SYNTAX_MASKS    (0xFF00)

#define IS_OTHER(ch)  ((syntax_table[(int)(ch)]&SYNTAX_VALUE) == SYNTAX_OTHER)
#define IS_IGNORE(ch) ((syntax_table[(int)(ch)]) == SYNTAX_IGNORE)
#define IS_SPACE(ch)  ((syntax_table[(int)(ch)]&SYNTAX_VALUE) == SYNTAX_SPACE)
#define IS_ALPHA(ch)  ((syntax_table[(int)(ch)]&SYNTAX_VALUE) == SYNTAX_ALPHA)
#define IS_NUM(ch)    ((syntax_table[(int)(ch)]&SYNTAX_VALUE) == SYNTAX_NUM)
#define IS_ALNUM(ch)  ((((syntax_table[(int)(ch)]) & SYNTAX_ALNUM) != 0) \
                          || ch == ':' || ch == '-')

#define IS_BGROUP(ch) (ch == CHAR_BGROUP)
#define IS_EGROUP(ch) (ch == CHAR_EGROUP)
#define IS_LQUOTE(ch) (ch == CHAR_LQUOTE)
#define IS_RQUOTE(ch) (ch == CHAR_RQUOTE)
#define IS_GROUP(ch)  ((syntax_table[(int)(ch)]&SYNTAX_VALUE) == SYNTAX_GROUP)
#define IS_SLASH(ch)  (ch == CHAR_SLASH || ch == '/')
#define IS_TAG(ch)    (ch == '<')
#define IS_CLOSE(ch)  (ch == '>')
#define IS_ENTITY(ch) (ch == '&')

void set_syntax __P ((int, const char *));
void set_syntax_internal __P ((int, int));
void unset_syntax_attribute __P ((int, int));


/* File: output.c --- output functions.  */
extern int current_diversion;
extern int output_current_line;

void output_init __P ((void));
void output_deallocate __P ((void));
void shipout_text __P ((struct obstack *, char *));
void make_diversion __P ((int));
void insert_diversion __P ((int));
void insert_file __P ((FILE *));
void freeze_diversions __P ((FILE *));
void remove_special_chars __P ((char *, boolean));


/* File symtab.c  --- symbol table definitions.  */

/*  Default case sensitiveness  */
#define CASELESS_DEFAULT 3

/* Operation modes for lookup_symbol ().  */
enum _symbol_lookup
{
  SYMBOL_LOOKUP,
  SYMBOL_INSERT,
  SYMBOL_DELETE
};

/* Symbol table entry.  */
struct _symbol
{
  struct _symbol *next;
  boolean traced;
  boolean container;
  boolean expand_args;

  char *name;
  char *hook_begin;
  char *hook_end;
  token_data data;
};

#define SYMBOL_NEXT(S)          ((S)->next)
#define SYMBOL_TRACED(S)        ((S)->traced)
#define SYMBOL_CONTAINER(S)     ((S)->container)
#define SYMBOL_EXPAND_ARGS(S)   ((S)->expand_args)
#define SYMBOL_NAME(S)          ((S)->name)
#define SYMBOL_HOOK_BEGIN(S)    ((S)->hook_begin)
#define SYMBOL_HOOK_END(S)      ((S)->hook_end)
#define SYMBOL_TYPE(S)          (TOKEN_DATA_TYPE (&(S)->data))
#define SYMBOL_TEXT(S)          (TOKEN_DATA_TEXT (&(S)->data))
#define SYMBOL_FUNC(S)          (TOKEN_DATA_FUNC (&(S)->data))

typedef enum _symbol_lookup symbol_lookup;
typedef struct _symbol symbol;
typedef void hack_symbol ();

#define HASHMAX 509             /* default, overridden by -Hsize */

extern symbol **sym_tab;
extern symbol **var_tab;
extern symbol **file_tab;
extern symbol **symtab;

void symtab_init __P ((void));
void symtab_deallocate __P ((void));
void caseless_init __P ((int));
symbol *lookup_symbol __P ((const char *, symbol_lookup));
symbol *lookup_entity __P ((const char *, symbol_lookup));
symbol *lookup_variable __P ((const char *, symbol_lookup));
symbol *lookup_file __P ((const char *, symbol_lookup));
void hack_all_symbols __P ((hack_symbol *, const char *));


/* File: macro.c  --- macro expansion.  */

void expand_input __P ((void));
void call_macro __P ((symbol *, struct obstack *, int, token_data **, read_type));
boolean get_attribute (struct obstack *obs, token_data *argp);

extern int expansion_level;


/* File: builtin.c  --- builtins.  */

enum _encoding_type
{
  ENCODING_8BIT,                /* 1-byte char */
  ENCODING_UTF8                 /* UTF-8 */
};
typedef enum _encoding_type encoding_type;

struct _builtin
{
  const char *name;
  boolean container;
  boolean expand_args;
  builtin_func *func;
};

typedef struct _builtin builtin;

extern boolean visible_quotes;

/* Used to disable risky functions. */
extern int safety_level;

/* Document encoding */
extern encoding_type document_encoding;

void locale_init __P ((int, char *));
void pcre_init __P ((void));
void pcre_deallocate __P ((void));
void initialize_builtin __P ((symbol *));
void builtin_init __P ((void));
void builtin_deallocate __P ((void));
void clear_tag_attr __P ((void));
void define_builtin __P ((const char *, const builtin *, boolean));
void break_init __P ((void));
void break_deallocate __P ((void));
void define_user_macro __P ((const char *, char *, symbol_lookup,
                             boolean, boolean, boolean));
void undivert_all __P ((void));
void expand_user_macro __P ((struct obstack *, symbol *, int, token_data **,
                             read_type));

const builtin *find_builtin_by_addr __P ((builtin_func *));
const builtin *find_builtin_by_name __P ((const char *));
void install_builtin_table __P ((builtin *));


/* File: devel.c  --- global functions for writing builtins and modules.  */

boolean bad_argc __P ((token_data *, int, int, int));
boolean numeric_arg __P ((token_data *, const char *, boolean, int *));
void shipout_int __P ((struct obstack *, int));
void shipout_long __P ((struct obstack *, long));
void shipout_string __P ((struct obstack *, const char *, int));
void dump_args __P ((struct obstack *, int, token_data **, const char *));
const char * predefined_attribute __P ((const char *, int *, token_data **, boolean));


/* File: path.c  --- path search for include files.  */

void include_init __P ((void));
void include_env_init __P ((void));
void include_deallocate __P ((void));
void add_include_directory __P ((const char *));
FILE *path_search __P ((const char *, char **));

/* These are for other search paths */

struct search_path
{
  struct search_path *next;     /* next directory to search */
  const char *dir;              /* directory */
  int len;
};

typedef struct search_path search_path;

struct search_path_info
{
  search_path *list;            /* the list of path directories */
  search_path *list_end;        /* the end of same */
  search_path *sys;             /* system path directories */
  search_path *sys_end;         /* the end of same */
  int max_length;               /* length of longest directory name */
};



/* File: eval.c  --- expression evaluation.  */

boolean evaluate __P ((struct obstack *obs,
                       const char *, const int radix, int min));

#ifdef WITH_GMP
boolean mp_evaluate __P ((struct obstack *obs,
                          const char *, const int radix, int min));
#endif /* WITH_GMP */


/* File: format.c  --- printf like formatting.  */

void format __P ((struct obstack *, int, token_data **));


/* File: freeze.c --- frozen state files.  */

void produce_frozen_state __P ((const char *));
void reload_frozen_state __P ((const char *));



/* File: module.c --- dynamic modules */

#if defined(WITH_MODULES) || defined(MP4H_MODULE)

typedef void module_init_t __P ((struct obstack *));
typedef void module_finish_t __P ((void));

typedef voidstar module_func __P ((const char *));

void module_init __P ((void));
void library_load __P ((const char *, struct obstack *));
void module_load __P ((const char *, struct obstack *));
void module_unload_all __P ((void));

#endif


/* Debugging the memory allocator.  */

#ifdef WITH_DMALLOC
# define DMALLOC_FUNC_CHECK
# include <dmalloc.h>
#endif

/* Other debug stuff.  */

#ifdef DEBUG
# define DEBUG_INPUT
# define DEBUG_MACRO
# define DEBUG_SYM
# define DEBUG_INCL
#endif


/*  Stuff for compiling builtins and loadable modules.  */

#ifdef MP4H_MODULE

#define MP4H_BUILTIN_ARGS struct obstack *obs, int argc, token_data **argv, \
                            read_type expansion
#define MP4H_BUILTIN_PROTO struct obstack *, int, token_data **, read_type
#define MP4H_BUILTIN_RECUR obs, argc, argv, expansion

#define DECLARE(name) \
  static void name __P ((MP4H_BUILTIN_PROTO))

#define ARG(i)  (i<argc ? TOKEN_DATA_TEXT (argv[i]) : "")
#define ARGBODY (TOKEN_DATA_TEXT (argv[argc]))

enum _mathop_type
{
  MATHOP_ADD,                   /* addition */
  MATHOP_SUB,                   /* substraction */
  MATHOP_MUL,                   /* multiplication */
  MATHOP_DIV,                   /* division */
  MATHOP_MIN,                   /* minimum */
  MATHOP_MAX,                   /* maximum */
  MATHOP_MOD                    /* modulus */
};

enum _mathrel_type
{
  MATHREL_GT,
  MATHREL_LT,
  MATHREL_EQ,
  MATHREL_NEQ 
};

typedef enum _mathop_type mathop_type;
typedef enum _mathrel_type mathrel_type;

typedef struct var_stack var_stack;
struct var_stack
{
    var_stack *prev;
    char *text;
};

#endif

#endif /* MP4H_H */
