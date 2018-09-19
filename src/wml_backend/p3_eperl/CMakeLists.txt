include(FindPerl)
IF (NOT PERL_FOUND)
    MESSAGE ( FATAL_ERROR "perl must be installed")
ENDIF(NOT PERL_FOUND)

MACRO(GET_PERL_CONFIG cfg)
    EXECUTE_PROCESS(COMMAND ${PERL_EXECUTABLE}
        "-e" "use Config; print \$Config{${cfg}}"
        OUTPUT_VARIABLE "perl_${cfg}"
    )
ENDMACRO(GET_PERL_CONFIG)

SET (PERL_CONFIGS archlib libs usedl ccflags optimize ccdlflags ldflags)
FOREACH (cfg ${PERL_CONFIGS})
    GET_PERL_CONFIG(${cfg})
ENDFOREACH(cfg)

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

SET (SOURCES
    eperl_main.c
)

SET(EXE "eperl")
ADD_EXECUTABLE(${EXE}
    ${SOURCES}
)

SET(SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/eperl_perl5_sm.pl")
SET(HEADER "${CMAKE_CURRENT_BINARY_DIR}/eperl_perl5_sm.h")
ADD_CUSTOM_COMMAND(
    OUTPUT ${HEADER}
    COMMAND ${PERL_EXECUTABLE}
    ARGS "${SCRIPT}" "-o" "${HEADER}"
    DEPENDS ${SCRIPT}
    VERBATIM
)


SET_SOURCE_FILES_PROPERTIES("eperl_main.c"
    PROPERTIES OBJECT_DEPENDS ${HEADER}
)

SET_SOURCE_FILES_PROPERTIES(${SOURCES}
    PROPERTIES COMPILE_FLAGS "${perl_optimize} ${perl_ccflags} -I${perl_archlib}/CORE -I."
)

SET_TARGET_PROPERTIES(${EXE}
    PROPERTIES LINK_FLAGS "${perl_ccdlflags} ${perl_ldflags} -L${perl_archlib}/CORE"
)

TARGET_LINK_LIBRARIES(${EXE}
    -lperl ${perl_libs}
)

SET_TARGET_PROPERTIES(${EXE}
    PROPERTIES OUTPUT_NAME "wml_p3_${EXE}"
)

INSTALL(TARGETS ${EXE} DESTINATION "${WML_LIBEXE_DIR}")

CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/config_sc.h.in" "${CMAKE_CURRENT_BINARY_DIR}/config_sc.h")

INCLUDE_DIRECTORIES(BEFORE "${CMAKE_CURRENT_BINARY_DIR}")
