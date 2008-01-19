include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckFunctionExists)

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

