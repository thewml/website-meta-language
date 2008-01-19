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

/* Handling of different input sources, and lexical analysis.  */

#include "mp4h.h"

#ifdef DEBUG_INPUT
static int print_token __P ((const char *, token_type, token_data *));
static void lex_debug __P ((void));
#endif

/* 
   Unread input can be either files, that should be read (eg. included
   files), strings, which should be rescanned (eg. macro expansion
   text), single characters or quoted macro definitions (as returned by
   the builtin "function-def").  Unread input are organised in a stack,
   implemented with an obstack.  Each input source is described by a
   "struct input_block".  The obstack is "input_stack".  The top of the
   input stack is "isp".

   Each input_block has an associated struct input_funcs, that defines
   functions for peeking, reading, unget and cleanup.  All input is done
   through the functions pointers of the input_funcs of the top most
   input_block.  When a input_block is exhausted, its reader returns
   CHAR_RETRY which causes the input_block to be popped from the
   input_stack.
   
   Pushing new input on the input stack is done by push_file (),
   push_string (), push_single () or push_macro () (for macro definitions).
   Because macro expansion needs direct access to the current input
   obstack (for optimization), push_string () are split in two functions,
   push_string_init (), which returns a pointer to the current input stack,
   and push_string_finish (), which return a pointer to the final text.
   The input_block *next is used to manage the coordination between the
   different push routines.
   
   The current file and line number are stored in two global variables,
   for use by the error handling functions in mp4h.c.  Whenever a file
   input_block is pushed, the current file name and line number is saved
   in the input_block, and the two variables are reset to match the new
   input file.

   THE SYNTAX TABLE

   The input is read character by character and grouped together
   according to a syntax table.  The character groups are (definitions
   are all in mp4h.h:

   SYNTAX_IGNORE        *Character to be deleted from input as if not present
   SYNTAX_OTHER         Any character with no special meaning to mp4h
   SYNTAX_SPACE         Whitespace (ignored when leading macro arguments)

   SYNTAX_ALPHA         Alphabetic characters (can start macro names)
   SYNTAX_NUM           Numeric characters
   SYNTAX_ALNUM         Alphanumeric characters (can form macro names)

   Besides adding new facilities, the use of a syntax table will reduce
   the number of calls to next_token ().  Now groups of OTHER, NUM and
   SPACE characters can be returned as a single token, since next_token
   () knows they have no special syntactical meaning to m4.  This is,
   however, only possible if only single character quotes comments
   comments are used, because otherwise the quote and comment characters
   will not show up in the syntax-table.

   The precedence as implemented by next_token () is:

   SYNTAX_IGNORE        *Filtered out below next_token ()
   SYNTAX_ALPHA         Reads macro name

   SYNTAX_OTHER and SYNTAX_NUM
                        Reads all SYNTAX_OTHER and SYNTAX_NUM 
   SYNTAX_SPACE         Reads all SYNTAX_SPACE
   the rest             Returned as a single char

   SYNTAX_DOLLAR is not currently used.  The character $ is treated as a
   SYNTAX_OTHER.  It could be done, but it will slow next_token () down
   a bit.  The $ is not really a part of m4's input syntax in the sense
   that a string is parsed equally whether there is a $ or not.  The
   character $ is used by convention in user macros.  */

struct input_funcs
{
  int (*peek_func)(void);       /* function to peek input */
  int (*read_func)(void);       /* function to read input */
  void (*unget_func)(int);      /* function to unread input */
  void (*clean_func)(void);     /* function to clean up */
};

struct input_block
{
  struct input_block *prev;     /* previous input_block on the input stack */
  struct input_funcs *funcs;    /* functions on this input_block */

  union
    {
      struct
        {
          unsigned int ch;      /* single char value */
        }
      u_c;
      struct
        {
          unsigned char *start;   /* string value */
          unsigned char *current; /* current value */
        }
      u_s;
      struct
        {
          FILE *file;           /* input file handle */
          char *name;           /* name of PREVIOUS input file */
          int lineno;           /* current line number for do */
          /* Yet another attack of "The curse of global variables" (sigh) */
          int out_lineno;       /* current output line number do */
          boolean advance_line; /* start_of_input_line from next_char () */
        }
      u_f;
      struct
        {
          builtin_func *func;   /* pointer to macros function */
          boolean traced;       /* TRUE iff builtin is traced */
          boolean read;         /* TRUE iff block has been read */
        }
      u_m;
    }
  u;
};

typedef struct input_block input_block;


/* Current input file name.  */
char *current_file;
char **array_current_file;

/* Current input line number.  */
int current_line;
int *array_current_line;

/* Obstack for storing individual tokens.  */
static struct obstack token_stack;

/* Normal input stack.  */
static struct obstack input_stack;

/* Wrapup input stack.  */
static struct obstack wrapup_stack;

