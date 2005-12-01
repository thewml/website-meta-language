
INCLUDES = -I$(top_srcdir)/pcre -I$(top_srcdir)/lib @INTLINCL@

bin_PROGRAMS = mp4h

EXTRA_mp4h_SOURCES = module.c

EXTRA_DIST = readme.tmpl

if LOADABLE_MODULES
MODULE_C = module.c ltdl.c
endif

mp4h_SOURCES = builtin.c debug.c devel.c freeze.c input.c \
  macro.c $(MODULE_C) mp4h.c output.c path.c symtab.c
if LOADABLE_MODULES
mp4h_LDFLAGS = -export-dynamic
endif
mp4h_LDADD   = -lm $(top_builddir)/lib/libmp4h.a $(top_builddir)/pcre/libpcre.a @INTLLIBS@

include_HEADERS = mp4h.h
noinst_HEADERS  = builtin.h ltdl.h

