SET (_test_cmd_cmd_line "${PERL_EXECUTABLE}"
    "${CMAKE_CURRENT_SOURCE_DIR}/run_test.pl"
)

ADD_TEST(
    NAME wml_tests
    COMMAND ${_test_cmd_cmd_line}
)

ADD_CUSTOM_TARGET(
    "check"
    ${_test_cmd_cmd_line}
    VERBATIM
)