/* Input or wrapup.  */
static struct obstack *current_input;

/* Bottom of token_stack, for obstack_free.  */
static char *token_bottom;

/* Pointer to top of current_input.  */
static input_block *isp;

/* Pointer to top of wrapup stack.  */
static input_block *wsp;

/* Aux. for handling split push_string ().  */
static input_block *next;

/* Global */
static token_data token_read;

/* Flag for next_char () to increment current_line.  */
static boolean start_of_input_line;

/* Input syntax table */
unsigned short syntax_table[256];

#define CHAR_EOF        256     /* character return on EOF */
#define CHAR_MACRO      257     /* character return for MACRO token */
#define CHAR_RETRY      258     /* character return for end of input block */

/* Comment chars.  */
STRING eolcomm;
STRING lquote, rquote;

boolean visible_quotes;



/*---------------------------------------------------------------------.
| push_file () pushes an input file on the input stack, saving the     |
| current file name and line number.  If next is non-NULL, this push   |
| invalidates a call to push_string_init (), whose storage are         |
| consequentely released.                                              |
|                                                                      |
| file_read () manages line numbers for error messages, so they do not |
| get wrong, due to lookahead.  The token consisting of a newline      |
| alone is taken as belonging to the line it ends, and the current     |
| line number is not incremented until the next character is read.     |
`---------------------------------------------------------------------*/

static int
file_peek(void)
{
  int ch;

  ch = getc (isp->u.u_f.file);
  if (ch == EOF)
    return CHAR_RETRY;

  ungetc (ch, isp->u.u_f.file);
  return ch;
}

static int
file_read(void)
{
  int ch;

  if (start_of_input_line)
    {
      start_of_input_line = FALSE;
      current_line++;
    }

  ch = getc (isp->u.u_f.file);
  if (ch == EOF)
    return CHAR_RETRY;

  if (ch == '\n')
    start_of_input_line = TRUE;
  return ch;
}

static void
file_unget(int ch)
{
  ungetc (ch, isp->u.u_f.file);
  if (ch == '\n')
    start_of_input_line = FALSE;
}

static void
file_clean(void)
{
  if (debug_level & DEBUG_TRACE_INPUT)
    DEBUG_MESSAGE2 ("Input reverted to %s, line %d",
                    isp->u.u_f.name, isp->u.u_f.lineno);

  fclose (isp->u.u_f.file);
  /*  A common mistake is to forget closing some complex tags, which
      result in an error `file: EOF when reading body of...'
      If file name has been changed by <__file__>, this name
      should appear in error message instead of real file name.  */
  if (isp->prev != NULL)
    {
      xfree ((voidstar) current_file);
      current_file = xstrdup (isp->u.u_f.name);
    }
  xfree ((voidstar) isp->u.u_f.name);
  current_line = isp->u.u_f.lineno;
  output_current_line = isp->u.u_f.out_lineno;
  start_of_input_line = isp->u.u_f.advance_line;
  if (isp->prev != NULL)
    output_current_line = -1;
}

static struct input_funcs file_funcs = {
  file_peek, file_read, file_unget, file_clean
};

void
push_file (FILE *fp, const char *title)
{
  input_block *i;

  if (next != NULL)
    {
      obstack_free (current_input, next);
      next = NULL;
    }

  if (debug_level & DEBUG_TRACE_INPUT)
    DEBUG_MESSAGE1 ("Input read from %s", title);

  i = (input_block *) obstack_alloc (current_input,
                                     sizeof (struct input_block));
  i->funcs = &file_funcs;

  i->u.u_f.file = fp;
  i->u.u_f.name = current_file;
  i->u.u_f.lineno = current_line;
  i->u.u_f.out_lineno = output_current_line;
  i->u.u_f.advance_line = start_of_input_line;

  current_file = xstrdup (obstack_copy0 (current_input, title, strlen (title)));
  current_line = 1;
  output_current_line = -1;

  i->prev = isp;
  isp = i;
}

/*-------------------------------------------------------------------------.
| push_macro () pushes a builtin macros definition on the input stack.  If |
| next is non-NULL, this push invalidates a call to push_string_init (),   |
| whose storage are consequentely released.                                |
`-------------------------------------------------------------------------*/

static int
macro_peek(void)
{
  if (isp->u.u_m.read == TRUE)
    return CHAR_RETRY;

  return CHAR_MACRO;
}

static int
macro_read(void)
{
  if (isp->u.u_m.read == TRUE)
    return CHAR_RETRY;

  isp->u.u_m.read = TRUE;
  return CHAR_MACRO;
}

static struct input_funcs macro_funcs = {
  macro_peek, macro_read, NULL, NULL
};

