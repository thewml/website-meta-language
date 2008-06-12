SET(CMAKE_VERBOSE_MAKEFILE ON)
include("../../Common.cmake")

PREPROCESS_PATH_PERL("asubst.src" "asubst.pl")
RUN_POD2MAN("asubst.src" "asubst.1" "1" "EN Tools" "EN Tools")
