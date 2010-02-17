ADD_CUSTOM_TARGET(
    "test"
    COMMAND "make" "-f" "test.mak"
        "PATH_PERL=${PERL_EXECUTABLE}"
        "prefix=${CMAKE_INSTALL_PREFIX}"
        "srcdir=${CMAKE_CURRENT_SOURCE_DIR}"
        "top_srcdir=${CMAKE_SOURCE_DIR}"
)

ADD_DEPENDENCIES(
    "test" "all"
    )
