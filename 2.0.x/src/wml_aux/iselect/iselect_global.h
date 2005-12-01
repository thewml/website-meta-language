/*      _ ____       _           _   
**     (_) ___|  ___| | ___  ___| |_ 
**    / /\___ \ / _ \ |/ _ \/ __| __|
**   / /  ___) |  __/ |  __/ (__| |_ 
**  (_(  |____/ \___|_|\___|\___|\__|
**
**  iSelect -- Interactive Selection Tool
**
**  iSelect is a Curses-based tool for interactive line selection 
**  in an ASCII file via a full-screen terminal session.
**  
**  ======================================================================
**
**  Copyright (c) 1996-1999 Ralf S. Engelschall.
**
**  This program is free software; it may be redistributed and/or
**  modified only under the terms of the GNU General Public License, 
**  which may be found in the iSelect source distribution.  
**  Look at the file COPYING for details. 
**  
**  This program is distributed in the hope that it will be useful, 
**  but WITHOUT ANY WARRANTY; without even the implied warranty of 
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
**  See the the GNU General Public License for more details.
**
**  ======================================================================
**
**  iselect_global.c -- global header
*/

#define EX__BASE        64      /* base value for error messages */
#define EX_USAGE        64      /* command line usage error */
#define EX_DATAERR      65      /* data format error */
#define EX_NOINPUT      66      /* cannot open input */
#define EX_NOUSER       67      /* addressee unknown */
#define EX_NOHOST       68      /* host name unknown */
#define EX_UNAVAILABLE  69      /* service unavailable */
#define EX_SOFTWARE     70      /* internal software error */
#define EX_OSERR        71      /* system error (e.g., can't fork) */
#define EX_OSFILE       72      /* critical OS file missing */
#define EX_CANTCREAT    73      /* can't create (user) output file */
#define EX_IOERR        74      /* input/output error */
#define EX_TEMPFAIL     75      /* temp failure; user is invited to retry */
#define EX_PROTOCOL     76      /* remote error in protocol */
#define EX_NOPERM       77      /* permission denied */
#define EX_CONFIG       78      /* configuration error */
#define EX__MAX         78      /* maximum listed value */

/* OK and FAIL exits should ALLWAYS exists */
#ifndef EX_OK
#define EX_OK   0
#endif
#ifndef EX_FAIL
#define EX_FAIL 1
#endif

#if __STDC__
#define VOID void
#else
#define VOID char
#endif

/*
 *  CU() -- CleanUp Makro
 *  implemented in a safety way
 */
#define DECL_EXRC int rc
#define ZERO 0
#define STMT(stuff) do { stuff } while (ZERO)
#define CU(returncode) STMT( rc = returncode; goto CUS; )
#define RETURN_EXRC return (rc)

/*
 *  Yeah, here it comes...all ASCII control codes.
 *  Defined as their correct acronyms.
 */
#define ASC_NUL '\x00'
#define ASC_SOH '\x01'
#define ASC_STX '\x02'
#define ASC_ETX '\x03'
#define ASC_EOT '\x04'
#define ASC_ENQ '\x05'
#define ASC_ACK '\x06'
#define ASC_BEL '\x07'
#define ASC_BS  '\x08'
#define ASC_HT  '\x09'
#define ASC_LF  '\x0a'
#define ASC_VT  '\x0b'
#define ASC_FF  '\x0c'
#define ASC_CR  '\x0d'
#define ASC_SO  '\x0e'
#define ASC_SI  '\x0f'
#define ASC_DLE '\x10'
#define ASC_DC1 '\x11'
#define ASC_DC2 '\x12'
#define ASC_DC3 '\x13'
#define ASC_DC4 '\x14'
#define ASC_NAK '\x15'
#define ASC_SYN '\x16'
#define ASC_ETB '\x17'
#define ASC_CAN '\x18'
#define ASC_EM  '\x19'
#define ASC_SUB '\x1a'
#define ASC_ESC '\x1b'
#define ASC_FS  '\x1c'
#define ASC_GS  '\x1d'
#define ASC_RS  '\x1e'
#define ASC_US  '\x1f'
#define ASC_SP  '\x20'
#define ASC_DEL '\x7f'
#define NUL ASC_NUL

#define ASC_QUOTE '\x22'
#define ASC_NL    ASC_LF
#define NL        ASC_NL

/*   define the boolean values in a general and
 *   portable way.
 */
#undef  TRUE
#define TRUE  (0 || !(0))
#undef  FALSE
#define FALSE (!(TRUE))
/* typedef enum { false = FALSE, true = TRUE } bool; */

/*   some other names for true and false */
#define YES   TRUE
#define NO    FALSE

#define GOOD  TRUE
#define WRONG FALSE

#undef  OK
#define OK    TRUE
#define BAD   FALSE

#define SOME  TRUE
#define NONE  FALSE

/*
 *   define NULL if not already defined
 */
#ifndef NULL
#define NULL (void *)0
#endif


/*   define some handy str* functions
 */
#define   strNE(s1, s2)          (strcmp(s1, s2))
#define   strEQ(s1, s2)         (!strcmp(s1, s2))
#define  strnNE(s1, s2, l)      (strncmp(s1, s2, l))
#define  strnEQ(s1, s2, l)     (!strncmp(s1, s2, l))
#define strniNE(s1, s2, l)  (strncasecmp(s1, s2, l))
#define strniEQ(s1, s2, l) (!strncasecmp(s1, s2, l))


/* message routines -- taken from the kpathsea 2.4 library lib.h file */

/* This should be called only after a system call fails.  Don't exit
   with status `errno', because that might be 256, which would mean
   success (exit statuses are truncated to eight bits).  */
#define FATAL_PERROR(s) do { perror (s); exit (EX_FAIL); } while (0)

#define START_FATAL() do { fputs ("fatal: ", stderr)
#define END_FATAL() fputs (".\n", stderr); exit (1); } while (0)
#define FATAL(str)                          \
  START_FATAL (); fputs (str, stderr); END_FATAL ()
#define FATAL1(str, e1)                         \
  START_FATAL (); fprintf (stderr, str, e1); END_FATAL ()
#define FATAL2(str, e1, e2)                     \
  START_FATAL (); fprintf (stderr, str, e1, e2); END_FATAL ()
#define FATAL3(str, e1, e2, e3)                     \
  START_FATAL (); fprintf (stderr, str, e1, e2, e3); END_FATAL ()
#define FATAL4(str, e1, e2, e3, e4)                 \
  START_FATAL (); fprintf (stderr, str, e1, e2, e3, e4); END_FATAL ()
#define FATAL6(str, e1, e2, e3, e4, e5, e6)             \
  START_FATAL (); fprintf (stderr, str, e1, e2, e3, e4, e5, e6); END_FATAL ()

#define START_WARNING() do { fputs ("warning: ", stderr)
#define END_WARNING() fputs (".\n", stderr); fflush (stderr); } while (0)
#define WARNING(str)                            \
  START_WARNING (); fputs (str, stderr); END_WARNING ()
#define WARNING1(str, e1)                       \
  START_WARNING (); fprintf (stderr, str, e1); END_WARNING ()
#define WARNING2(str, e1, e2)                       \
  START_WARNING (); fprintf (stderr, str, e1, e2); END_WARNING ()
#define WARNING3(str, e1, e2, e3)                   \
  START_WARNING (); fprintf (stderr, str, e1, e2, e3); END_WARNING ()
#define WARNING4(str, e1, e2, e3, e4)                   \
  START_WARNING (); fprintf (stderr, str, e1, e2, e3, e4); END_WARNING ()

/*EOF*/
