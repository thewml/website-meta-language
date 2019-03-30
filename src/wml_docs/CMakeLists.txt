SET (STDS
    "wml_barebone"
    )

SET (WML_DOCS
    "wml_intro"
    "wml_tutorial"
    "wml_macros"
    "wml_faq"
    "wml_tags"
)

FOREACH(MYSTD ${STDS})
    SET (TXT_SRC "${MYSTD}.txt")
    SET (MAN7_LOCAL "${MYSTD}.7")

    # MESSAGE (FATAL_ERROR "${CMAKE_CURRENT_BINARY_DIR}/${MAN7_LOCAL}")
    ADD_COPY_TARGET("std_copy_${MYSTD}" "${CMAKE_CURRENT_SOURCE_DIR}/${TXT_SRC}" "${CMAKE_CURRENT_BINARY_DIR}/${MAN7_LOCAL}")

    INSTALL_CAT_MAN("${MAN7_LOCAL}" "7")

ENDFOREACH(MYSTD ${STDS})

# MESSAGE (FATAL_ERROR "CUR_BIN = ${CMAKE_CURRENT_BINARY_DIR}")
SET (TAGS_POD "${CMAKE_CURRENT_BINARY_DIR}/wml_tags.pod")
ADD_CUSTOM_COMMAND(
    OUTPUT "${TAGS_POD}"
    COMMAND ${PERL_EXECUTABLE}
    ARGS "${CMAKE_CURRENT_SOURCE_DIR}/wml_tags.pod.pl"
        "--src" "${CMAKE_CURRENT_SOURCE_DIR}/wml_tags.pod.in"
        "--dest" "${TAGS_POD}"
        "--main" "${CMAKE_CURRENT_SOURCE_DIR}/wml_tags.L.main"
        "--incl" "${CMAKE_CURRENT_SOURCE_DIR}/wml_tags.L.incl"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/wml_tags.pod.in"
            "${CMAKE_CURRENT_SOURCE_DIR}/wml_tags.L.main"
            "${CMAKE_CURRENT_SOURCE_DIR}/wml_tags.L.incl"
    VERBATIM
)

# The custom command needs to be assigned to a target.
ADD_CUSTOM_TARGET(
    ${TARGET_NAME} ALL
    DEPENDS ${TAGS_POD}
)

SET (pod_dests )

FOREACH(MYDOC ${WML_DOCS})
    SET (POD_SRC "${MYDOC}.pod")
    SET (MAN_LOCAL "${MYDOC}")

    IF (${MYDOC} STREQUAL "wml_tags")
        SET (POD2MAN_SOURCE_IS_IN_BINARY 1)
    ENDIF (${MYDOC} STREQUAL "wml_tags")
    INST_POD2MAN("pod_dests" "${POD_SRC}" "${MAN_LOCAL}" "7")
ENDFOREACH(MYDOC ${WML_DOCS})

ADD_CUSTOM_TARGET(
    "docs_pods" ALL
    DEPENDS ${pod_dests}
)
