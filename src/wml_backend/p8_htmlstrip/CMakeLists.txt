SET(CMAKE_VERBOSE_MAKEFILE ON)
include("../../Common.cmake")

PREPROCESS_PATH_PERL("htmlstrip_preproc" "htmlstrip.src" "htmlstrip.pl")
RUN_POD2MAN("htmlstrip_pod" "htmlstrip.src" "htmlstrip.1" "1" "EN Tools" "EN Tools")
