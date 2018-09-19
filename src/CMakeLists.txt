CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

INCLUDE ("${CMAKE_SOURCE_DIR}/cmake/shlomif_common_bootstrap.cmake")
SHLOMIF_COMMON_SETUP("${private_mod_path}")
# see - http://www.cmake.org/Wiki/CMake_FAQ#Is_there_an_option_to_produce_more_.27verbose.27_compiling.3F
# SET(CMAKE_VERBOSE_MAKEFILE ON)

IF (NOT CMAKE_INSTALL_PREFIX)
    MESSAGE (FATAL_ERROR "CMAKE_INSTALL_PREFIX must be defined. Specify it with -DCMAKE_INSTALL_PREFIX=...")
ENDIF (NOT CMAKE_INSTALL_PREFIX)

INCLUDE(GNUInstallDirs)

SET (WML_LIB_DIR_BASE "${CMAKE_INSTALL_LIBDIR}")
set (p "${CMAKE_INSTALL_PREFIX}/")
string(FIND "${WML_LIB_DIR_BASE}" "${p}" _pos)
IF("${_pos}" EQUAL "0")
    string(LENGTH "${p}" _l)
    string(SUBSTRING "${WML_LIB_DIR_BASE}" "${_l}" -1 WML_LIB_DIR_BASE)
ENDIF()

SET (WML_LIB_DIR "${WML_LIB_DIR_BASE}/wml")
SET (WML_DATA_DIR "${WML_LIB_DIR}/data")
SET (WML_LIBEXE_DIR "${WML_LIB_DIR}/exec/")

ENABLE_TESTING()

ADD_SUBDIRECTORY( p3_eperl )
