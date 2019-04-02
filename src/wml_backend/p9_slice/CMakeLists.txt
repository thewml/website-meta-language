SET(BASENAME "slice")
SET(src "${CMAKE_CURRENT_SOURCE_DIR}/${BASENAME}_main.pl")
SET(destbase "${BASENAME}.pl")
SET(dest "${CMAKE_CURRENT_BINARY_DIR}/${destbase}")

SET (yytab_base "SliceTermParser.pm")

SET (yytab "${CMAKE_CURRENT_SOURCE_DIR}/${yytab_base}")
# Workaround for git clone SNAFU.
EXECUTE_PROCESS(
    COMMAND "cmake" "-E" "touch_nocreate" "${yytab}"
)

ADD_CUSTOM_COMMAND(
    OUTPUT "${yytab}"
    COMMAND "pbyacc"
    ARGS "-l" "-P" "SliceTermParser" "slice_term.y"
    COMMAND "${PERL_EXECUTABLE}"
    ARGS "-i" "-lp" "./process-SliceTerm.pl" "./SliceTermParser.pm"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/slice_term.y"
)

INSTALL (
    FILES "${yytab}"
    DESTINATION "${PKGDATADIR}"
)

PREPROCESS_PATH_PERL_WITH_FULL_NAMES(
    "${BASENAME}_preproc" "${src}" "${dest}"
)

SET(INST_BASE "wml_p9_${BASENAME}")

SET (POD2MAN_SOURCE_IS_IN_BINARY)
SET (pod_dests )
INST_RENAME_POD2MAN(
    "pod_dests" "${BASENAME}.pod" "${BASENAME}" "1" "${INST_BASE}"
)

ADD_CUSTOM_TARGET(
    "${BASENAME}_pod" ALL
    DEPENDS ${pod_dests} ${yytab}
)

INSTALL(
    PROGRAMS "${dest}"
    DESTINATION "${WML_LIBEXE_DIR}"
    RENAME "${INST_BASE}"
)
