SET (myfiles
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
    sys/bootp4
    sys/compat1
)

FOREACH (base ${myfiles})
    SET (destbase "${base}.wml")
    SET (dest "${CMAKE_CURRENT_BINARY_DIR}/${destbase}")
    SET (src "${CMAKE_CURRENT_SOURCE_DIR}/${base}.src")
    SET (manname "${CMAKE_CURRENT_BINARY_DIR}/${base}")

    STRING (REPLACE "/" "__" target_base "${base}")

    ADD_CUSTOM_COMMAND(
        OUTPUT "${dest}"
        COMMAND "${PERL_EXECUTABLE}"
        ARGS "${CMAKE_CURRENT_SOURCE_DIR}/preprocess-perl-source.pl"
            "--input" "${src}"
            "--output" "${dest}"
        DEPENDS "${src}"
    )
    ADD_CUSTOM_TARGET(
        "preprocess_perl_include_${target_base}" ALL
        DEPENDS "${dest}"
    )
    INSTALL (FILES "${dest}" DESTINATION "lib/include/${destbase}")
    STRING (REPLACE "/" "::" dest_man "wml/${base}")

    INST_RENAME_POD2MAN(
        "include_${target_base}_pod"
        "${src}"
        "${manname}"
        "3"
        "${dest_man}"
    )
ENDFOREACH(base)

