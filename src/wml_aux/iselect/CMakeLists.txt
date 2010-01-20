SET(CMAKE_VERBOSE_MAKEFILE ON)
INCLUDE ("../../Common.cmake")
########### next target ###############

DEFINE_WML_AUX_C_PROG (
   "iselect"
   iselect_main.c iselect_browse.c iselect_keys.c
   iselect_getopt.c iselect_version.c iselect_help.c iselect_readme.c
)

