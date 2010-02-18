PREPROCESS_PATH_PERL("divert_preproc" "divert.src" "divert.pl")
INST_POD2MAN("divert_pod" "divert.src" "divert" "1")
INSTALL(
    PROGRAMS "divert.pl"
    DESTINATION "${WML_LIBEXE_DIR}"
    RENAME "wml_p5_divert"
)
