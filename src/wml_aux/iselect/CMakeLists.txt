SET(CMAKE_VERBOSE_MAKEFILE ON)
INCLUDE ("../../Common.cmake")

SET (CURSES_NEED_NCURSES TRUE)
INCLUDE(FindCurses)

DEFINE_WML_AUX_C_PROG (
   "iselect"
   iselect_main.c iselect_browse.c iselect_keys.c
   iselect_getopt.c iselect_version.c iselect_help.c iselect_readme.c
)

SIMPLE_POD2MAN(
    "wml_aux_iselect"
    "${CMAKE_CURRENT_SOURCE_DIR}/iselect.pod"
    "${CMAKE_CURRENT_BINARY_DIR}/iselect"
    "1"
)

TARGET_LINK_LIBRARIES(
    iselect
    ${CURSES_LIBRARIES}
)

SET (USE_NCURSES "1")
SET (USE_CURSES "")
SET (USE_SLCURSES "")

CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/config_ac.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config_ac.h
    )

CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/config_sc.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config_sc.h
    )
