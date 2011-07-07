SET (STDS
    "wml_barebone"
    "wml_std_css1"
    "wml_std_css2"
    "wml_std_csspos"
    "wml_std_html20"
    "wml_std_html32"
    "wml_std_html40"
    "wml_std_html401"
    "wml_std_wai10"
    "wml_std_xhtml10"
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

    ADD_COPY_TARGET("std_copy_${MYSTD}" "${TXT_SRC}" "${MAN7_LOCAL}")

    INSTALL_CAT_MAN("${MAN7_LOCAL}" "7")

ENDFOREACH(MYSTD ${STDS})

SET (TAGS_POD "${CMAKE_CURRENT_BINARY_DIR}/wml_tags.pod")
ADD_CUSTOM_COMMAND(
    OUTPUT "${TAGS_POD}"
    COMMAND ${PERL_EXECUTABLE}
    ARGS "${CMAKE_CURRENT_SOURCE_DIR}/wml_tags.pod.pl"
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

FOREACH(MYDOC ${WML_DOCS})
    SET (POD_SRC "${MYDOC}.pod")
    SET (MAN_LOCAL "${MYDOC}")

    INST_POD2MAN("wml_doc_${MYDOC}_pod" "${POD_SRC}" "${MAN_LOCAL}" "7")
ENDFOREACH(MYDOC ${WML_DOCS})

