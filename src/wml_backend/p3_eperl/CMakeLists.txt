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

SET (PERL_CONFIGS archlib libs usedl ccflags optimize ccdlflags ldflags)
FOREACH (cfg ${PERL_CONFIGS})
    GET_PERL_CONFIG(${cfg})
ENDFOREACH(cfg)

EXECUTE_PROCESS(COMMAND ${PERL_EXECUTABLE}
    "-e" "printf '%.3f',\$]"
    OUTPUT_VARIABLE perl_vers
)

IF(perl_usedl STREQUAL "define")
    # Workaround for perl-5.10.0
    IF (perl_vers LESS 5.010)
        SET (perl_dla "${perl_archlib}/auto/DynaLoader/DynaLoader.a")
    ELSE(perl_vers LESS 5.010)
        SET (perl_dla "")
    ENDIF(perl_vers LESS 5.010)
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
    netinet/in.h
    pwd.h
    sys/param.h
    sys/socket.h
    unistd.h
    )
CHECK_MULTI_INCLUDE_FILES(${INCLUDE_FILES_TO_CHECK})

SET (FUNCTIONS_TO_CHECK
    setegid
    seteuid
    )
CHECK_MULTI_FUNCTIONS_EXISTS(${FUNCTIONS_TO_CHECK})

set(license_cc "${CMAKE_CURRENT_BINARY_DIR}/eperl_license")
set(license_c "${license_cc}.c")
set(license_s "${CMAKE_CURRENT_SOURCE_DIR}/COPYING.short")

set(readme_cc "${CMAKE_CURRENT_BINARY_DIR}/eperl_readme")
set(readme_c "${readme_cc}.c")
set(readme_s "${CMAKE_CURRENT_SOURCE_DIR}/README")

set(logo_cc "${CMAKE_CURRENT_BINARY_DIR}/eperl_logo")
set(logo_c "${logo_cc}.c")
set(logo_s "${CMAKE_CURRENT_SOURCE_DIR}/empty_img.webp")


SET (SOURCES
    ${CMAKE_SOURCE_DIR}/lib/getopt.c
    ${CMAKE_SOURCE_DIR}/lib/getopt1.c
   "${license_c}"
   "${logo_c}"
   "${readme_c}"
    eperl_debug.c
    eperl_http.c
    eperl_main.c
    eperl_parse.c
    eperl_perl5.c
    eperl_pp.c
    eperl_sys.c
)
RUN_ASC2C("${readme_s}" "${readme_cc}" "ePerl_README")
RUN_ASC2C("${license_s}" "${license_cc}" "ePerl_LICENSE")
ADD_CUSTOM_COMMAND(
    COMMAND ${PERL_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/bin2c-new.pl ${logo_s} "${logo_cc}" "ePerl_LOGO"
    OUTPUT "${logo_c}"
    DEPENDS ${logo_s}
)

SET(EXE "eperl")
ADD_EXECUTABLE(${EXE}
    ${SOURCES}
)

SET(SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/eperl_perl5_sm.pl")
SET(HEADER "${CMAKE_CURRENT_BINARY_DIR}/eperl_perl5_sm.h")
ADD_CUSTOM_COMMAND(
    OUTPUT ${HEADER}
    COMMAND ${PATH_PERL}
    ARGS "${SCRIPT}" "-o" "${HEADER}"
    DEPENDS ${SCRIPT}
    VERBATIM
)


SET_SOURCE_FILES_PROPERTIES("eperl_perl5.c"
    PROPERTIES OBJECT_DEPENDS ${HEADER}
)

SET_SOURCE_FILES_PROPERTIES(${SOURCES}
    PROPERTIES COMPILE_FLAGS "${perl_optimize} ${perl_ccflags} -I${perl_archlib}/CORE -I."
)

SET(perl_lib_flags "${perl_ccdlflags} ${perl_ldflags}")
# See:
# https://github.com/thewml/website-meta-language/issues/44
# and other reports - sanity check for gdbm_compat presence.
FOREACH (var "perl_libs" "perl_lib_flags")
    STRING(REGEX REPLACE
        "(^|\$|[ \t])[^ \t]*gdbm[^ \t]*(^|\$|[ \t])"
        " "
        "${var}" "${${var}}")
    STRING(REGEX REPLACE
        "[ \t]+\$"
        ""
        "${var}" "${${var}}")
ENDFOREACH ()
# MESSAGE(FATAL_ERROR "libs = < ${perl_libs} ${perl_lib_flags} >")
SET(perl_match_lib_flags "${perl_lib_flags} ${perl_libs}")
STRING(REGEX MATCH "(gdbm[_\\-]?compat)" perl_needs_gdbm "${perl_match_lib_flags}")
IF (NOT (perl_needs_gdbm STREQUAL ""))
    FIND_LIBRARY(found_gdbm NAMES "gdbm_compat" REQUIRED)
ELSE ()
    STRING(REGEX MATCH "(gdbm)" perl_needs_gdbm "${perl_match_lib_flags}")
    IF (NOT (perl_needs_gdbm STREQUAL ""))
        FIND_LIBRARY(found_gdbm NAMES "gdbm" REQUIRED)
    ENDIF ()
ENDIF ()

SET_TARGET_PROPERTIES(${EXE}
    PROPERTIES LINK_FLAGS "${perl_lib_flags} -L${perl_archlib}/CORE"
)

TARGET_LINK_LIBRARIES(${EXE}
    ${perl_dla} -lperl ${perl_libs}
)

SET_TARGET_PROPERTIES(${EXE}
    PROPERTIES OUTPUT_NAME "wml_p3_${EXE}"
)

SET (POD_PATH  "${CMAKE_CURRENT_BINARY_DIR}/eperl.pod")
ADD_CUSTOM_COMMAND(
    OUTPUT "${POD_PATH}"
    COMMAND "${PERL_EXECUTABLE}"
    ARGS "${CMAKE_CURRENT_SOURCE_DIR}/eperl-pod-preproc.pl"
        "--input" "${CMAKE_CURRENT_SOURCE_DIR}/eperl.proto.pod"
        "--output" "${POD_PATH}"
        "--versionfrom" "${CMAKE_CURRENT_SOURCE_DIR}/eperl_version.h"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/eperl-pod-preproc.pl" "${CMAKE_CURRENT_SOURCE_DIR}/eperl.proto.pod" "${CMAKE_CURRENT_SOURCE_DIR}/eperl_version.h"
)

ADD_CUSTOM_TARGET(
    "eperl_pod_preproc" ALL
    DEPENDS "${POD_PATH}"
)

SET (POD2MAN_SOURCE_IS_IN_BINARY 1)
SET (eperl_pods )
INST_RENAME_POD2MAN( "eperl_pods" "eperl.pod" "eperl" "1" "wml_p3_eperl")
SET (POD2MAN_TARGET_IS_A_LIST)

ADD_CUSTOM_TARGET(
    "eperl_pod" ALL
    DEPENDS "${eperl_pods}"
)

INSTALL(TARGETS ${EXE} DESTINATION "${WML_LIBEXE_DIR}")

CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/config_ac.h.in" "${CMAKE_CURRENT_BINARY_DIR}/config_ac.h")
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/config_sc.h.in" "${CMAKE_CURRENT_BINARY_DIR}/config_sc.h")

INCLUDE_DIRECTORIES(BEFORE "${CMAKE_CURRENT_BINARY_DIR}")
