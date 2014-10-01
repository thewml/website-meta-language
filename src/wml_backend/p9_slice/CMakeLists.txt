SET(BASENAME "slice")
SET(src "${CMAKE_CURRENT_SOURCE_DIR}/${BASENAME}_main.pl")
SET(intermediatebase "${BASENAME}.pre.pl")
SET(intermediate "${CMAKE_CURRENT_BINARY_DIR}/${intermediatebase}")
SET(destbase "${BASENAME}.pl")
SET(dest "${CMAKE_CURRENT_BINARY_DIR}/${destbase}")

SET (yyperl "${CMAKE_CURRENT_SOURCE_DIR}/slice_term.pl")
SET (yytab "${CMAKE_CURRENT_SOURCE_DIR}/SliceTermParser.pm")

ADD_CUSTOM_COMMAND(
    OUTPUT "${yyperl}"
    COMMAND "${PERL_EXECUTABLE}"
    ARGS "-lap" "${CMAKE_CURRENT_SOURCE_DIR}/etc/p1.pl" "<" "${yytab}"
        ">" "${yyperl}"
    DEPENDS "${yytab}"
)

ADD_CUSTOM_COMMAND(
    OUTPUT "${yytab}"
    COMMAND "pbyacc"
    ARGS "-l" "-P" "SliceTermParser" "slice_term.y"
    COMMAND "${PERL_EXECUTABLE}"
    ARGS "-i" "-lap" "-e" "s/ +\\z//ms" "./SliceTermParser.pm"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/slice_term.y"
)

ADD_CUSTOM_COMMAND(
    OUTPUT "${intermediate}"
    COMMAND "${PERL_EXECUTABLE}"
    ARGS "${CMAKE_CURRENT_SOURCE_DIR}/etc/expand.pl"
         "${src}"
         "${intermediate}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    DEPENDS "${src}"
    "${CMAKE_CURRENT_SOURCE_DIR}/slice_term.pl"
)

ADD_CUSTOM_TARGET(
    "${BASENAME}_preproc_2" ALL
    DEPENDS "${intermediate}"
)

PREPROCESS_PATH_PERL_WITH_FULL_NAMES(
    "${BASENAME}_preproc" "${intermediate}" "${dest}"
)

SET(INST_BASE "wml_p9_${BASENAME}")

SET (POD2MAN_SOURCE_IS_IN_BINARY)
SET (pod_dests )
INST_RENAME_POD2MAN(
    "pod_dests" "${BASENAME}.pod" "${BASENAME}" "1" "${INST_BASE}"
)

ADD_CUSTOM_TARGET(
    "${BASENAME}_pod" ALL
    DEPENDS ${pod_dests}
)

INSTALL(
    PROGRAMS "${dest}"
    DESTINATION "${WML_LIBEXE_DIR}"
    RENAME "${INST_BASE}"
)
