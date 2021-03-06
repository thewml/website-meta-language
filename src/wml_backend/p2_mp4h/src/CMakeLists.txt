SET(CMAKE_VERBOSE_MAKEFILE ON)
ADD_DEFINITIONS(-DHAVE_CONFIG_H)

# TODO - replace ".." by the fully qualified path.
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/../lib ${CMAKE_CURRENT_SOURCE_DIR})

########### next target ###############

SET(mp4h_SRCS
   builtin.c
   debug.c
   devel.c
   freeze.c
   input.c
   ltdl.c
   macro.c
   module.c
   mp4h.c
   output.c
   path.c
   symtab.c
   )

ADD_EXECUTABLE(mp4h ${mp4h_SRCS})

FIND_LIBRARY(LIBPCRE_LIB pcre)
FIND_LIBRARY(LIBDL_LIB dl)
# Optional
IF (LIBDL_LIB)
    SET(LIBDL_LIB2 ${LIBDL_LIB} )
ENDIF()

FIND_LIBM()

TARGET_LINK_LIBRARIES(mp4h ${MATH_LIB_LIST} ${LIBPCRE_LIB} mp4h_lib ${LIBDL_LIB2})

SET_TARGET_PROPERTIES(mp4h
    PROPERTIES OUTPUT_NAME "wml_p2_mp4h"
)

INSTALL( TARGETS mp4h DESTINATION "${WML_LIBEXE_DIR}")
