# see - http://www.cmake.org/Wiki/CMake_FAQ#Is_there_an_option_to_produce_more_.27verbose.27_compiling.3F
SET(CMAKE_VERBOSE_MAKEFILE ON)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# TODO:
# add wml_test , wml_misc , wml_include , wml_frontend
SUBDIRS ( wml_aux wml_backend  
     )
