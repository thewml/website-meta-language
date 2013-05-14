DEFINE_WML_AUX_PERL_PROG_WITHOUT_MAN ("txt2html")

CONFIGURE_FILE ("txt2html.txt" "wml_aux_txt2html.1" COPYONLY)

INSTALL_CAT_MAN( "wml_aux_txt2html.1" "1")

INSTALL(
    FILES "txt2html.dict"
    DESTINATION "${WML_DATA_DIR}"
)
