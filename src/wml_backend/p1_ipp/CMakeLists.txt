SET(CMAKE_VERBOSE_MAKEFILE ON)
include("../../Common.cmake")

PREPROCESS_PATH_PERL("ipp_preproc" "ipp.src" "ipp.pl")
INST_POD2MAN("ipp_pod" "ipp.src" "ipp" "1")
