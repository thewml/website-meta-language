SET(BASENAME "slice")
SET(src "${CMAKE_CURRENT_SOURCE_DIR}/${BASENAME}_main.pl")
SET(dest "${CMAKE_CURRENT_BINARY_DIR}/${BASENAME}.pl")

ADD_CUSTOM_COMMAND(
    OUTPUT "${dest}"
    COMMAND "${PERL_EXECUTABLE}"
    ARGS "${CMAKE_CURRENT_SOURCE_DIR}/etc/expand.pl"
         "${src}"
         "${dest}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    DEPENDS "${src}"
)
ADD_CUSTOM_TARGET(
    "${BASENAME}_preproc" ALL
    DEPENDS "${dest}"
)

SET(INST_BASE "wml_p9_${BASENAME}")

INST_RENAME_POD2MAN(
    "${BASENAME}_pod" "${dest}" "${BASENAME}" "1" "${INST_BASE}"
)
INSTALL(
    PROGRAMS "${dest}"
    DESTINATION "${WML_LIBEXE_DIR}"
    RENAME "${INST_BASE}"
)
