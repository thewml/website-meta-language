FILE(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/TheWML/Frontends/Wml/")
PREPROCESS_PATH_PERL_WITH_FULL_NAMES("preprocess_perl_include_config" "${CMAKE_CURRENT_SOURCE_DIR}/TheWML/Config.pm.src" "${CMAKE_CURRENT_BINARY_DIR}/TheWML/Config.pm")
INSTALL (
    FILES
        "TheWML/Backends.pm"
        "${CMAKE_CURRENT_BINARY_DIR}/TheWML/Config.pm"
    DESTINATION "${WML_LIB_DIR}/TheWML"
)

INSTALL (
    FILES "TheWML/Backends/Divert.pm"
    DESTINATION "${WML_LIB_DIR}/TheWML/Backends"
)

INSTALL (
    FILES
        "TheWML/Backends/IPP/Args.pm"
        "TheWML/Backends/IPP/Delimit.pm"
        "TheWML/Backends/IPP/File.pm"
        "TheWML/Backends/IPP/Line.pm"
        "TheWML/Backends/IPP/Main.pm"
        "TheWML/Backends/IPP/Map.pm"
    DESTINATION "${WML_LIB_DIR}/TheWML/Backends/IPP"
)

INSTALL (
    FILES
        "TheWML/Frontends/Wml/Base.pm"
        "TheWML/Frontends/Wml/OptD.pm"
        "TheWML/Frontends/Wml/PassesManager.pm"
        "TheWML/Frontends/Wml/PassObj.pm"
        "TheWML/Frontends/Wml/Protect.pm"
        "TheWML/Frontends/Wml/Runner.pm"
        "TheWML/Frontends/Wml/Util.pm"
        "TheWML/Frontends/Wml/WmlRc.pm"
        "TheWML/Frontends/Wml/WmlRcDir.pm"
    DESTINATION "${WML_LIB_DIR}/TheWML/Frontends/Wml"
)

ADD_CUSTOM_TARGET(
    "preprocess_perl_includes" ALL
    DEPENDS ${preproc_target_dests}
)

ADD_CUSTOM_TARGET(
    "includes_pods" ALL
    DEPENDS ${include_pods_dests}
)
