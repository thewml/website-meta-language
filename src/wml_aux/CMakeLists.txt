# see - http://www.cmake.org/Wiki/CMake_FAQ#Is_there_an_option_to_produce_more_.27verbose.27_compiling.3F
SET(CMAKE_VERBOSE_MAKEFILE ON)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

ADD_SUBDIRECTORY( iselect )
ADD_SUBDIRECTORY( htmlclean )
ADD_SUBDIRECTORY( htmlinfo )
ADD_SUBDIRECTORY( linklint )
ADD_SUBDIRECTORY( map2html )
ADD_SUBDIRECTORY( tidy )
ADD_SUBDIRECTORY( txt2html )
ADD_SUBDIRECTORY( weblint )
ADD_SUBDIRECTORY( freetable )