void
push_macro (builtin_func *func, boolean traced)
{
  input_block *i;

  if (next != NULL)
    {
      obstack_free (current_input, next);
      next = NULL;
    }

  i = (input_block *) obstack_alloc (current_input,
                                     sizeof (struct input_block));
  i->funcs = &macro_funcs;

  i->u.u_m.func = func;
  i->u.u_m.traced = traced;
  i->u.u_m.read = FALSE;

  i->prev = isp;
  isp = i;
}

/*------------------------------------------------.
| * Push a single character on to the input stack |
`------------------------------------------------*/

static int
single_peek(void)
{
  return isp->u.u_c.ch;
}

static int
single_read(void)
{
  int ch = isp->u.u_c.ch;

  if (ch != CHAR_RETRY)
    isp->u.u_c.ch = CHAR_RETRY;

  return ch;
}

static struct input_funcs single_funcs = {
  single_peek, single_read, NULL, NULL
};

void
push_single (int ch)
{
  input_block *i;

  if (next != NULL)
    {
      obstack_free (current_input, next);
      next = NULL;
    }

  i = (input_block *) obstack_alloc (current_input,
                                     sizeof (struct input_block));

  i->funcs = &single_funcs;

  i->u.u_c.ch = ch;

  i->prev = isp;
  isp = i;
}

/*------------------------------------------------------------------.
| First half of push_string ().  The pointer next points to the new |
| input_block.                                                      |
`------------------------------------------------------------------*/

static int
string_peek(void)
{
  int ch = *isp->u.u_s.current;

  return (ch == '\0') ? CHAR_RETRY : ch;
}

static int
string_read(void)
{
  int ch = *isp->u.u_s.current++;

  return (ch == '\0') ? CHAR_RETRY : ch;
}

static void
string_unget(int ch)
{
  if (isp->u.u_s.current > isp->u.u_s.start)
    *--isp->u.u_s.current = ch;
  else
    push_single(ch);
}

static struct input_funcs string_funcs = {
  string_peek, string_read, string_unget, NULL
};

struct obstack *
push_string_init (void)
{
  if (next != NULL)
    {
      MP4HERROR ((warning_status, 0,
        "INTERNAL ERROR: Recursive push_string!"));
      exit (1);
    }

  next = (input_block *) obstack_alloc (current_input,
                                        sizeof (struct input_block));
  next->funcs = &string_funcs;

  return current_input;
}

/*------------------------------------------------------------------------.
| Last half of push_string ().  If next is now NULL, a call to push_file  |
| () has invalidated the previous call to push_string_init (), so we just |
| give up.  If the new object is void, we do not push it.  The function   |
| push_string_finish () returns a pointer to the finished object.  This   |
| pointer is only for temporary use, since reading the next token might   |
| release the memory used for the object.                                 |
|                                                                         |
`------------------------------------------------------------------------*/

const char *
push_string_finish (read_type expansion)
{
  const char *ret = NULL;

  if (next == NULL)
    return NULL;

  if (obstack_object_size (current_input) > 0)
    {
      obstack_1grow (current_input, '\0');
      next->u.u_s.start = (unsigned char *) obstack_finish (current_input);
      if (expansion == READ_ATTR_VERB || expansion == READ_ATTR_ASIS
          || expansion == READ_BODY)
        {
          TOKEN_DATA_TYPE (&token_read) = TOKEN_TEXT;
          TOKEN_DATA_TEXT (&token_read) = xstrdup ((char *) next->u.u_s.start);
          next->u.u_s.current = next->u.u_s.start + strlen ((char *) next->u.u_s.start);
        }
      else
        next->u.u_s.current = next->u.u_s.start;

      next->prev = isp;
      isp = next;
      ret = (char *) isp->u.u_s.start;   /* for immediate use only */
    }
  else
    obstack_free (current_input, next); /* people might leave garbage on it. */
  next = NULL;
  return ret;
}


/*--------------------------------------------------------------------------.
| The function push_wrapup () pushes a string on the wrapup stack.  When    |
| he normal input stack gets empty, the wrapup stack will become the input  |
| stack, and push_string () and push_file () will operate on wrapup_stack.  |
| Push_wrapup should be done as push_string (), but this will suffice, as   |
| long as arguments to m4_m4wrap () are moderate in size.                   |
`--------------------------------------------------------------------------*/
void
push_wrapup (const char *s)
{
  input_block *i = (input_block *) obstack_alloc (&wrapup_stack,
                                                  sizeof (struct input_block));
  i->prev = wsp;

  i->funcs = &string_funcs;

  i->u.u_s.start = (unsigned char *) obstack_copy0 (&wrapup_stack, s, strlen (s));
  i->u.u_s.current = i->u.u_s.start;

  wsp = i;
}



/*-------------------------------------------------------------------------.
| The function pop_input () pops one level of input sources.  If the       |
| popped input_block is a file, current_file and current_line are reset to |
| the saved values before the memory for the input_block are released.     |
`-------------------------------------------------------------------------*/

