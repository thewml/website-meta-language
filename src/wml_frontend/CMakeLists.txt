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

FOREACH (cmd wmk wml wmb wmd wmu)
    set_vars()
    # MESSAGE( FATAL_ERROR "WML_LIB_DIR = ${WML_LIB_DIR}")
    ADD_CUSTOM_COMMAND(
        OUTPUT "${dest}"
        COMMAND "${PERL_EXECUTABLE}"
        ARGS "${CMAKE_CURRENT_SOURCE_DIR}/preprocess-perl-source.pl"
            "--input" "${src}"
            "--output" "${dest}"
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
        DEPENDS "${src}"
    )
    INSTALL (
        PROGRAMS "${dest}"
        DESTINATION "bin"
        RENAME "${cmd}"
        )
    INST_POD2MAN("${cmd}_pod" "${srcbase}" "${mannamebase}" "1")
    ADD_CUSTOM_TARGET(
        "preprocess_perl_frontend_${cmd}" ALL
        DEPENDS "${dest}"
    )
ENDFOREACH(cmd)

FOREACH(cmd wmd_missing)
    set_vars()
    INST_POD2MAN("${cmd}_pod" "${srcbase}" "${mannamebase}" "1")
ENDFOREACH(cmd)

INSTALL_DATA ("wmd.txt")
