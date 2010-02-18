PREPROCESS_PATH_PERL("slice_preproc" "slice_main.pl" "slice")
INST_POD2MAN("slice_pod" "slice.pod" "slice" "1")
INSTALL(
    PROGRAMS "slice"
    DESTINATION "${WML_LIBEXE_DIR}"
    RENAME "wml_p9_slice"
)
