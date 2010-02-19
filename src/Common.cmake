include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckFunctionExists)
include(FindPerl)
IF (NOT PERL_FOUND)
    MESSAGE ( FATAL_ERROR "perl must be installed")
ENDIF(NOT PERL_FOUND)

# Taken from http://www.cmake.org/pipermail/cmake/2007-March/013060.html
MACRO(REPLACE_FUNCTIONS sources)
  FOREACH(name ${ARGN})
    STRING(TOUPPER have_${name} SYMBOL_NAME)
    CHECK_FUNCTION_EXISTS(${name} ${SYMBOL_NAME})
    IF(NOT ${SYMBOL_NAME})
      SET(${sources} ${${sources}} ${name}.c)
    ENDIF(NOT ${SYMBOL_NAME})
  ENDFOREACH(name)
ENDMACRO(REPLACE_FUNCTIONS)

MACRO(CHECK_MULTI_INCLUDE_FILES)
  FOREACH(name ${ARGN})
    STRING(TOUPPER have_${name} SYMBOL_NAME)
    STRING(REGEX REPLACE "\\." "_" SYMBOL_NAME ${SYMBOL_NAME})
    STRING(REGEX REPLACE "/" "_" SYMBOL_NAME ${SYMBOL_NAME})
    CHECK_INCLUDE_FILE(${name} ${SYMBOL_NAME})
  ENDFOREACH(name)
ENDMACRO(CHECK_MULTI_INCLUDE_FILES)

MACRO(CHECK_MULTI_FUNCTIONS_EXISTS)
  FOREACH(name ${ARGN})
    STRING(TOUPPER have_${name} SYMBOL_NAME)
    CHECK_FUNCTION_EXISTS(${name} ${SYMBOL_NAME})
  ENDFOREACH(name)
ENDMACRO(CHECK_MULTI_FUNCTIONS_EXISTS)

MACRO(PREPROCESS_PATH_PERL TARGET_NAME SOURCE DEST)
    ADD_CUSTOM_COMMAND(
        OUTPUT "${DEST}"
        COMMAND "${PERL_EXECUTABLE}"
        ARGS "${CMAKE_SOURCE_DIR}/cmake/preprocess-path-perl.pl" 
            "--input" "${SOURCE}"
            "--output" "${DEST}"
            "--subst" "PATH_PERL=${PERL_EXECUTABLE}"
        COMMAND chmod ARGS "a+x" "${DEST}"
        DEPENDS "${SOURCE}"
    )
    # The custom command needs to be assigned to a target.
    ADD_CUSTOM_TARGET(
        ${TARGET_NAME} ALL
        DEPENDS ${DEST}
    )
ENDMACRO(PREPROCESS_PATH_PERL)

# Copies the file from one place to the other.
# TARGET_NAME is the name of the makefile target to add.
# SOURCE is the source path.
# DEST is the destination path.
MACRO(ADD_COPY_TARGET TARGET_NAME SOURCE DEST)
    SET(PATH_PERL ${PERL_EXECUTABLE})
    ADD_CUSTOM_COMMAND(
        OUTPUT ${DEST}
        COMMAND ${PATH_PERL}
        ARGS "-e" 
        "my (\$src, \$dest) = @ARGV; use File::Copy; copy(\$src, \$dest);"
        ${SOURCE}
        ${DEST}
        DEPENDS ${SOURCE}
        VERBATIM
    )
    # The custom command needs to be assigned to a target.
    ADD_CUSTOM_TARGET(
        ${TARGET_NAME} ALL
        DEPENDS ${DEST}
    )
ENDMACRO(ADD_COPY_TARGET)

MACRO(RUN_POD2MAN TARGET_NAME SOURCE DEST SECTION CENTER RELEASE)
    SET(PATH_PERL ${PERL_EXECUTABLE})
    ADD_CUSTOM_COMMAND(
        OUTPUT ${DEST}
        COMMAND ${PATH_PERL} 
        ARGS "-e" 
        "my (\$src, \$dest, \$sect, \$center, \$release) = @ARGV; my \$pod = qq{Hoola.pod}; use File::Copy; copy(\$src, \$pod); system(qq{pod2man --section=\$sect --center=\"\$center\" --release=\"\$release\" \$pod > \$dest}); unlink(\$pod)"
        ${SOURCE}
        ${DEST}
        ${SECTION}
        "${CENTER}"
        "${RELEASE}"
        DEPENDS ${SOURCE}
        VERBATIM
    )
    # The custom command needs to be assigned to a target.
    ADD_CUSTOM_TARGET(
        ${TARGET_NAME} ALL
        DEPENDS ${DEST}
    )
ENDMACRO(RUN_POD2MAN)

MACRO(SIMPLE_POD2MAN TARGET_NAME SOURCE DEST SECTION)
   RUN_POD2MAN("${TARGET_NAME}" "${SOURCE}" "${DEST}.${SECTION}"
       "${SECTION}"
       "EN Tools" "EN Tools"
   )    
ENDMACRO(SIMPLE_POD2MAN)

MACRO(INST_POD2MAN TARGET_NAME SOURCE DEST SECTION)
   SIMPLE_POD2MAN ("${TARGET_NAME}" "${SOURCE}" "${DEST}" "${SECTION}")
   INSTALL_MAN ("${DEST}.${SECTION}" "${SECTION}")
