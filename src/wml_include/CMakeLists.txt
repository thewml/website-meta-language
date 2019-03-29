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
SET (batch_list )

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
    LIST (APPEND batch_list "${src}" "${dest}")
    LIST (APPEND preproc_target_dests "${dest}")
    INSTALL (
        FILES "${dest}"
        DESTINATION "${PKGDATADIR}/include/"
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
ENDFOREACH()
EXECUTE_PROCESS(COMMAND "${PERL_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/batch-preprocess-perl-source.pl" ${batch_list})

FILE(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/TheWML/Frontends/Wml/")
PREPROCESS_PATH_PERL_WITH_FULL_NAMES("preprocess_perl_include_config" "${CMAKE_CURRENT_SOURCE_DIR}/TheWML/Config.pm.src" "${CMAKE_CURRENT_BINARY_DIR}/TheWML/Config.pm")
INSTALL (
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/TheWML/Config.pm"
    DESTINATION "${PKGDATADIR}/TheWML"
)
FOREACH( fn
        "TheWML/Backends/ASubst/Main.pm"
        "TheWML/Backends/Divert/Main.pm"
        "TheWML/Backends/Fixup/Main.pm"
        "TheWML/Backends/HtmlStrip/Main.pm"
        "TheWML/Backends/IPP/Args.pm"
        "TheWML/Backends/IPP/Delimit.pm"
        "TheWML/Backends/IPP/File.pm"
        "TheWML/Backends/IPP/Line.pm"
        "TheWML/Backends/IPP/Main.pm"
        "TheWML/Backends/IPP/Map.pm"
        "TheWML/Backends/Slice/Main.pm"
        "TheWML/CmdLine/Base.pm"
        "TheWML/CmdLine/IO.pm"
        "TheWML/Frontends/Wml/OptD.pm"
        "TheWML/Frontends/Wml/PassObj.pm"
        "TheWML/Frontends/Wml/PassesManager.pm"
        "TheWML/Frontends/Wml/Protect.pm"
        "TheWML/Frontends/Wml/Runner.pm"
        "TheWML/Frontends/Wml/Util.pm"
        "TheWML/Frontends/Wml/WmlRc.pm"
        "TheWML/Frontends/Wml/WmlRcDir.pm"
        )
    get_filename_component(_dir "${fn}" DIRECTORY)
    INSTALL(
        FILES "${fn}"
        DESTINATION "${PKGDATADIR}/${_dir}"
    )
ENDFOREACH()

ADD_CUSTOM_TARGET(
    "preprocess_perl_includes" ALL
    DEPENDS ${preproc_target_dests}
)

ADD_CUSTOM_TARGET(
    "includes_pods" ALL
    DEPENDS ${include_pods_dests}
)
