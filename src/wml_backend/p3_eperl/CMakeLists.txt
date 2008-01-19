SET(CMAKE_VERBOSE_MAKEFILE ON)

include("${CMAKE_CURRENT_SOURCE_DIR}/../../Common.cmake")

include(FindPerl)
IF (NOT PERL_FOUND)
    MESSAGE ( FATAL_ERROR "perl must be installed")
ENDIF(NOT PERL_FOUND)

SET(PATH_PERL ${PERL_EXECUTABLE})
SET(perl_prog ${PERL_EXECUTABLE})
# TODO - this is just to get it working - change to something more meaningful
SET(prefix "/usr")

EXEC_PROGRAM(${PERL_EXECUTABLE}
    ARGS "-e" "use Config\; print \$Config{archlib}"
    OUTPUT_VARIABLE perl_archlib
)
EXEC_PROGRAM(${PERL_EXECUTABLE}
    ARGS "-e" "printf '%.3f',\$]"
    OUTPUT_VARIABLE perl_vers
)

EXEC_PROGRAM(${PERL_EXECUTABLE}
    ARGS "-e" "use Config\; print \$Config{libs}"
    OUTPUT_VARIABLE perl_libs
)

EXEC_PROGRAM(${PERL_EXECUTABLE}
    ARGS "-e" "use Config\; print \$Config{usedl}"
    OUTPUT_VARIABLE usedl
)

IF(usedl STREQUAL "define")
    SET (perl_dla "${perl_archlib}/auto/DynaLoader/DynaLoader.a")
    SET (HAVE_PERL_DYNALOADER 1)
ELSE(usedl STREQUAL "define")
    SET (perl_dla "")
ENDIF(usedl STREQUAL "define")

EXEC_PROGRAM("whoami"
    OUTPUT_VARIABLE build_user
)

EXEC_PROGRAM("date"
    ARGS "+%D-%m-%Y"
    OUTPUT_VARIABLE build_time_iso
)

SET (INCLUDE_FILES_TO_CHECK
    ctype.h
    grp.h
    netdb.h
    netinet/in.h
    pwd.h
    signal.h
    stdarg.h
    stdio.h
    stdlib.h
    string.h
    sys/param.h
    sys/socket.h
    sys/stat.h
    sys/types.h
    time.h
    unistd.h
    )
CHECK_MULTI_INCLUDE_FILES(${INCLUDE_FILES_TO_CHECK})



ADD_EXECUTABLE(eperl
    eperl_config.c
    eperl_debug.c
    eperl_getopt.c
    eperl_http.c
    eperl_license.c
    eperl_logo.c
    eperl_main.c
    eperl_parse.c
    eperl_perl5.c
    eperl_powered.c
    eperl_pp.c
    eperl_readme.c
    eperl_sys.c
    eperl_version.c
)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config_ac.h.cmake ${CMAKE_CURRENT_SOURCE_DIR}/config_ac.h)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config_sc.h.cmake ${CMAKE_CURRENT_SOURCE_DIR}/config_sc.h)
