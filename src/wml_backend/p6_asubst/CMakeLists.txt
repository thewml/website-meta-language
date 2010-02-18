SET(CMAKE_VERBOSE_MAKEFILE ON)
include("../../Common.cmake")

PREPROCESS_PATH_PERL("asubst_preproc" "asubst.src" "asubst.pl")
INST_POD2MAN("asubst_pod" "asubst.src" "asubst" "1")
INSTALL(
    PROGRAMS "asubst.pl"
    DESTINATION "${WML_LIBEXE_DIR}"
    RENAME "wml_p6_asubst"
)
