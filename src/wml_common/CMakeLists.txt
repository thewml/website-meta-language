SET (CPAN_MODULES_TO_CHECK
    "Bit::Vector"
    "GD"
    "Getopt::Long"
    "HTML::Clean"
    "Image::Size"
    "IO"
    "Term::ReadKey"
    )

FOREACH (MODULE ${CPAN_MODULES_TO_CHECK})
    EXECUTE_PROCESS (
        COMMAND "${PERL_EXECUTABLE}" "-M${MODULE}" "-e" "exit(0)"
        RESULT_VARIABLE "RESULT"
    )
    IF (NOT RESULT EQUAL 0)
        MESSAGE(FATAL_ERROR "Your Perl doesn't have the module ${MODULE}. Please install it.")
    ENDIF (NOT RESULT EQUAL 0)
ENDFOREACH (MODULE ${CPAN_MODULES_TO_CHECK})
