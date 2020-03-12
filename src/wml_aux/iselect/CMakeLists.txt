SET(CMAKE_VERBOSE_MAKEFILE ON)

SET (CURSES_NEED_NCURSES TRUE)
INCLUDE(FindCurses)

set(help_cc "${CMAKE_CURRENT_BINARY_DIR}/iselect_help")
set(help_c "${help_cc}.c")
set(help_s "${CMAKE_CURRENT_SOURCE_DIR}/iselect_help.txt")
set(readme_cc "${CMAKE_CURRENT_BINARY_DIR}/iselect_readme")
set(readme_c "${readme_cc}.c")
set(readme_s "${CMAKE_CURRENT_SOURCE_DIR}/iselect_readme.txt")
# message(FATAL_ERROR ${readme_c})
DEFINE_WML_AUX_C_PROG (
   "iselect"
   "${CMAKE_CURRENT_BINARY_DIR}"
   "${help_c}"
   "${readme_c}"
   ${CMAKE_SOURCE_DIR}/lib/getopt.c
   ${CMAKE_SOURCE_DIR}/lib/getopt1.c
   iselect_main.c iselect_browse.c iselect_keys.c
   iselect_version.c
)

INCLUDE_DIRECTORIES( BEFORE "${CMAKE_CURRENT_BINARY_DIR}")
INCLUDE_DIRECTORIES( AFTER "${CMAKE_CURRENT_SOURCE_DIR}")

SET (pod_dests )
SIMPLE_POD2MAN(
    "pod_dests"
    "iselect.pod"
    "iselect"
    "1"
)

RUN_ASC2C("${help_s}" "${help_cc}" "iSelect_Help")
RUN_ASC2C("${readme_s}" "${readme_cc}" "iSelect_README")
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

CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/config_ac.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config_ac.h
    )

CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/config_sc.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config_sc.h
    )
