SET(CMAKE_VERBOSE_MAKEFILE ON)
ADD_DEFINITIONS(-DHAVE_CONFIG_H)

########### next target ###############

include("${CMAKE_CURRENT_SOURCE_DIR}/../../../Common.cmake")
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/..)
REPLACE_FUNCTIONS(LIB_SOURCES strtol obstack xmalloc xstrdup)
SET(mp4h_STAT_SRCS
   error.c
   ${LIB_SOURCES}
   )

ADD_LIBRARY(m4_lib STATIC ${mp4h_STAT_SRCS})


########### install files ###############




#original Makefile.am contents follow:

#
#INCLUDES = @INTLINCL@
#
#noinst_LIBRARIES  = libmp4h.a
#
#libmp4h_a_SOURCES = error.c
#libmp4h_a_LIBADD  = @LIBOBJS@
#noinst_HEADERS    = error.h getopt.h obstack.h
#
