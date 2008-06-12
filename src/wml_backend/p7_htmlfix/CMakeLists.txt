SET(CMAKE_VERBOSE_MAKEFILE ON)
include("../../Common.cmake")

PREPROCESS_PATH_PERL("htmlfix.src" "htmlfix.pl")
RUN_POD2MAN("htmlfix.src" "htmlfix.1" "1" "EN Tools" "EN Tools")
