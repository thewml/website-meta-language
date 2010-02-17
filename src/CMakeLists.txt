# see - http://www.cmake.org/Wiki/CMake_FAQ#Is_there_an_option_to_produce_more_.27verbose.27_compiling.3F
SET(CMAKE_VERBOSE_MAKEFILE ON)

SET(VERSION "2.1.1")

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE ("Common.cmake")

SET (WML_LIB_DIR "lib/wml")
SET (WML_DATA_DIR "${WML_LIB_DIR}/data")

SUBDIRS (
    wml_aux 
    wml_backend  
    wml_common
    wml_docs
    wml_frontend
    wml_test
    wml_include
    wml_misc
     )
