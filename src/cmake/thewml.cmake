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
MACRO(PREPROCESS_PATH_PERL_WITH_FULL_NAMES TARGET_NAME SOURCE DEST)
    ADD_CUSTOM_COMMAND(
        OUTPUT "${DEST}"
        COMMAND "${PERL_EXECUTABLE}"
        ARGS "${CMAKE_SOURCE_DIR}/cmake/preprocess-path-perl.pl"
            "--input" "${SOURCE}"
            "--output" "${DEST}"
            "--subst" "WML_VERSION=${VERSION}"
            "--subst" "WML_CONFIG_ARGS="
            "--subst" "perlprog=${PERL_EXECUTABLE}"
            "--subst" "perlvers=${PERL_EXECUTABLE}"
            "--subst" "built_system=${CMAKE_SYSTEM_NAME}"
            "--subst" "built_user=${username}"
            "--subst" "built_date=${date}"
            "--subst" "prefix=${CMAKE_INSTALL_PREFIX}"
            "--subst" "bindir=${CMAKE_INSTALL_PREFIX}/bin"
            "--subst" "libdir=${CMAKE_INSTALL_PREFIX}/${WML_LIB_DIR}"
            "--subst" "mandir=${CMAKE_INSTALL_PREFIX}/share/man"
            "--subst" "PATH_PERL=${PERL_EXECUTABLE}"
            "--subst" "INSTALLPRIVLIB=${PKGDATADIR}"
            "--subst" "INSTALLARCHLIB=${CMAKE_INSTALL_PREFIX}/${WML_LIB_DIR}"
            ${PREPROCESS_PATH_PERL__ARGS}
        COMMAND chmod ARGS "a+x" "${DEST}"
        DEPENDS "${SOURCE}"
    )
    # The custom command needs to be assigned to a target.
    ADD_CUSTOM_TARGET(
        ${TARGET_NAME} ALL
        DEPENDS ${DEST}
    )
ENDMACRO()

MACRO(PREPROCESS_PATH_PERL TGT BASE_SOURCE BASE_DEST)
    PREPROCESS_PATH_PERL_WITH_FULL_NAMES ("${TGT}" "${CMAKE_CURRENT_SOURCE_DIR}/${BASE_SOURCE}" "${CMAKE_CURRENT_BINARY_DIR}/${BASE_DEST}")
ENDMACRO()

MACRO(DEFINE_WML_AUX_PERL_PROG_WITHOUT_MAN BASENAME)
    PREPROCESS_PATH_PERL("preproc_${BASENAME}" "${BASENAME}.src" "${BASENAME}.pl")
    INSTALL(
        PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/${BASENAME}.pl"
        DESTINATION "${WML_LIBEXE_DIR}"
        RENAME "wml_aux_${BASENAME}"
    )
ENDMACRO()

MACRO(DEFINE_WML_AUX_PERL_PROG BASENAME)
    DEFINE_WML_AUX_PERL_PROG_WITHOUT_MAN("${BASENAME}")
    SET (aux_pod_dests )
    RUN_POD2MAN("aux_pod_dests" "${BASENAME}.src" "${BASENAME}.1" "1" "EN  Tools" "En Tools")
    INSTALL_RENAME_MAN ("${BASENAME}.1" 1 "wml_aux_${BASENAME}" "${CMAKE_CURRENT_BINARY_DIR}")
    ADD_CUSTOM_TARGET(
        "pod_${BASENAME}" ALL
        DEPENDS ${aux_pod_dests}
    )
ENDMACRO()

MACRO(DEFINE_WML_AUX_C_PROG_WITHOUT_MAN BASENAME)
    ADD_EXECUTABLE(${BASENAME} ${ARGN})
    SET_TARGET_PROPERTIES("${BASENAME}"
        PROPERTIES OUTPUT_NAME "wml_aux_${BASENAME}"
    )
    INSTALL(
        TARGETS "${BASENAME}"
        DESTINATION "${WML_LIBEXE_DIR}"
    )
ENDMACRO()

MACRO(DEFINE_WML_AUX_C_PROG BASENAME MAN_SOURCE_DIR)
    DEFINE_WML_AUX_C_PROG_WITHOUT_MAN (${BASENAME} ${ARGN})
    INSTALL_RENAME_MAN ("${BASENAME}.1" 1 "wml_aux_${BASENAME}" "${MAN_SOURCE_DIR}")
ENDMACRO()

MACRO(DEFINE_WML_PERL_BACKEND BASENAME DEST_BASENAME)
    PREPROCESS_PATH_PERL(
        "${BASENAME}_preproc" "${BASENAME}.src" "${BASENAME}.pl"
    )
    SET (perl_backend_pod_tests )
    INST_RENAME_POD2MAN(
        "perl_backend_pod_tests" "${BASENAME}.src" "${BASENAME}" "1"
        "${DEST_BASENAME}"
    )
    ADD_CUSTOM_TARGET(
        "${BASENAME}_pod" ALL
        DEPENDS ${perl_backend_pod_tests}
    )
    INSTALL(
        PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/${BASENAME}.pl"
        DESTINATION "${WML_LIBEXE_DIR}"
        RENAME "${DEST_BASENAME}"
    )
ENDMACRO()

