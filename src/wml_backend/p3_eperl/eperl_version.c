/*
**  eperl_version.c -- Version Information for ePerl (syntax: C/C++)
**  [automatically generated and maintained by GNU shtool]
*/

#ifdef _EPERL_VERSION_C_AS_HEADER_

#ifndef _EPERL_VERSION_C_
#define _EPERL_VERSION_C_

#define EPERL_VERSION 0x20220E

typedef struct {
    const int   v_hex;
    const char *v_short;
    const char *v_long;
    const char *v_tex;
    const char *v_gnu;
    const char *v_web;
    const char *v_sccs;
    const char *v_rcs;
} eperl_version_t;

extern eperl_version_t eperl_version;

#endif /* _EPERL_VERSION_C_ */

#else /* _EPERL_VERSION_C_AS_HEADER_ */

#define _EPERL_VERSION_C_AS_HEADER_
#include "eperl_version.c"
#undef  _EPERL_VERSION_C_AS_HEADER_

eperl_version_t eperl_version = {
    0x20220E,
    "2.2.14",
    "2.2.14 (02-Aug-1998)",
    "This is ePerl, Version 2.2.14 (02-Aug-1998)",
    "ePerl 2.2.14 (02-Aug-1998)",
    "ePerl/2.2.14",
    "@(#)ePerl 2.2.14 (02-Aug-1998)",
    "$Id: ePerl 2.2.14 (02-Aug-1998) $"
};

#endif /* _EPERL_VERSION_C_AS_HEADER_ */

