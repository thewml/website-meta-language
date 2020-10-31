CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT(wml)

INCLUDE ("${CMAKE_SOURCE_DIR}/cmake/shlomif_common_bootstrap.cmake")
SHLOMIF_COMMON_SETUP("${private_mod_path}")
INCLUDE ("${CMAKE_SOURCE_DIR}/cmake/thewml.cmake")
# see - http://www.cmake.org/Wiki/CMake_FAQ#Is_there_an_option_to_produce_more_.27verbose.27_compiling.3F
# SET(CMAKE_VERBOSE_MAKEFILE ON)

SET (CPU_ARCH "" CACHE STRING "A specific CPU architecture to pass to gcc")
option (OPTIMIZATION_OMIT_FRAME_POINTER "Use -fomit-frame-pointer with gcc (not recommended for debugging)")
SET (PKGDATADIR_SUBDIR "wml")
SET (RELATIVE_PKGDATADIR "${RELATIVE_DATADIR}/${PKGDATADIR_SUBDIR}")
SET (PKGDATADIR "${DATADIR}/${PKGDATADIR_SUBDIR}")

SET(VERSION "2.32.0")

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
SET (WML_DATA_DIR "${PKGDATADIR}/data")
SET (WML_LIBEXE_DIR "${WML_LIB_DIR}/exec/")

# This is the equivalent to perform a "make dist"/"make distdir" etc.
SET(CPACK_PACKAGE_NAME "wml")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Website Meta Language")
SET(CPACK_PACKAGE_VENDOR "Shlomi Fish")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")

# Process and extract the version number.
# FILE( READ "ver.txt" VERSION)

CHOMP (VERSION)

STRING (REGEX MATCHALL "([0-9]+)" VERSION_DIGITS "${VERSION}")

LIST(GET VERSION_DIGITS 0 CPACK_PACKAGE_VERSION_MAJOR)
LIST(GET VERSION_DIGITS 1 CPACK_PACKAGE_VERSION_MINOR)
LIST(GET VERSION_DIGITS 2 CPACK_PACKAGE_VERSION_PATCH)

SET(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_DESCRIPTION_SUMMARY} ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

SET (base_with_ver "${CPACK_PACKAGE_NAME}-[0-9]+\\\\.[0-9]+\\\\.[0-9]+")

SET(CPACK_SOURCE_GENERATOR "TXZ")
SET(CPACK_SOURCE_IGNORE_FILES
    "~$"
    "/\\\\.tidyall\\\\.d/"
    "/t/t/.*\\\\.exe$"
    "/tags$"
)

IF(WIN32 AND NOT UNIX)
    # There is a bug in NSI that does not handle full unix paths properly. Make
    # sure there is at least one set of four (4) backlasshes.
    # SET(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}\\\\dondorf-king.bmp")
    SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\wml.exe")
    SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\thewml.github.io")
    SET(CPACK_NSIS_URL_INFO_ABOUT "${CPACK_NSIS_HELP_LINK}")
    SET(CPACK_NSIS_DISPLAY_NAME "Website Meta Language")
    SET(CPACK_NSIS_CONTACT "shlomif@cpan.org")
    SET(CPACK_NSIS_MODIFY_PATH ON)
    # Setting for NSIS :
    SET(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\wml.ico")
    SET(CPACK_NSIS_MUI_UNIICON ${CPACK_NSIS_MUI_ICON})
    SET(CPACK_PACKAGE_ICON ${CPACK_NSIS_MUI_ICON})
    SET(CPACK_NSIS_MODIFY_PATH "ON")
ELSE()
    # SET(CPACK_STRIP_FILES "")
    # SET(CPACK_SOURCE_STRIP_FILES "")
ENDIF()

SET(CPACK_PACKAGE_EXECUTABLES
    "wml" "Website Meta Language Frontend"
)

FIND_PROGRAM(_lynx NAMES "lynx")
IF (NOT _lynx)
    MESSAGE (FATAL_ERROR "lynx is required - please install it.")
ENDIF ()

INCLUDE (CPack)

SET (CPAN_MODULES_TO_CHECK
    "Bit::Vector"
    "Carp::Always"
    "Class::XSAccessor"
    "File::Which"
    "GD"
    "Getopt::Long"
    "Image::Size"
    "List::MoreUtils"
    "Path::Tiny"
    "Term::ReadKey"
    "Time::HiRes"
    )

CHECK_FOR_MULTIPLE_PERL_MODULES(${CPAN_MODULES_TO_CHECK})

ENABLE_TESTING()

FUNCTION(RUN_ASC2C src dest varname)
    ADD_CUSTOM_COMMAND(
        COMMAND ${PERL_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/asc2c-new.pl "${src}" "${dest}" "${varname}"
        OUTPUT "${dest}.c"
        DEPENDS "${src}"
    )
ENDFUNCTION()

ADD_SUBDIRECTORY( wml_aux )
ADD_SUBDIRECTORY( wml_backend )
ADD_SUBDIRECTORY( wml_docs )
ADD_SUBDIRECTORY( wml_frontend )
ADD_SUBDIRECTORY( wml_test )
ADD_SUBDIRECTORY( wml_include )
ADD_SUBDIRECTORY( wml_misc )