static void
pop_input (void)
{
  input_block *tmp = isp->prev;

  if (isp->funcs->clean_func != NULL)
    (*isp->funcs->clean_func)();

  obstack_free (current_input, isp);
  next = NULL;                  /* might be set in push_string_init () */

  isp = tmp;
}

/*------------------------------------------------------------------------.
| To switch input over to the wrapup stack, main () calls pop_wrapup ().  |
| Since wrapup text can install new wrapup text, pop_wrapup () returns    |
| FALSE when there is no wrapup text on the stack, and TRUE otherwise.    |
`------------------------------------------------------------------------*/

boolean
pop_wrapup (void)
{
  if (wsp == NULL)
    return FALSE;

  current_input = &wrapup_stack;
  isp = wsp;
  wsp = NULL;

  return TRUE;
}

void
input_close(void)
{
  do
    pop_input ();
  while (isp);
}

/*-------------------------------------------------------------------.
| When a MACRO token is seen, next_token () uses init_macro_token () |
| to retrieve the value of the function pointer.                     |
`-------------------------------------------------------------------*/

static void
init_macro_token (token_data *td)
{
  if (isp->funcs->read_func != macro_read)
    {
      MP4HERROR ((warning_status, 0,
        "INTERNAL ERROR: Bad call to init_macro_token ()"));
      exit (1);
    }

  TOKEN_DATA_TYPE (td) = TOKEN_FUNC;
  TOKEN_DATA_FUNC (td) = isp->u.u_m.func;
  TOKEN_DATA_FUNC_TRACED (td) = isp->u.u_m.traced;

}


/*---------------------------------------------------------------.
| Low level input is done a character at a time.  The function   |
| next_char () is used to read and advance the input to the next |
| character.                                                     |
`---------------------------------------------------------------*/

static int
next_char (void)
{
  int ch;
  int (*f)(void);

  while (1)
    {
      if (isp == NULL)
        return CHAR_EOF;

      f = isp->funcs->read_func;
      if (f != NULL)
        {
          while ((ch = (*f)()) != CHAR_RETRY)
            {
              /* if (!IS_IGNORE(ch)) */
                return ch;
            }
        }
      else
        {
          MP4HERROR ((warning_status, 0,
            "INTERNAL ERROR: Input stack botch in next_char ()"));
          exit (1);
        }

      /* End of input source --- pop one level.  */
      pop_input ();
    }
}

/*--------------------------------------------------------------------.
| The function peek_input () is used to look at the next character in |
| the input stream.  At any given time, it reads from the input_block |
| on the top of the current input stack.                              |
`--------------------------------------------------------------------*/

int
peek_input (void)
{
  int ch;
  int (*f)(void);

  while (1)
    {
      if (isp == NULL)
        return CHAR_EOF;

      f = isp->funcs->peek_func;
      if (f != NULL)
        {
          if ((ch = (*f)()) != CHAR_RETRY)
            {
              return /* (IS_IGNORE(ch)) ? next_char () : */ ch;
            }
        }
      else
        {
          MP4HERROR ((warning_status, 0,
            "INTERNAL ERROR: Input stack botch in peek_input ()"));
          exit (1);
        }

      /* End of input source --- pop one level.  */
      pop_input ();
    }
}

/*---------------------------------------------------------------.
| The function unget_input () puts back a character on the input |
| stack, using an existing input_block if possible               |
`---------------------------------------------------------------*/

static void
unget_input (int ch)
{
  if (ch == CHAR_EOF)
    return;
  if (isp != NULL && isp->funcs->unget_func != NULL)
    (*isp->funcs->unget_func)(ch);
  else
    push_single(ch);
}

/*---------------------------------------------------------------.
| The function unget_string () puts back a string on the input   |
| stack, using the unget_input () subroutine.  This function is  |
| global because it is called by collect_body ().                |
`---------------------------------------------------------------*/

void
unget_string (char *text)
{
  char *cp;

  for (cp=text + strlen (text) - 1; cp>=text; cp--)
    unget_input (*cp);
}

/*------------------------------------------------------------------------.
| skip_line () simply discards all immediately following characters, upto |
| the first newline.  All whitespaces following this newline are also     |
| discarded.                                                              |
`------------------------------------------------------------------------*/

void
skip_line (void)
{
  int ch;

  while ((ch = next_char ()) != CHAR_EOF && ch != '\n')
    ;
  while ((ch = next_char ()) != CHAR_EOF && (ch == '\t' || ch == ' '))
    ;
  unget_input (ch);
}

/*------------------------------------------------------------------------.
| skip_buffer () discards current buffer.  This function is called by     |
| <return> to exit current function.                                      |
`------------------------------------------------------------------------*/

void
skip_buffer (void)
{
  if (isp == NULL)
    return;

  pop_input ();
}


