SET(BASENAME "slice")
SET(src "${CMAKE_CURRENT_SOURCE_DIR}/${BASENAME}_main.pl")
SET(intermediatebase "${BASENAME}.pre.pl")
SET(intermediate "${CMAKE_CURRENT_BINARY_DIR}/${intermediatebase}")
SET(destbase "${BASENAME}.pl")
SET(dest "${CMAKE_CURRENT_BINARY_DIR}/${destbase}")

ADD_CUSTOM_COMMAND(
    OUTPUT "${intermediate}"
    COMMAND "${PERL_EXECUTABLE}"
    ARGS "${CMAKE_CURRENT_SOURCE_DIR}/etc/expand.pl"
         "${src}"
         "${intermediate}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    DEPENDS "${src}"
)

ADD_CUSTOM_TARGET(
    "${BASENAME}_preproc_2" ALL
    DEPENDS "${intermediate}"
)

PREPROCESS_PATH_PERL_WITH_FULL_NAMES(
    "${BASENAME}_preproc" "${intermediate}" "${dest}"
)

SET(INST_BASE "wml_p9_${BASENAME}")

INST_RENAME_POD2MAN(
    "${BASENAME}_pod" "${destbase}" "${BASENAME}" "1" "${INST_BASE}"
)
INSTALL(
    PROGRAMS "${dest}"
    DESTINATION "${WML_LIBEXE_DIR}"
    RENAME "${INST_BASE}"
)
