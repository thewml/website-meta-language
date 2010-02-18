SET(CMAKE_VERBOSE_MAKEFILE ON)
include("../../Common.cmake")

PREPROCESS_PATH_PERL("htmlfix_preproc" "htmlfix.src" "htmlfix.pl")
INST_POD2MAN("htmlfix_pod" "htmlfix.src" "htmlfix" "1")
INSTALL(
    PROGRAMS "htmlfix.pl"
    DESTINATION "${WML_LIBEXE_DIR}"
    RENAME "wml_p7_htmlfix"
)