/*---------------------------------------------------------------------.
|   This function is for matching a string against a prefix of the     |
| input stream.  If the string matches the input, the input is         |
| discarded, otherwise the characters read are pushed back again.  The |
| function is used only when multicharacter quotes or comment          |
| delimiters are used.                                                 |
|                                                                      |
|   All strings herein should be unsigned.  Otherwise sign-extension   |
| of individual chars might break quotes with 8-bit chars in it.       |
`---------------------------------------------------------------------*/

static boolean
match_comment (const unsigned char *s)
{
  int n;                        /* number of characters matched */
  int ch;                       /* input character */
  const unsigned char *t;
  struct obstack *st;

  ch = peek_input ();
  if (ch != *s)
    return FALSE;               /* fail */

  (void) next_char ();
  if (s[1] == '\0')
    return TRUE;                /* short match */

  for (n = 1, t = s++; (ch = peek_input ()) == *s++; n++)
    {
      (void) next_char ();
      if (*s == '\0')           /* long match */
        return TRUE;
    }

  /* Push back input.  */
  st = push_string_init ();
  obstack_grow (st, t, n);
  push_string_finish (READ_NORMAL);
  return FALSE;
}

/*------------------------------------------------------------------------.
| The macro MATCH() is used to match a string against the input.  The     |
| first character is handled inline, for speed.  Hopefully, this will not |
| hurt efficiency too much when single character quotes and comment       |
| delimiters are used.                                                    |
| When it succeeds, ch is set to CHAR_EOF and then no character is        |
| put back on input.                                                      |
`------------------------------------------------------------------------*/

#define MATCH(ch, s) \
  ((s)[0] == (ch) \
   && (ch) != '\0' \
   && ((s)[1] == '\0' \
       || (match_comment ((unsigned char *) (s) + 1))))



/*----------------------------------------------------------.
| Inititialise input stacks, and quote/comment characters.  |
`----------------------------------------------------------*/

void
input_init (void)
{
  int i;

  current_file = xstrdup ("NONE");
  current_line = 0;
  array_current_line = (int *) xmalloc (sizeof (int) * nesting_limit);
  array_current_file = (char **) xmalloc (sizeof (char *) * nesting_limit);
  for (i = 0; i < nesting_limit; i++)
    array_current_file[i] = (char *) NULL;

  obstack_init (&token_stack);
  obstack_init (&input_stack);
  obstack_init (&wrapup_stack);

  current_input = &input_stack;

  obstack_1grow (&token_stack, '\0');
  token_bottom = obstack_finish (&token_stack);

  isp = NULL;
  wsp = NULL;
  next = NULL;

  start_of_input_line = FALSE;

  eolcomm.string = xstrdup (DEF_EOLCOMM);
  eolcomm.length = strlen (eolcomm.string);

  lquote.string  = xstrdup (DEF_LQUOTE);
  lquote.length  = strlen (lquote.string);

  rquote.string  = xstrdup (DEF_RQUOTE);
  rquote.length  = strlen (rquote.string);
  visible_quotes = FALSE;

  syntax_init ();
}

void
syntax_init (void)
{
  int ch;

  for (ch = 256; --ch > 0; )
    {
      if (isspace(ch))
        set_syntax_internal(SYNTAX_SPACE, ch);
      else if (isalpha(ch) || ch == '_')
        set_syntax_internal(SYNTAX_ALPHA, ch);
      else if (isdigit(ch))
        set_syntax_internal(SYNTAX_NUM, ch);
      else
        set_syntax_internal(SYNTAX_OTHER, ch);
    }
  /* set_syntax_internal(SYNTAX_IGNORE, 0); */

  set_syntax_internal(SYNTAX_GROUP, CHAR_LQUOTE);
  set_syntax_internal(SYNTAX_GROUP, CHAR_RQUOTE);
  set_syntax_internal(SYNTAX_GROUP, CHAR_BGROUP);
  set_syntax_internal(SYNTAX_GROUP, CHAR_EGROUP);
}

/*---------------------------------------------------------.
| Deallocate obstacks.                                     |
| This function is not necessary, but freeing memory when  |
| program  stops helps finding memory leaks.               |
`---------------------------------------------------------*/

void
input_deallocate (void)
{
  int i;

  xfree ((voidstar) array_current_line);
  for (i = 0; i < nesting_limit; i++)
    xfree(array_current_file[i]);
  xfree ((voidstar) array_current_file);
  xfree ((voidstar) eolcomm.string);
  xfree ((voidstar) lquote.string);
  xfree ((voidstar) rquote.string);

  obstack_free (&token_stack, 0);
  obstack_free (&input_stack, 0);
  obstack_free (&wrapup_stack, 0);
}


/*-------------------------------------------.
| Functions to manipulate the syntax table.  |
`-------------------------------------------*/

