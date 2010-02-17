SET(CMAKE_VERBOSE_MAKEFILE ON)
include("../../Common.cmake")

PREPROCESS_PATH_PERL("asubst_preproc" "asubst.src" "asubst.pl")
INST_POD2MAN("asubst_pod" "asubst.src" "asubst" "1")
