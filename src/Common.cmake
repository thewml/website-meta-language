# Copyright (c) 2012 Shlomi Fish
#
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation
# files (the "Software"), to deal in the Software without
# restriction, including without limitation the rights to use,
# copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following
# conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
# OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.
#
#
# (This copyright notice applies only to this file)

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

MACRO(PREPROCESS_PATH_PERL_WITH_FULL_NAMES TARGET_NAME SOURCE DEST)
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
ENDMACRO(PREPROCESS_PATH_PERL_WITH_FULL_NAMES)

MACRO(PREPROCESS_PATH_PERL TARGET_NAME BASE_SOURCE BASE_DEST)
    SET (DEST "${CMAKE_CURRENT_BINARY_DIR}/${BASE_DEST}")
    SET (SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/${BASE_SOURCE}")
    PREPROCESS_PATH_PERL_WITH_FULL_NAMES ("${TARGET_NAME}" "${SOURCE}" "${DEST}")
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

MACRO(RUN_POD2MAN TARGET_NAME BASE_SOURCE BASE_DEST SECTION CENTER RELEASE)
    SET (DEST "${CMAKE_CURRENT_BINARY_DIR}/${BASE_DEST}")
    IF (POD2MAN_SOURCE_IS_IN_BINARY)
        SET (SOURCE "${CMAKE_CURRENT_BINARY_DIR}/${BASE_SOURCE}")
    ELSE (POD2MAN_SOURCE_IS_IN_BINARY)
        SET (SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/${BASE_SOURCE}")
    ENDIF (POD2MAN_SOURCE_IS_IN_BINARY)
    # It is null by default.
    SET (POD2MAN_SOURCE_IS_IN_BINARY )
    SET(PATH_PERL ${PERL_EXECUTABLE})
    ADD_CUSTOM_COMMAND(
        OUTPUT "${DEST}"
        COMMAND "${PATH_PERL}"
        ARGS "${CMAKE_SOURCE_DIR}/cmake/pod2man-wrapper.pl"
            "--src" "${SOURCE}"
            "--dest" "${DEST}"
            "--section" "${SECTION}"
            "--center" "${CENTER}"
            "--release" "${RELEASE}"
        DEPENDS ${SOURCE}
        VERBATIM
    )
    # The custom command needs to be assigned to a target.
    IF (POD2MAN_TARGET_IS_A_LIST)
        LIST(APPEND "${TARGET_NAME}" "${DEST}")
    ELSE (POD2MAN_TARGET_IS_A_LIST)
        ADD_CUSTOM_TARGET(
            ${TARGET_NAME} ALL
            DEPENDS ${DEST}
        )
    ENDIF (POD2MAN_TARGET_IS_A_LIST)
ENDMACRO(RUN_POD2MAN)

MACRO(SIMPLE_POD2MAN TARGET_NAME SOURCE DEST SECTION)
   RUN_POD2MAN("${TARGET_NAME}" "${SOURCE}" "${DEST}.${SECTION}"
       "${SECTION}"
       "EN Tools" "EN Tools"
   )
ENDMACRO(SIMPLE_POD2MAN)

MACRO(INST_POD2MAN TARGET_NAME SOURCE DEST SECTION)
   SIMPLE_POD2MAN ("${TARGET_NAME}" "${SOURCE}" "${DEST}" "${SECTION}")
   INSTALL_MAN ("${CMAKE_CURRENT_BINARY_DIR}/${DEST}.${SECTION}" "${SECTION}")
ENDMACRO(INST_POD2MAN)

MACRO(INST_RENAME_POD2MAN TARGET_NAME SOURCE DEST SECTION INSTNAME)
   SIMPLE_POD2MAN ("${TARGET_NAME}" "${SOURCE}" "${DEST}" "${SECTION}")
   INSTALL_RENAME_MAN ("${DEST}.${SECTION}" "${SECTION}" "${INSTNAME}" "${CMAKE_CURRENT_BINARY_DIR}")
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

MACRO(INSTALL_RENAME_MAN SOURCE SECTION INSTNAME MAN_SOURCE_DIR)
    INSTALL(
        FILES
            "${MAN_SOURCE_DIR}/${SOURCE}"
        DESTINATION
            "share/man/man${SECTION}"
        RENAME
            "${INSTNAME}.${SECTION}"
    )
ENDMACRO(INSTALL_RENAME_MAN)

MACRO(INSTALL_CAT_MAN SOURCE SECTION)
    INSTALL(
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/${SOURCE}"
        DESTINATION
            "share/man/cat${SECTION}"
    )
ENDMACRO(INSTALL_CAT_MAN)

MACRO(DEFINE_WML_AUX_PERL_PROG_WITHOUT_MAN BASENAME)
    PREPROCESS_PATH_PERL("preproc_${BASENAME}" "${BASENAME}.src" "${BASENAME}.pl")
    INSTALL(
        PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/${BASENAME}.pl"
        DESTINATION "${WML_LIBEXE_DIR}"
        RENAME "wml_aux_${BASENAME}"
    )
ENDMACRO(DEFINE_WML_AUX_PERL_PROG_WITHOUT_MAN BASENAME)

MACRO(DEFINE_WML_AUX_PERL_PROG BASENAME)
    DEFINE_WML_AUX_PERL_PROG_WITHOUT_MAN("${BASENAME}")
    RUN_POD2MAN("pod_${BASENAME}" "${BASENAME}.src" "${BASENAME}.1" "1" "EN  Tools" "En Tools")
    INSTALL_RENAME_MAN ("${BASENAME}.1" 1 "wml_aux_${BASENAME}" "${CMAKE_CURRENT_BINARY_DIR}")
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

MACRO(DEFINE_WML_AUX_C_PROG BASENAME MAN_SOURCE_DIR)
    DEFINE_WML_AUX_C_PROG_WITHOUT_MAN (${BASENAME} ${ARGN})
    INSTALL_RENAME_MAN ("${BASENAME}.1" 1 "wml_aux_${BASENAME}" "${MAN_SOURCE_DIR}")
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
        PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/${BASENAME}.pl"
        DESTINATION "${WML_LIBEXE_DIR}"
        RENAME "${DEST_BASENAME}"
    )
ENDMACRO(DEFINE_WML_PERL_BACKEND)

MACRO(CHOMP VAR)
    STRING(REGEX REPLACE "[\r\n]+$" "" ${VAR} "${${VAR}}")
ENDMACRO(CHOMP)
