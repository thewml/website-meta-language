SET(CMAKE_VERBOSE_MAKEFILE ON)
include("../../Common.cmake")

PREPROCESS_PATH_PERL("ipp.src" "ipp.pl")
RUN_POD2MAN("ipp.src" "ipp.1" "1" "EN Tools" "EN Tools")
