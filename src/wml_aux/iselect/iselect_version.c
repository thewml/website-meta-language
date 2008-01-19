/*
**  iselect_version.c -- Version Information for iSelect (syntax: C/C++)
**  [automatically generated and maintained by GNU shtool]
*/

#ifdef _ISELECT_VERSION_C_AS_HEADER_

#ifndef _ISELECT_VERSION_C_
#define _ISELECT_VERSION_C_

#define ISELECT_VERSION 0x102200

typedef struct {
    const int   v_hex;
    const char *v_short;
    const char *v_long;
    const char *v_tex;
    const char *v_gnu;
    const char *v_web;
    const char *v_sccs;
    const char *v_rcs;
} iselect_version_t;

extern iselect_version_t iselect_version;

#endif /* _ISELECT_VERSION_C_ */

#else /* _ISELECT_VERSION_C_AS_HEADER_ */

#define _ISELECT_VERSION_C_AS_HEADER_
#include "iselect_version.c"
#undef  _ISELECT_VERSION_C_AS_HEADER_

iselect_version_t iselect_version = {
    0x102200,
    "1.2.0",
    "1.2.0 (01-Jul-2000)",
    "This is iSelect, Version 1.2.0 (01-Jul-2000)",
    "iSelect 1.2.0 (01-Jul-2000)",
    "iSelect/1.2.0",
    "@(#)iSelect 1.2.0 (01-Jul-2000)",
    "$Id: iSelect 1.2.0 (01-Jul-2000) $"
};

#endif /* _ISELECT_VERSION_C_AS_HEADER_ */

