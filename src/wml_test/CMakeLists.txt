ENABLE_TESTING()

SET (_test_cmd_cmd_line "make" "-f" "test.mak" "test"
    "PATH_PERL=${PERL_EXECUTABLE}"
    "prefix=${CMAKE_INSTALL_PREFIX}"
    "srcdir=${CMAKE_CURRENT_SOURCE_DIR}"
    "top_srcdir=${CMAKE_SOURCE_DIR}"
    )

ADD_TEST(
    NAME wml_tests
    COMMAND ${_test_cmd_cmd_line})

ADD_CUSTOM_TARGET(
    "check"
    ${_test_cmd_cmd_line}
    VERBATIM
)

