SET (PACKAGE_RAW "mp4h")
SET (VERSION_RAW "1.3.0")

SET (PACKAGE "\"${PACKAGE_RAW}\"")
SET (PACKAGE_VERSION "\"${VERSION_RAW}\"")

SET (PACKAGE_DATE "\"19-Jan-2008\"")

# see - http://www.cmake.org/Wiki/CMake_FAQ#Is_there_an_option_to_produce_more_.27verbose.27_compiling.3F
SET(CMAKE_VERBOSE_MAKEFILE ON)

include("${CMAKE_CURRENT_SOURCE_DIR}/Common.cmake")
SUBDIRS (
    lib
    src
)

# CHECK_INCLUDE_FILE("stdarg.h" HAVE_STDARG_H)
# CHECK_INCLUDE_FILE("unistd.h" HAVE_UNISTD_H)
# CHECK_INCLUDE_FILE("stdio.h" HAVE_STDIO_H)

SET (INCLUDE_FILES_TO_CHECK
    alloca.h
    argz.h
    assert.h
    ctype.h
    dirent.h
    dld.h
    dlfcn.h
    dl.h
    errno.h
    grp.h
    inttypes.h
    limits.h
    locale.h
    malloc.h
    memory.h
    ndir.h
    nl/types.h
    pwd.h
    stdarg.h
    stddef.h
    stdint.h
    stdio.h
    stdlib.h
    string.h
    strings.h
    strings.h
    sys/dir.h
    sys/dl.h
    sys/ndir.h
    sys/param.h
    sys/stat.h
    sys/times.h
    sys/types.h
    time.h
    unistd.h
    )
CHECK_MULTI_INCLUDE_FILES(${INCLUDE_FILES_TO_CHECK})

SET (REQUIRED_STDC_HEADERS float.h string.h stdlib.h stdarg.h)
CHECK_INCLUDE_FILES("${REQUIRED_STDC_HEADERS}" STDC_HEADERS)

SET (FUNCTIONS_TO_CHECK 
    alloca
    argz_append
    argz_create_sep
    argz_insert
    argz_next
    argz_stringify
    tmpfile
    mkstemp
    strerror
    vprintf
    )
CHECK_MULTI_FUNCTIONS_EXISTS(${FUNCTIONS_TO_CHECK})

SET (VERSION ${PACKAGE_VERSION})
SET (PACKAGE_NAME ${PACKAGE})

SET (PACKAGE_STRING "\"${PACKAGE_RAW} ${VERSION_RAW}\"")

SET (MP4HLIBDIR "${CMAKE_INSTALL_PREFIX}/lib/mp4h")

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_SOURCE_DIR}/config.h)
