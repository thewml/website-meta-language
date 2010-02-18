PREPROCESS_PATH_PERL("ipp_preproc" "ipp.src" "ipp.pl")
INST_POD2MAN("ipp_pod" "ipp.src" "ipp" "1")
INSTALL(
    PROGRAMS "ipp.pl"
    DESTINATION "${WML_LIBEXE_DIR}"
    RENAME "wml_p1_ipp"
)
