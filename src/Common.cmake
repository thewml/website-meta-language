include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckFunctionExists)
include(FindPerl)
IF (NOT PERL_FOUND)
    MESSAGE ( FATAL_ERROR "perl must be installed")
ENDIF(NOT PERL_FOUND)

# Taken from http://www.cmake.org/pipermail/cmake/2007-March/013060.html
MACRO(REPLACE_FUNCTIONS sources)
  FOREACH(name ${ARGN})
    STRING(TOUPPER have_${name} SYMBOL_NAME)
    CHECK_FUNCTION_EXISTS(${name} ${SYMBOL_NAME})
    IF(NOT ${SYMBOL_NAME})
      SET(${sources} ${${sources}} ${name}.c)
    ENDIF(NOT ${SYMBOL_NAME})
  ENDFOREACH(name)
ENDMACRO(REPLACE_FUNCTIONS)

MACRO(CHECK_MULTI_INCLUDE_FILES)
  FOREACH(name ${ARGN})
    STRING(TOUPPER have_${name} SYMBOL_NAME)
    STRING(REGEX REPLACE "\\." "_" SYMBOL_NAME ${SYMBOL_NAME})
    STRING(REGEX REPLACE "/" "_" SYMBOL_NAME ${SYMBOL_NAME})
    CHECK_INCLUDE_FILE(${name} ${SYMBOL_NAME})
  ENDFOREACH(name)
ENDMACRO(CHECK_MULTI_INCLUDE_FILES)

MACRO(CHECK_MULTI_FUNCTIONS_EXISTS)
  FOREACH(name ${ARGN})
    STRING(TOUPPER have_${name} SYMBOL_NAME)
    CHECK_FUNCTION_EXISTS(${name} ${SYMBOL_NAME})
  ENDFOREACH(name)
ENDMACRO(CHECK_MULTI_FUNCTIONS_EXISTS)

MACRO(PREPROCESS_PATH_PERL TARGET_NAME SOURCE DEST)
    SET(PATH_PERL ${PERL_EXECUTABLE})
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
        ${TARGET_NAME} ALL
        DEPENDS ${DEST}
    )
ENDMACRO(PREPROCESS_PATH_PERL)

MACRO(RUN_POD2MAN TARGET_NAME SOURCE DEST SECTION CENTER RELEASE)
    SET(PATH_PERL ${PERL_EXECUTABLE})
    ADD_CUSTOM_COMMAND(
        OUTPUT ${DEST}
        COMMAND ${PATH_PERL} 
        ARGS "-e" 
        "my (\$src, \$dest, \$sect, \$center, \$release) = @ARGV; my \$pod = qq{Hoola.pod}; use File::Copy; copy(\$src, \$pod); system(qq{pod2man --section=\$sect --center=\"\$center\" --release=\"\$release\" \$pod > \$dest}); unlink(\$pod)"
        ${SOURCE}
        ${DEST}
        ${SECTION}
        "${CENTER}"
        "${RELEASE}"
        DEPENDS ${SOURCE}
        VERBATIM
    )
    # The custom command needs to be assigned to a target.
    ADD_CUSTOM_TARGET(
        ${TARGET_NAME} ALL
        DEPENDS ${DEST}
    )
ENDMACRO(RUN_POD2MAN)


# Finds libm and puts the result in the MATH_LIB_LIST variable.
# If it cannot find it, it fails with an error.
MACRO(FIND_LIBM)
    IF (UNIX)
        FIND_LIBRARY(LIBM_LIB m)
        IF(LIBM_LIB STREQUAL "LIBM_LIB-NOTFOUND")
            MESSAGE(FATAL_ERROR "Cannot find libm")
        ELSE(LIBM_LIB STREQUAL "LIBM_LIB-NOTFOUND")
            SET(MATH_LIB_LIST ${LIBM_LIB})
        ENDIF(LIBM_LIB STREQUAL "LIBM_LIB-NOTFOUND")
    ELSE(UNIX)
        SET(MATH_LIB_LIST)
    ENDIF(UNIX)
ENDMACRO(FIND_LIBM)

MACRO(INSTALL_MAN SOURCE SECTION)
    INSTALL(
        FILES
            ${SOURCE}
        DESTINATION
            "share/man/man${SECTION}"
   )
ENDMACRO(INSTALL_MAN)

MACRO(DEFINE_WML_AUX_PERL_PROG BASENAME)
    PREPROCESS_PATH_PERL("preproc_${BASENAME}" "${BASENAME}.src" "${BASENAME}.pl")
    RUN_POD2MAN("pod_${BASENAME}" "${BASENAME}.src" "${BASENAME}.1" "1" "EN  Tools" "En Tools")
    INSTALL(
        FILES "${BASENAME}.pl"
        DESTINATION "lib/exec/wml_aux_${BASENAME}"
    )
    INSTALL_MAN ("${BASENAME}.1" 1)
ENDMACRO(DEFINE_WML_AUX_PERL_PROG BASENAME)

MACRO(DEFINE_WML_AUX_C_PROG BASENAME)
    ADD_EXECUTABLE(${BASENAME} ${ARGN})
    INSTALL(
        TARGETS ${BASENAME}
        DESTINATION "lib/exec/wml_aux_${BASENAME}"
    )
    INSTALL_MAN ("${BASENAME}.1" 1)
ENDMACRO(DEFINE_WML_AUX_C_PROG BASENAME)

