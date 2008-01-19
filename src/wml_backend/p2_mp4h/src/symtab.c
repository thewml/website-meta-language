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

/* This file handles all the low level work around the symbol table.  The
   symbol table is a simple chained hash table.  Each symbol is described
   by a struct symbol, which is placed in the hash table based upon the
   symbol name.  Symbols that hash to the same entry in the table are
   kept on a list, sorted by name.  As a special case, to facilitate the
   "pushdef" and "popdef" builtins, a symbol can be several times in the
   symbol table, one for each definition.  Since the name is the same,
   all the entries for the symbol will be on the same list, and will
   also, because the list is sorted, be adjacent.  All the entries for a
   name are simply ordered on the list by age.  The current definition
   will then always be the first found.  */

#include "mp4h.h"

/*----------------------------------------------------------------------.
| Initialise the symbol table, by allocating the necessary storage, and |
| zeroing all the entries.                                              |
`----------------------------------------------------------------------*/

/* Pointer to symbol table.  */
symbol **sym_tab;

/* Pointer to entity table.  */
symbol **entity_tab;

/* Pointer to variable table.  */
symbol **var_tab;

/* Pointer to file table.  */
symbol **file_tab;

/* Any of previous pointers.  */
symbol **symtab;

/* True if tags are case insensitive  */
boolean caseless_tag;

/* True if variables are case insensitive  */
boolean caseless_var;

/* True if entities are case insensitive  */
boolean caseless_ent;

static void
hash_table_init (symbol **s)
{
  int i;
  for (i = hash_table_size; --i >= 0;)
    *s++ = NULL;
}

void
symtab_init (void)
{
  var_tab = (symbol **) xmalloc (hash_table_size * sizeof (symbol *));
  file_tab = (symbol **) xmalloc (hash_table_size * sizeof (symbol *));
  entity_tab = (symbol **) xmalloc (hash_table_size * sizeof (symbol *));
  sym_tab = (symbol **) xmalloc (hash_table_size * sizeof (symbol *));
  hash_table_init (var_tab);
  hash_table_init (file_tab);
  hash_table_init (entity_tab);
  hash_table_init (sym_tab);
  symtab = sym_tab;
}

void
caseless_init (int caseless)
{
  caseless_tag = ((caseless & 1) == 1);
  caseless_var = ((caseless & 2) == 2);
  caseless_ent = ((caseless & 4) == 4);
}

/*--------------------------------------------.
| Free all storage associated with a symbol.  |
`--------------------------------------------*/

static void
free_symbol (symbol *sym)
{
  xfree ((voidstar) SYMBOL_NAME (sym));
  xfree ((voidstar) SYMBOL_HOOK_BEGIN (sym));
  xfree ((voidstar) SYMBOL_HOOK_END (sym));
  if (SYMBOL_TYPE (sym) == TOKEN_TEXT)
    xfree ((voidstar) SYMBOL_TEXT (sym));
  xfree ((voidstar) sym);
}

static void
hash_table_free (symbol **s)
{
  int h;
  symbol *sym, *next;

  for (h = 0; h < hash_table_size; h++)
    {
      for (sym = s[h]; sym != NULL; )
        {
          next = SYMBOL_NEXT (sym);
          free_symbol (sym);
          sym = next;
        }
    }

  xfree ((voidstar) s);
}

void
symtab_deallocate (void)
{
  hash_table_free (var_tab);
  hash_table_free (file_tab);
  hash_table_free (entity_tab);
  hash_table_free (sym_tab);
}

/*--------------------------------------------------.
| Return a hashvalue for a string, from GNU-emacs.  |
`--------------------------------------------------*/

static int
hash (const char *s)
{
  register int val = 0;

  register const char *ptr = s;
  register char ch;

  while ((ch = *ptr++) != '\0')
    {
      if (ch >= 0140)
        ch -= 40;
      val = ((val << 3) + (val >> 28) + ch);
    };
  val = (val < 0) ? -val : val;
  return val % hash_table_size;
}

/*------------------------------------------------------------------------.
| Search in, and manipulation of the symbol table, are all done by        |
| lookup_symbol ().  It basically hashes NAME to a list in the symbol     |
| table, and searched this list for the first occurence of a symbol with  |
| the name.                                                               |
|                                                                         |
| The MODE parameter determines what lookup_symbol () will do.  It can    |
| either just do a lookup, do a lookup and insert if not present, do an   |
| insertion even if the name is already in the list, delete the first     |
| occurrence of the name on the list or delete all occurences of the name |
| on the list.                                                            |
`------------------------------------------------------------------------*/

