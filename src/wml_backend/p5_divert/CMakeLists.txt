SET(CMAKE_VERBOSE_MAKEFILE ON)
include("../../Common.cmake")

PREPROCESS_PATH_PERL("divert_preproc" "divert.src" "divert.pl")
RUN_POD2MAN("divert_pod" "divert.src" "divert.1" "1" "EN Tools" "EN Tools")