void
set_syntax_internal (int code, int ch)
{
  if (code & SYNTAX_MASKS)
    syntax_table[ch] |= code;
  else
    syntax_table[ch] = code;

#ifdef DEBUG_SYNTAX
  fprintf(stderr, "Set syntax %o %c = %04X\n", 
          ch, isprint(ch) ? ch : '-',
          syntax_table[ch]);
#endif
}

void
unset_syntax_attribute (int code, int ch)
{
  if (code & SYNTAX_MASKS)
    syntax_table[ch] &= ~code;

#ifdef DEBUG_SYNTAX
  fprintf(stderr, "Unset syntax %o %c = %04X\n", 
          ch, isprint(ch) ? ch : '-',
          syntax_table[ch]);
#endif
}

void
set_syntax (int code, const char *chars)
{
  int ch;

  if (*chars != '\0')
    {
      while ((ch = *chars++))
        set_syntax_internal (code, ch);
    }
  else
    {
      for (ch = 256; --ch > 0; )
        set_syntax_internal (code, ch);
    }
}



/*-------------------------------------------------------------------------.
| Parse and return a single token from the input stream.  A token can      |
| either be TOKEN_EOF, if the input_stack is empty; it can be TOKEN_QUOTED |
| for a quoted string; TOKEN_WORD for something that is a potential macro  |
| name; TOKEN_STRING for a list of alphanumeric characters; and            |
| TOKEN_SIMPLE for any single character that is not a part of any of the   |
| previous types.                                                          |
|                                                                          |
| Next_token () return the token type, and passes back a pointer to the    |
| token data through TD.  The token text is collected on the obstack       |
| token_stack, which never contains more than one token text at a time.    |
| The storage pointed to by the fields in TD is therefore subject to       |
| change the next time next_token () is called.                            |
`-------------------------------------------------------------------------*/

