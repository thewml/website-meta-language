SET(CMAKE_VERBOSE_MAKEFILE ON)
########### next target ###############

DEFINE_WML_AUX_C_PROG_WITHOUT_MAN (
   "tidy"
   attrs.c
   clean.c
   config.c
   entities.c
   istack.c
   lexer.c
   localize.c
   parser.c
   pprint.c
   tags.c
   tidy.c
)

SET (html "Overview.html")
SET (man "wml_aux_tidy.1")
ADD_CUSTOM_COMMAND(
    OUTPUT "${man}"
    COMMAND "lynx"
    ARGS "-dump" "-nolist" "-width=72" "${html}" ">" "${man}"
    DEPENDS "${html}"
)

ADD_CUSTOM_TARGET(
    "tidy_manify" ALL
    DEPENDS "${man}"
)

INSTALL_CAT_MAN ("${man}" "1")
