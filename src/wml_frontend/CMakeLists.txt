SET (username "unknown")

EXECUTE_PROCESS(COMMAND "whoami"
    OUTPUT_VARIABLE username
)

CHOMP(username)

EXECUTE_PROCESS(COMMAND "${PERL_EXECUTABLE}"
    "-MPOSIX" "-e" "print strftime('%Y-%m-%d', localtime())"
    OUTPUT_VARIABLE date
)

MACRO(set_vars)
    SET (destbase "${cmd}.pl")
    SET (dest "${CMAKE_CURRENT_BINARY_DIR}/${destbase}")
    SET (srcbase "${cmd}.src")
    SET (src "${CMAKE_CURRENT_SOURCE_DIR}/${srcbase}")
    SET (mannamebase "${cmd}")
    SET (manname "${CMAKE_CURRENT_BINARY_DIR}/${mannamebase}")
ENDMACRO(set_vars)

SET (pod_dests )

MACRO(PREPROCESS_PATH_PERL_WITH_FULL_NAMES__AND_WITH_POD TARGET_NAME SOURCE DEST)
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
            "--subst" "INSTALLPRIVLIB=${CMAKE_INSTALL_PREFIX}/${WML_LIB_DIR}"
            "--subst" "INSTALLARCHLIB=${CMAKE_INSTALL_PREFIX}/${WML_LIB_DIR}"
            "--discardpod=0"
        COMMAND chmod ARGS "a+x" "${DEST}"
        DEPENDS "${SOURCE}"
    )
    # The custom command needs to be assigned to a target.
    ADD_CUSTOM_TARGET(
        ${TARGET_NAME} ALL
        DEPENDS ${DEST}
    )
ENDMACRO()

FOREACH (cmd wmk wml wmb wmd wmu wml-params-conf)
    set_vars()
    # MESSAGE( FATAL_ERROR "WML_LIB_DIR = ${WML_LIB_DIR}")
    # IF ("${cmd}" STREQUAL "wml-params-conf")
    IF (TRUE)
        PREPROCESS_PATH_PERL_WITH_FULL_NAMES__AND_WITH_POD("preprocess_perl_frontend_${cmd}" "${src}" "${dest}")
    ELSE()
        PREPROCESS_PATH_PERL_WITH_FULL_NAMES("preprocess_perl_frontend_${cmd}" "${src}" "${dest}")
    ENDIF()
    INSTALL (
        PROGRAMS "${dest}"
        DESTINATION "bin"
        RENAME "${cmd}"
        )
    SET (POD2MAN_SOURCE_IS_IN_BINARY "1")
    INST_POD2MAN("pod_dests" "${destbase}" "${mannamebase}" "1")
ENDFOREACH(cmd)

FOREACH(cmd wmd_missing)
    set_vars()
    INST_POD2MAN("pod_dests" "${srcbase}" "${mannamebase}" "1")
ENDFOREACH(cmd)

ADD_CUSTOM_TARGET(
    "frontend_pods" ALL
    DEPENDS ${pod_dests}
)

INSTALL_DATA ("wmd.txt")
