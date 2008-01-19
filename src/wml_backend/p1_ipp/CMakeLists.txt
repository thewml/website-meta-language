SET(CMAKE_VERBOSE_MAKEFILE ON)
include(FindPerl)
IF (NOT PERL_FOUND)
    MESSAGE ( FATAL_ERROR "perl must be installed")
ENDIF(NOT PERL_FOUND)

SET(PATH_PERL ${PERL_EXECUTABLE})
SET(SOURCE "ipp.src")
SET(DEST "ipp.pl")
ADD_CUSTOM_COMMAND(
    OUTPUT ${DEST}
    COMMAND ${PATH_PERL} 
    ARGS "-e" 
    "open I, qq{<\$ARGV[0]}; open O, qq{>\$ARGV[1]}; while(<I>){s{\\@PATH_PERL\\@}{\$ARGV[2]}g;print O \$_;} close(I); close(O);"
    ${SOURCE}
    ${DEST}
    ${PATH_PERL}
    COMMAND chmod ARGS "a+x" ${DEST}
    DEPENDS ${SOURCE}
    VERBATIM
    )
# The custom command needs to be assigned to a target.
ADD_CUSTOM_TARGET(
    process_perl ALL
    DEPENDS ${DEST}
    )
