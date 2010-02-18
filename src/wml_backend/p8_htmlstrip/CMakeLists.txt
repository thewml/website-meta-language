PREPROCESS_PATH_PERL("htmlstrip_preproc" "htmlstrip.src" "htmlstrip.pl")
INST_POD2MAN("htmlstrip_pod" "htmlstrip.src" "htmlstrip" "1")
INSTALL(
    PROGRAMS "htmlstrip.pl"
    DESTINATION "${WML_LIBEXE_DIR}"
    RENAME "wml_p8_htmlstrip"
)
