SET(CMAKE_VERBOSE_MAKEFILE ON)

SET (CURSES_NEED_NCURSES TRUE)
INCLUDE(FindCurses)

DEFINE_WML_AUX_C_PROG (
   "iselect"
   "${CMAKE_CURRENT_BINARY_DIR}"
   iselect_main.c iselect_browse.c iselect_keys.c
   iselect_getopt.c iselect_version.c iselect_help.c iselect_readme.c
)

INCLUDE_DIRECTORIES( BEFORE "${CMAKE_CURRENT_BINARY_DIR}")

SET (POD2MAN_TARGET_IS_A_LIST 1)
SET (pod_dests )
SIMPLE_POD2MAN(
    "pod_dests"
    "iselect.pod"
    "iselect"
    "1"
)

SET (POD2MAN_TARGET_IS_A_LIST )

ADD_CUSTOM_TARGET(
    "wml_aux_iselect" ALL
    DEPENDS ${pod_dests}
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
