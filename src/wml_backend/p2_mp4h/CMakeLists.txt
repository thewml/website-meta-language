SET (PACKAGE_RAW "mp4h")
SET (VERSION_RAW "1.3.0")

SET (PACKAGE "\"${PACKAGE_RAW}\"")
SET (PACKAGE_VERSION "\"${VERSION_RAW}\"")

SET (PACKAGE_DATE "\"19-Jan-2008\"")

CHECK_INCLUDE_FILE("dirent.h" HAVE_DIRENT_H)
SET (LTDL_OBJDIR "./lt")

SET (INCLUDE_FILES_TO_CHECK
    alloca.h
    argz.h
    dld.h
    dlfcn.h
    dl.h
    grp.h
    locale.h
    memory.h
    pwd.h
    sys/dl.h
    sys/param.h
    unistd.h
    )
CHECK_MULTI_INCLUDE_FILES(${INCLUDE_FILES_TO_CHECK})

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
    getegid
    geteuid
    getgid
    getgrgid
    getpagesize
    getpwuid
    getuid
    iconv
    mempcpy
    munmap
    obstack
    setlocale
    stpcpy
    strcasecmp
    tsearch
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

MACRO (_gen_mod modbase1 modbase with_mp4h)
    IF (NOT (WIN32 OR CYGWIN))
        SET (module "${CMAKE_CURRENT_SOURCE_DIR}/modules/${modbase1}")
        add_library(
            "${modbase}"
            MODULE
            "${module}/${modbase}.c"
        )
        set_target_properties("${modbase}" PROPERTIES PREFIX "")

        install(
            FILES
                "${module}/${modbase}.src"
            DESTINATION
               "${MP4HLIBDIR}/${modbase1}"
           RENAME
               "${modbase}.mp4hp"
        )
        if (with_mp4h)
        install(
            FILES
                "${module}/${modbase}.mp4h"
            DESTINATION
                "${MP4HLIBDIR}"
        )
        endif()
        install(
            TARGETS
                "${modbase}"
            DESTINATION
                "${MP4HLIBDIR}/${modbase1}"
        )
        SET(WITH_MODULES 1)
        SET(HAVE_LIBDL 1)
    ENDIF()
ENDMACRO()

_gen_mod("intl" "gettext" TRUE)
_gen_mod("system" "exec" "")
_gen_mod("system" "sh" "")
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/config.h.in" "${CMAKE_CURRENT_BINARY_DIR}/config.h")
ADD_SUBDIRECTORY( lib )
ADD_SUBDIRECTORY( src )
ADD_SUBDIRECTORY( "doc" )
