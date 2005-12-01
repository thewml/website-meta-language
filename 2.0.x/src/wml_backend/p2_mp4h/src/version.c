/*
**  src/version.c -- Version Information for mp4h (syntax: C/C++)
**  [automatically generated and maintained by GNU shtool]
*/

#ifdef _SRC_VERSION_C_AS_HEADER_

#ifndef _SRC_VERSION_C_
#define _SRC_VERSION_C_

#define MP4H_VERSION 0x102203

typedef struct {
    const int   v_hex;
    const char *v_short;
    const char *v_long;
    const char *v_tex;
    const char *v_gnu;
    const char *v_web;
    const char *v_sccs;
    const char *v_rcs;
} mp4h_version_t;

extern mp4h_version_t mp4h_version;

#endif /* _SRC_VERSION_C_ */

#else /* _SRC_VERSION_C_AS_HEADER_ */

#define _SRC_VERSION_C_AS_HEADER_
#include "src/version.c"
#undef  _SRC_VERSION_C_AS_HEADER_

mp4h_version_t mp4h_version = {
    0x102203,
    "1.2.3",
    "1.2.3 (26-Oct-2001)",
    "This is mp4h, Version 1.2.3 (26-Oct-2001)",
    "mp4h 1.2.3 (26-Oct-2001)",
    "mp4h/1.2.3",
    "@(#)mp4h 1.2.3 (26-Oct-2001)",
    "$Id: mp4h 1.2.3 (26-Oct-2001) $"
};

#endif /* _SRC_VERSION_C_AS_HEADER_ */

