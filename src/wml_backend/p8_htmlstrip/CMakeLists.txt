SET(CMAKE_VERBOSE_MAKEFILE ON)
include("../../Common.cmake")

PREPROCESS_PATH_PERL("htmlstrip.src" "htmlstrip.pl")
RUN_POD2MAN("htmlstrip.src" "htmlstrip.1" "1" "EN Tools" "EN Tools")
