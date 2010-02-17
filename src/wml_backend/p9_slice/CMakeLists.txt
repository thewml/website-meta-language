SET(CMAKE_VERBOSE_MAKEFILE ON)
include("../../Common.cmake")

PREPROCESS_PATH_PERL("slice_preproc" "slice_main.pl" "slice")
INST_POD2MAN("slice_pod" "slice.pod" "slice" "1")
