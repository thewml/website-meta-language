ADD_CUSTOM_TARGET(
    "test"
    "make" "-f" "test.mak" "test"
        "PATH_PERL=${PERL_EXECUTABLE}"
        "prefix=${CMAKE_INSTALL_PREFIX}"
        "srcdir=${CMAKE_CURRENT_SOURCE_DIR}"
        "top_srcdir=${CMAKE_SOURCE_DIR}"
    VERBATIM
)

ADD_DEPENDENCIES(
    "test" "all"
    )
