SET (myfiles
    all
    des/all
    des/gfont
    des/imgbg
    des/imgdot
    des/lowsrc
    des/navbar
    des/preload
    des/rollover
    des/space
    des/typography
    fmt/all
    fmt/isolatin
    fmt/pod
    fmt/sdf
    fmt/text
    fmt/url
    fmt/verbatim
    fmt/xtable
    imp/all
    imp/csmap
    imp/fsview
    imp/generic
    mod/all
    mod/MakeMaker
    mod/version
    std/all
    std/box
    std/case
    std/grid
    std/href
    std/info
    std/label
    std/lang
    std/logo
    std/page
    std/tags
    std/toc
    sup/all
    sup/hextriple
    sup/path
    sys/all
    sys/bootp3
    sys/bootp4
    sys/compat1
)

CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/sys/bootp3.src.in"
    "${CMAKE_CURRENT_BINARY_DIR}/sys/bootp3.src"
    COPYONLY
)

SET (dirs_to_make )
SET (preproc_target_dests )
SET (include_pods_dests )

FOREACH (base ${myfiles})

    # Make the directory to which the file belongs.
    STRING (REGEX REPLACE "/.*" "" basedir "${base}")
    LIST (FIND dirs_to_make "${basedir}" found)
    IF ("${found}" STREQUAL "-1")
        LIST (APPEND dirs_to_make "${basedir}")
        FILE (MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${basedir}")
    ENDIF ("${found}" STREQUAL "-1")

    SET (destbase "${base}.wml")
    SET (dest "${CMAKE_CURRENT_BINARY_DIR}/${destbase}")
    SET (srcbase "${base}.src")
    IF ("${base}" STREQUAL "sys/bootp3")
        SET (src "${CMAKE_CURRENT_BINARY_DIR}/${srcbase}")
        SET (is_binary 1)
        # MESSAGE (FATAL_ERROR "src = ${src}")
    ELSE ("${base}" STREQUAL "sys/bootp3")
        SET (src "${CMAKE_CURRENT_SOURCE_DIR}/${srcbase}")
        SET (is_binary )
    ENDIF ("${base}" STREQUAL "sys/bootp3")
    SET (mannamebase "${base}")
    SET (manname "${CMAKE_CURRENT_BINARY_DIR}/${mannamebase}")

    STRING (REPLACE "/" "__" target_base "${base}")


    ADD_CUSTOM_COMMAND(
        OUTPUT "${dest}"
        COMMAND "${PERL_EXECUTABLE}"
        ARGS "${CMAKE_CURRENT_SOURCE_DIR}/preprocess-perl-source.pl"
            "--input" "${src}"
            "--output" "${dest}"
        DEPENDS "${src}"
    )
    LIST (APPEND preproc_target_dests "${dest}")
    INSTALL (
        FILES "${dest}"
        DESTINATION "${WML_LIB_DIR}/include/"
        RENAME "${destbase}"
    )
    STRING (REPLACE "/" "::" dest_man "wml/${base}")

    IF (is_binary)
        SET (POD2MAN_SOURCE_IS_IN_BINARY 1)
    ENDIF (is_binary)
    INST_RENAME_POD2MAN(
        "include_pods_dests"
        "${srcbase}"
        "${mannamebase}"
        "3"
        "${dest_man}"
    )
ENDFOREACH(base)

FILE(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/WML_Frontends/Wml/")
PREPROCESS_PATH_PERL_WITH_FULL_NAMES("preprocess_perl_include_runner" "${CMAKE_CURRENT_SOURCE_DIR}/WML_Frontends/Wml/Runner.pm.src" "${CMAKE_CURRENT_BINARY_DIR}/WML_Frontends/Wml/Runner.pm")
INSTALL (
    FILES "WML_Backends.pm"
    DESTINATION "${WML_LIB_DIR}"
)

INSTALL (
    FILES "WML_Backends/Divert.pm"
    DESTINATION "${WML_LIB_DIR}/WML_Backends"
)

INSTALL (
    FILES
        "WML_Backends/IPP/Args.pm"
        "WML_Backends/IPP/Delimit.pm"
        "WML_Backends/IPP/Map.pm"
    DESTINATION "${WML_LIB_DIR}/WML_Backends/IPP"
)

INSTALL (
    FILES
        "WML_Frontends/Wml/Base.pm"
        "WML_Frontends/Wml/PassesManager.pm"
        "WML_Frontends/Wml/PassObj.pm"
        "${CMAKE_CURRENT_BINARY_DIR}/WML_Frontends/Wml/Runner.pm"
        "WML_Frontends/Wml/Util.pm"
    DESTINATION "${WML_LIB_DIR}/WML_Frontends/Wml"
)

ADD_CUSTOM_TARGET(
    "preprocess_perl_includes" ALL
    DEPENDS ${preproc_target_dests}
)

ADD_CUSTOM_TARGET(
    "includes_pods" ALL
    DEPENDS ${include_pods_dests}
)