token_type
next_token (token_data *td, read_type expansion, boolean in_string)
{
  int ch;
  token_type type = TOKEN_NONE;
  int quote_level;

  if (TOKEN_DATA_TYPE (&token_read) != TOKEN_VOID)
    {
      type = TOKEN_STRING;
      obstack_grow (&token_stack, TOKEN_DATA_TEXT (&token_read),
              strlen (TOKEN_DATA_TEXT (&token_read)));
      xfree ((voidstar) TOKEN_DATA_TEXT (&token_read));
      TOKEN_DATA_TYPE (&token_read) = TOKEN_VOID;
    }

  while (type == TOKEN_NONE)
    {
      obstack_free (&token_stack, token_bottom);
      obstack_1grow (&token_stack, '\0');
      token_bottom = obstack_finish (&token_stack);

      ch = peek_input ();
      if (ch == CHAR_EOF)                 /* EOF */
        {
#ifdef DEBUG_INPUT
          fprintf (stderr, "next_token -> EOF\n");
#endif
          return TOKEN_EOF;
        }

      if (ch == CHAR_MACRO)               /* MACRO TOKEN */
        {
          init_macro_token (td);
          (void) next_char ();
#ifdef DEBUG_INPUT
          print_token("next_token", TOKEN_MACDEF, td);
#endif
          return TOKEN_MACDEF;
        }

      (void) next_char ();
      if (IS_TAG(ch))             /* ESCAPED WORD */
        {
          if (lquote.length > 0 && MATCH (ch, lquote.string))
            {
              if (visible_quotes || expansion == READ_ATTR_VERB
                  || expansion == READ_ATTR_ASIS || expansion == READ_BODY)
                obstack_grow (&token_stack, lquote.string, lquote.length);
              while ((ch = next_char ()) != CHAR_EOF)
                {
                  if (rquote.length > 0 && MATCH (ch, rquote.string))
                    break;
                  obstack_1grow (&token_stack, ch);
                }
              if (visible_quotes || expansion == READ_ATTR_VERB
                  || expansion == READ_ATTR_ASIS || expansion == READ_BODY)
                {
                  obstack_grow (&token_stack, rquote.string, rquote.length);
                  type = TOKEN_STRING;
                }
              else
                type = TOKEN_QUOTED;
            }
          else
            {
              obstack_1grow (&token_stack, ch);
              if ((ch = peek_input ()) != CHAR_EOF)
                {
                  if (ch == '/')
                    {
                      obstack_1grow (&token_stack, '/');
                      (void) next_char ();
                      ch = peek_input ();
                    }
                  if (IS_ALPHA(ch))
                    {
                      ch = next_char ();
                      obstack_1grow (&token_stack, ch);
                      while ((ch = next_char ()) != CHAR_EOF && IS_ALNUM(ch))
                        {
                          obstack_1grow (&token_stack, ch);
                        }
                      if (ch == '*')
                        {
                          obstack_1grow (&token_stack, ch);
                          ch = peek_input ();
                        }
                      else
                        unget_input(ch);

                      if (IS_SPACE(ch) || IS_CLOSE(ch) || IS_SLASH (ch))
                        type = TOKEN_WORD;
                      else
                        type = TOKEN_STRING;
                    }
                  else
                    type = TOKEN_STRING;
                }
              else
                type = TOKEN_SIMPLE;        /* escape before eof */
            }
        }
      else if (IS_CLOSE(ch))
        {
          obstack_1grow (&token_stack, ch);
          type = TOKEN_SIMPLE;
        }
      else if (IS_ENTITY(ch))             /* entity  */
        {
          obstack_1grow (&token_stack, ch);
          if ((ch = peek_input ()) != CHAR_EOF)
            {
              if (IS_ALPHA(ch))
                {
                  ch = next_char ();
                  obstack_1grow (&token_stack, ch);
                  while ((ch = next_char ()) != CHAR_EOF && IS_ALNUM(ch))
                    {
                      obstack_1grow (&token_stack, ch);
                    }

                  if (ch == ';')
                    {
                      obstack_1grow (&token_stack, ch);
                      type = TOKEN_ENTITY;
                    }
                  else
                    {
                      type = TOKEN_STRING;
                      unget_input(ch);
                    }
                }
              else
                type = TOKEN_STRING;
            }
          else
            type = TOKEN_SIMPLE;        /* escape before eof */
        }
      else if (eolcomm.length > 0 && MATCH (ch, eolcomm.string))
        skip_line ();
      else if (expansion == READ_BODY)
        {
          if (ch == '"')
            obstack_1grow (&token_stack, CHAR_QUOTE);
          else
            obstack_1grow (&token_stack, ch);
          while ((ch = next_char ()) != CHAR_EOF
                  && ! IS_TAG(ch) && ! IS_CLOSE (ch))
            {
              if (eolcomm.length > 0 && MATCH (ch, eolcomm.string))
                {
                  skip_line ();
                  ch = CHAR_EOF;
                  break;
                }
              if (ch == '"')
                obstack_1grow (&token_stack, CHAR_QUOTE);
              else
                obstack_1grow (&token_stack, ch);
            }
          unget_input(ch);

          type = TOKEN_STRING;
        }
      /*  Below we know that expansion != READ_BODY  */
      else if (IS_ALPHA(ch))
        {
          obstack_1grow (&token_stack, ch);
          while ((ch = next_char ()) != CHAR_EOF && (IS_ALNUM(ch)))
            {
              obstack_1grow (&token_stack, ch);
            }
          if (ch == '*')
            {
              obstack_1grow (&token_stack, ch);
              ch = peek_input ();
            }
          else
            unget_input(ch);

          type = TOKEN_STRING;
        }
      else if (IS_RQUOTE(ch))
        {
          MP4HERROR ((EXIT_FAILURE, 0,
             "INTERNAL ERROR: CHAR_RQUOTE found."));
        }
      else if (IS_LQUOTE(ch))             /* QUOTED STRING */
        {
          quote_level = 1;
          while (1)
            {
              ch = next_char ();
              if (ch == CHAR_EOF)
                MP4HERROR ((EXIT_FAILURE, 0,
                   "INTERNAL ERROR: EOF in string"));

              if (IS_BGROUP(ch) || IS_EGROUP(ch))
                continue;
              else if (IS_RQUOTE(ch))
                {
                  quote_level--;
                  if (quote_level == 0)
                      break;
                }
              else if (IS_LQUOTE(ch))
                quote_level++;
              else
                obstack_1grow (&token_stack, ch);
            }
          type = TOKEN_QUOTED;
        }
      else if (IS_BGROUP(ch))             /* BEGIN GROUP */
        type = TOKEN_BGROUP;
      else if (IS_EGROUP(ch))             /* END GROUP */
        type = TOKEN_EGROUP;
      else if (ch == '"')                 /* QUOTED STRING */
        {
          switch (expansion)
          {
            case READ_NORMAL:
              obstack_1grow (&token_stack, CHAR_QUOTE);
              type = TOKEN_SIMPLE;
              break;

            case READ_ATTRIBUTE:
            case READ_ATTR_VERB:
              type = TOKEN_QUOTE;
              break;

            case READ_ATTR_ASIS:
            case READ_ATTR_QUOT:
              obstack_1grow (&token_stack, '"');
              type = TOKEN_QUOTE;
              break;

            default:
              MP4HERROR ((warning_status, 0,
                "INTERNAL ERROR: Unknown expansion type"));
              exit (1);
          }
        }
      else if (ch == '\\')
        {
          switch (expansion)
          {
            case READ_NORMAL:
              obstack_1grow (&token_stack, ch);
              type = TOKEN_SIMPLE;
              break;

            case READ_ATTRIBUTE:
            case READ_ATTR_QUOT:
              ch = next_char();
              if (ch == 'n')
                obstack_1grow (&token_stack, '\n');
              else if (ch == 't')
                obstack_1grow (&token_stack, '\t');
              else if (ch == 'r')
                obstack_1grow (&token_stack, '\r');
              else if (ch == '\\')
                obstack_1grow (&token_stack, ch);
              else if (ch == '"' && in_string)
                obstack_1grow (&token_stack, CHAR_QUOTE);
              else
                {
                  if (!(exp_flags & EXP_STD_BSLASH))
                    obstack_1grow (&token_stack, '\\');
                  obstack_1grow (&token_stack, ch);
                }

              type = TOKEN_STRING;
              break;

            case READ_ATTR_VERB:
              ch = next_char();
              if (ch == '"' && in_string)
                obstack_1grow (&token_stack, CHAR_QUOTE);
              else
                {
                  obstack_1grow (&token_stack, '\\');
                  obstack_1grow (&token_stack, ch);
                }

              type = TOKEN_STRING;
              break;

            case READ_ATTR_ASIS:
              obstack_1grow (&token_stack, ch);
              ch = next_char();
              obstack_1grow (&token_stack, ch);
              type = TOKEN_STRING;
              break;

            default:
              MP4HERROR ((warning_status, 0,
                "INTERNAL ERROR: Unknown expansion type"));
              exit (1);
          }
        }
      else /* EVERYTHING ELSE */
        {
          obstack_1grow (&token_stack, ch);

          if (IS_OTHER(ch) || IS_NUM(ch))
            type = TOKEN_STRING;
          else if (IS_SPACE(ch))
            {
              while ((ch = next_char ()) != CHAR_EOF && IS_SPACE(ch))
                obstack_1grow (&token_stack, ch);
              unget_input(ch);
              type = TOKEN_SPACE;
            }
          else
            type = TOKEN_SIMPLE;
        }
    }

  obstack_1grow (&token_stack, '\0');

  TOKEN_DATA_TYPE (td) = TOKEN_TEXT;
  TOKEN_DATA_TEXT (td) = obstack_finish (&token_stack);

#ifdef DEBUG_INPUT
  print_token("next_token", type, td);
#endif

  return type;
}



