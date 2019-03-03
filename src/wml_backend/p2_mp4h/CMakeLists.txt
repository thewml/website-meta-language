SET (PACKAGE_RAW "mp4h")
SET (VERSION_RAW "1.3.0")

SET (PACKAGE "\"${PACKAGE_RAW}\"")
SET (PACKAGE_VERSION "\"${VERSION_RAW}\"")

SET (PACKAGE_DATE "\"19-Jan-2008\"")

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
    __argz_count
    __argz_next
    __argz_stringify
    alloca
    argz_append
    argz_create_sep
    argz_insert
    argz_next
    argz_stringify
    dlerror
    feof_unlocked
    fgets_unlocked
    getc_unlocked
    getcwd
    getegid
    geteuid
    getgid
    getgrgid
    getpagesize
    getpwuid
    getuid
    iconv
    index
    memcpy
    memmove
    mempcpy
    mkstemp
    munmap
    obstack
    putenv
    rindex
    setenv
    setlocale
    stpcpy
    strcasecmp
    strchr
    strcmp
    strdup
    strerror
    strrchr
    strtol
    strtoul
    tmpfile
    tsearch
    vprintf
    xmalloc
    xstrdup
    )
CHECK_MULTI_FUNCTIONS_EXISTS(${FUNCTIONS_TO_CHECK})

SET (VERSION ${PACKAGE_VERSION})
SET (PACKAGE_NAME ${PACKAGE})

SET (PACKAGE_STRING "\"${PACKAGE_RAW} ${VERSION_RAW}\"")

SET (MP4HLIBDIR "${CMAKE_INSTALL_PREFIX}/lib/mp4h")

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )
INCLUDE_DIRECTORIES( "${CMAKE_CURRENT_SOURCE_DIR}/lib" )
INCLUDE_DIRECTORIES( "${CMAKE_CURRENT_SOURCE_DIR}/src" )
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/config.h.in" "${CMAKE_CURRENT_BINARY_DIR}/config.h")
ADD_SUBDIRECTORY( lib )
ADD_SUBDIRECTORY( src )
ADD_SUBDIRECTORY( "doc" )


IF (NOT (WIN32 OR CYGWIN))
    SET (modbase1 "intl")
    SET (module "${CMAKE_CURRENT_SOURCE_DIR}/modules/${modbase1}")
    SET (modbase "gettext")
    add_library(
        "${modbase}"
        MODULE
        "${module}/${modbase}.c"
    )

    install(
        FILES
            "${module}/${modbase}.src"
        DESTINATION
           "${MP4HLIBDIR}/${modbase1}/${modbase}.mp4hp"
    )
    install(
        FILES
            "${module}/${modbase}.mp4h"
        DESTINATION
            "${MP4HLIBDIR}"
    )
    install(
        TARGETS
            "${modbase}"
        DESTINATION
            "${MP4HLIBDIR}"
    )
ENDIF()
