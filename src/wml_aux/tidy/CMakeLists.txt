SET(CMAKE_VERBOSE_MAKEFILE ON)
INCLUDE ("../../Common.cmake")
########### next target ###############

DEFINE_WML_AUX_C_PROG_WITHOUT_MAN (
   "tidy"
   attrs.c
   clean.c
   config.c
   entities.c
   istack.c
   lexer.c
   localize.c
   parser.c
   pprint.c
   tags.c
   tidy.c
)