ENDMACRO(INST_POD2MAN)

MACRO(INST_RENAME_POD2MAN TARGET_NAME SOURCE DEST SECTION INSTNAME)
   SIMPLE_POD2MAN ("${TARGET_NAME}" "${SOURCE}" "${DEST}" "${SECTION}")
   INSTALL_RENAME_MAN ("${DEST}.${SECTION}" "${SECTION}" "${INSTNAME}")
ENDMACRO(INST_RENAME_POD2MAN)

# Finds libm and puts the result in the MATH_LIB_LIST variable.
# If it cannot find it, it fails with an error.
MACRO(FIND_LIBM)
    IF (UNIX)
        FIND_LIBRARY(LIBM_LIB m)
        IF(LIBM_LIB STREQUAL "LIBM_LIB-NOTFOUND")
            MESSAGE(FATAL_ERROR "Cannot find libm")
        ELSE(LIBM_LIB STREQUAL "LIBM_LIB-NOTFOUND")
            SET(MATH_LIB_LIST ${LIBM_LIB})
        ENDIF(LIBM_LIB STREQUAL "LIBM_LIB-NOTFOUND")
    ELSE(UNIX)
        SET(MATH_LIB_LIST)
    ENDIF(UNIX)
ENDMACRO(FIND_LIBM)

MACRO(INSTALL_MAN SOURCE SECTION)
    INSTALL(
        FILES
            ${SOURCE}
        DESTINATION
            "share/man/man${SECTION}"
    )
ENDMACRO(INSTALL_MAN)

MACRO(INSTALL_DATA SOURCE)
    INSTALL(
        FILES
            "${SOURCE}"
        DESTINATION
            "${WML_DATA_DIR}"
    )
ENDMACRO(INSTALL_DATA)

MACRO(INSTALL_RENAME_MAN SOURCE SECTION INSTNAME)
    INSTALL(
        FILES
            "${SOURCE}"
        DESTINATION
            "share/man/man${SECTION}"
        RENAME
            "${INSTNAME}.${SECTION}"
    )
ENDMACRO(INSTALL_RENAME_MAN)

MACRO(INSTALL_CAT_MAN SOURCE SECTION)
    INSTALL(
        FILES
            ${SOURCE}
        DESTINATION
            "share/man/cat${SECTION}"
    )
ENDMACRO(INSTALL_CAT_MAN)

MACRO(DEFINE_WML_AUX_PERL_PROG_WITHOUT_MAN BASENAME)
    PREPROCESS_PATH_PERL("preproc_${BASENAME}" "${BASENAME}.src" "${BASENAME}.pl")
    INSTALL(
        PROGRAMS "${BASENAME}.pl"
        DESTINATION "${WML_LIBEXE_DIR}"
        RENAME "wml_aux_${BASENAME}"
    )
ENDMACRO(DEFINE_WML_AUX_PERL_PROG_WITHOUT_MAN BASENAME)

MACRO(DEFINE_WML_AUX_PERL_PROG BASENAME)
    DEFINE_WML_AUX_PERL_PROG_WITHOUT_MAN("${BASENAME}")
    RUN_POD2MAN("pod_${BASENAME}" "${BASENAME}.src" "${BASENAME}.1" "1" "EN  Tools" "En Tools")
    INSTALL_RENAME_MAN ("${BASENAME}.1" 1 "wml_aux_${BASENAME}")
ENDMACRO(DEFINE_WML_AUX_PERL_PROG BASENAME)

MACRO(DEFINE_WML_AUX_C_PROG_WITHOUT_MAN BASENAME)
    ADD_EXECUTABLE(${BASENAME} ${ARGN})
    SET_TARGET_PROPERTIES("${BASENAME}" 
        PROPERTIES OUTPUT_NAME "wml_aux_${BASENAME}"
    )
    INSTALL(
        TARGETS "${BASENAME}"
        DESTINATION "${WML_LIBEXE_DIR}"
    )
ENDMACRO(DEFINE_WML_AUX_C_PROG_WITHOUT_MAN BASENAME)

MACRO(DEFINE_WML_AUX_C_PROG BASENAME)
    DEFINE_WML_AUX_C_PROG_WITHOUT_MAN (${BASENAME} ${ARGN})
    INSTALL_RENAME_MAN ("${BASENAME}.1" 1 "wml_aux_${BASENAME}")
ENDMACRO(DEFINE_WML_AUX_C_PROG BASENAME)

MACRO(DEFINE_WML_PERL_BACKEND BASENAME DEST_BASENAME)
    PREPROCESS_PATH_PERL(
        "${BASENAME}_preproc" "${BASENAME}.src" "${BASENAME}.pl"
    )    
    INST_RENAME_POD2MAN(
        "${BASENAME}_pod" "${BASENAME}.src" "${BASENAME}" "1"
        "${DEST_BASENAME}"
    )
    INSTALL(
        PROGRAMS "${BASENAME}.pl"
        DESTINATION "${WML_LIBEXE_DIR}"
        RENAME "${DEST_BASENAME}"
    )    
ENDMACRO(DEFINE_WML_PERL_BACKEND)
