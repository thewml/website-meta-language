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

MACRO(GET_PERL_CONFIG cfg)
    EXECUTE_PROCESS(COMMAND ${PERL_EXECUTABLE}
        "-e" "use Config; print \$Config{${cfg}}"
        OUTPUT_VARIABLE "perl_${cfg}"
    )    
ENDMACRO(GET_PERL_CONFIG)

SET (PERL_CONFIGS archlib libs usedl ccflags optimize)
FOREACH (cfg ${PERL_CONFIGS})
    GET_PERL_CONFIG(${cfg})
ENDFOREACH(cfg)

EXECUTE_PROCESS(COMMAND ${PERL_EXECUTABLE}
    "-e" "printf '%.3f',\$]"
    OUTPUT_VARIABLE perl_vers
)

IF(perl_usedl STREQUAL "define")
    SET (perl_dla "${perl_archlib}/auto/DynaLoader/DynaLoader.a")
    SET (HAVE_PERL_DYNALOADER 1)
ELSE(perl_usedl STREQUAL "define")
    SET (perl_dla "")
ENDIF(perl_usedl STREQUAL "define")

EXECUTE_PROCESS(COMMAND "whoami"
    OUTPUT_VARIABLE build_user
)

EXECUTE_PROCESS(COMMAND "date" "+%D-%m-%Y"
    OUTPUT_VARIABLE build_time_iso
)

MACRO(CHOMP VAR)
    STRING(REGEX REPLACE "[\r\n]+$" "" ${VAR} "${${VAR}}")
ENDMACRO(CHOMP)

CHOMP(build_user)
CHOMP(build_time_iso)

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

SET (FUNCTIONS_TO_CHECK 
    memmove
    setegid
    seteuid
    strdup
    )
CHECK_MULTI_FUNCTIONS_EXISTS(${FUNCTIONS_TO_CHECK})


SET (SOURCES 
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

ADD_EXECUTABLE(eperl
    ${SOURCES}
)

SET_SOURCE_FILES_PROPERTIES(${SOURCES}
    PROPERTIES COMPILE_FLAGS "${perl_optimize} ${perl_ccflags} -I${perl_archlib}/CORE -I."
)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config_ac.h.cmake ${CMAKE_CURRENT_SOURCE_DIR}/config_ac.h)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config_sc.h.cmake ${CMAKE_CURRENT_SOURCE_DIR}/config_sc.h)