static symbol *
generic_lookup (const char *name, symbol_lookup mode, boolean caseless)
{
  int h, cmp = 1;
  symbol *sym, *prev;
  symbol **spp;
  char *lcname;
  char *cp;

  lcname = xstrdup (name);
  if (caseless)
    {
      for (cp=lcname; *cp != '\0'; cp++)
        *cp = tolower (*cp);
    }

  h = hash (lcname);
  sym = symtab[h];

  for (prev = NULL; sym != NULL; prev = sym, sym = sym->next)
    {
      cmp = strcmp (SYMBOL_NAME (sym), lcname);
      if (cmp >= 0)
        break;
    }

  /* If just searching, return status of search.  */

  if (mode == SYMBOL_LOOKUP)
    {
      xfree ((voidstar) lcname);
      return cmp == 0 ? sym : NULL;
    }

  /* Symbol not found.  */

  spp = (prev != NULL) ?  &prev->next : &symtab[h];

  switch (mode)
    {

    case SYMBOL_INSERT:

      /* Return the symbol, if the name was found in the table.
         Otherwise, just insert the name, and return the new symbol.  */

      if (cmp == 0 && sym != NULL)
        break;

      /* Insert a name in the symbol table.  */
      sym = (symbol *) xmalloc (sizeof (symbol));
      initialize_builtin (sym);
      SYMBOL_NAME (sym) = xstrdup (lcname);

      SYMBOL_NEXT (sym) = *spp;
      (*spp) = sym;

      break;

    case SYMBOL_DELETE:

      /* Delete all occurences of symbols with NAME.  */

      if (cmp != 0)
        sym = NULL;
      if (sym == NULL)
        break;
      do
        {
          *spp = SYMBOL_NEXT (sym);
          free_symbol (sym);
          sym = *spp;
        }
      while (sym != NULL && strcmp (lcname, SYMBOL_NAME (sym)) == 0);
      sym = NULL;
      break;

    default:
      MP4HERROR ((warning_status, 0,
        "INTERNAL ERROR: Illegal mode to symbol_lookup ()"));
      exit (1);
    }
  xfree ((voidstar) lcname);
  return sym;
}

symbol *
lookup_symbol (const char *name, symbol_lookup mode)
{
  symtab = sym_tab;
  return generic_lookup (name, mode, caseless_tag);
}

symbol *
lookup_entity (const char *name, symbol_lookup mode)
{
  symtab = entity_tab;
  return generic_lookup (name, mode, caseless_ent);
}

symbol *
lookup_variable (const char *name, symbol_lookup mode)
{
  symtab = var_tab;
  return generic_lookup (name, mode, caseless_var);
}

symbol *
lookup_file (const char *name, symbol_lookup mode)
{
  symtab = file_tab;
  return generic_lookup (name, mode, FALSE);
}

/*----------------------------------------------------------------------.
| The following function is used for the cases, where we want to do     |
| something to each and every symbol in the table.  The function        |
| hack_all_symbols () traverses the symbol table, and calls a specified |
| function FUNC for each symbol in the table.  FUNC is called with a    |
| pointer to the symbol, and the DATA argument.                         |
`----------------------------------------------------------------------*/

void
hack_all_symbols (hack_symbol *func, const char *data)
{
  int h;
  symbol *sym;

  for (h = 0; h < hash_table_size; h++)
    {
      for (sym = symtab[h]; sym != NULL; sym = SYMBOL_NEXT (sym))
        (*func) (sym, data);
    }
}


#ifdef DEBUG_SYM

static void
dump_symbol (symbol *sym, const char *data)
{
  printf ("\tname %s, addr 0x%p, next 0x%p, flags%s\n",
           SYMBOL_NAME (sym), sym, sym->next,
           SYMBOL_TRACED (sym) ? " traced" : "");
}

static void
symtab_debug (void)
{
  token_type t;
  token_data td;
  const char *text;
  symbol *s;
  int delete;

  while ((t = next_token (&td, READ_NORMAL, FALSE)) != TOKEN_EOF)
    {
      if (t != TOKEN_WORD)
        continue;
      text = TOKEN_DATA_TEXT (&td);
      if (*text == '_')
        {
          delete = 1;
          text++;
        }
      else
        delete = 0;

      s = lookup_symbol (text, SYMBOL_LOOKUP);

      if (s == NULL)
        {
          printf (_("Name `%s' is unknown"), text);
          printf ("\n");
        }

      if (delete)
        (void) lookup_symbol (text, SYMBOL_DELETE);
      else
        (void) lookup_symbol (text, SYMBOL_INSERT);
    }
  hack_all_symbols (dump_symbol, "");
}

static void
symtab_print_list (int i)
{
  symbol *sym;

  printf ("Symbol dump %d:\n", i);
  for (sym = symtab[0]; sym != NULL; sym = sym->next)
    printf ("\tname %s, addr 0x%p, next 0x%p, flags%s\n",
           SYMBOL_NAME (sym), sym, sym->next,
           SYMBOL_TRACED (sym) ? " traced" : "");
}

#endif /* DEBUG_SYM */
