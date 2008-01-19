# see - http://www.cmake.org/Wiki/CMake_FAQ#Is_there_an_option_to_produce_more_.27verbose.27_compiling.3F
SET(CMAKE_VERBOSE_MAKEFILE ON)
ADD_DEFINITIONS(-DHAVE_CONFIG_H)
INCLUDE_DIRECTORIES( ".." )
ADD_EXECUTABLE (mp4h 
    builtin.c debug.c devel.c freeze.c input.c
    macro.c mp4h.c output.c path.c symtab.c
    )
