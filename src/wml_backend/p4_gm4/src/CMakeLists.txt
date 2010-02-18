SET(CMAKE_VERBOSE_MAKEFILE ON)
ADD_DEFINITIONS(-DHAVE_CONFIG_H)

# TODO - replace ".." by the fully qualified path.
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/../lib ${CMAKE_CURRENT_SOURCE_DIR})

########### next target ###############

SET(m4_SRCS
   m4.c
   builtin.c
   debug.c
   eval.c
   format.c
   freeze.c
   input.c
   macro.c
   output.c
   path.c
   symtab.c
   )

FIND_LIBM()

ADD_EXECUTABLE(m4 ${m4_SRCS})

TARGET_LINK_LIBRARIES( m4 ${MATH_LIB_LIST} m4_lib )

SET_TARGET_PROPERTIES(m4
    PROPERTIES OUTPUT_NAME "wml_p4_gm4"
)

INSTALL(
    TARGETS m4
    DESTINATION "${WML_LIBEXE_DIR}"
)