/*-----------------------.
| Read a file verbatim.  |
`-----------------------*/

void
read_file_verbatim (struct obstack *obs)
{
  int ch;
  int (*f)(void);
  struct obstack *st;

  if (next != NULL)
    st = obs;
  else
    st = push_string_init ();

  f = isp->funcs->read_func;
  if (f != NULL)
    {
      while ((ch = (*f)()) != CHAR_RETRY)
        obstack_1grow (st, ch);
    }
  else
    {
      MP4HERROR ((warning_status, 0,
        "INTERNAL ERROR: Input stack botch in read_file_verbatim ()"));
      exit (1);
    }
  push_string_finish (READ_BODY);
}



#ifdef DEBUG_INPUT

static int
print_token (const char *s, token_type t, token_data *td)
{
  fprintf (stderr, "%s: ", s);
  switch (t)
    {                           /* TOKSW */
    case TOKEN_SIMPLE:
      fprintf (stderr, "char\t\"%s\"\n", TOKEN_DATA_TEXT (td));
      break;

    case TOKEN_WORD:
      fprintf (stderr, "word\t\"%s\"\n", TOKEN_DATA_TEXT (td));
      break;

    case TOKEN_ENTITY:
      fprintf (stderr, "entity\t\"%s\"\n", TOKEN_DATA_TEXT (td));
      break;

    case TOKEN_STRING:
      fprintf (stderr, "string\t\"%s\"\n", TOKEN_DATA_TEXT (td));
      break;

    case TOKEN_QUOTED:
      fprintf (stderr, "quoted\t\"%s\"\n", TOKEN_DATA_TEXT (td));
      break;

    case TOKEN_BGROUP:
      fprintf (stderr, "bgroup\n");
      break;

    case TOKEN_EGROUP:
      fprintf (stderr, "egroup\n");
      break;

    case TOKEN_QUOTE:
      fprintf (stderr, "quote\n");
      break;

    case TOKEN_SPACE:
      fprintf (stderr, "space\t\"%s\"\n", TOKEN_DATA_TEXT (td));
      break;

    case TOKEN_MACDEF:
      fprintf (stderr, "macro 0x%x\n", (int)TOKEN_DATA_FUNC (td));
      break;

    case TOKEN_EOF:
      fprintf (stderr, "eof\n");
      break;

    case TOKEN_NONE:
      fprintf (stderr, "none\n");
      break;

    default:
      MP4HERROR ((warning_status, 0,
        "INTERNAL ERROR: unknown token in print_token"));
      break;
    }
  return 0;
}

static void
lex_debug (void)
{
  token_type t;
  token_data td;

  while ((t = next_token (&td, READ_NORMAL, FALSE)) != TOKEN_EOF)
    print_token ("lex", t, &td);
}
#endif
