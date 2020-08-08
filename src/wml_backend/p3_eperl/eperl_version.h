/*  eperl_version.h -- Version Information for ePerl (syntax: C/C++)
 [was automatically generated and maintained by GNU shtool; now tweaked manually] */

#ifndef EPERL_VERSION_H
#define EPERL_VERSION_H

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
} eperl_version_type;

static const eperl_version_type eperl_version = {
    0x20220E,
    "2.2.14",
    "2.2.14 (02-Aug-1998)",
    "This is ePerl, Version 2.2.14 (02-Aug-1998)",
    "ePerl 2.2.14 (02-Aug-1998)",
    "ePerl/2.2.14",
    "@(#)ePerl 2.2.14 (02-Aug-1998)",
    "$Id: ePerl 2.2.14 (02-Aug-1998) $"
};

#endif
