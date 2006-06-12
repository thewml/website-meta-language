
DEFS = @DEFS@ -DSUPPORT_UTF8

noinst_PROGRAMS  = dftables

dftables_SOURCES = dftables.c
dftables_DEPENDENCIES = maketables.c

noinst_LIBRARIES = libpcre.a

libpcre_a_SOURCES = maketables.c study.c pcre.c

noinst_HEADERS    = internal.h pcre.h

chartables.c: dftables$(EXEEXT)
	./dftables > $@

BUILT_SOURCES = chartables.c

CLEANFILES    = chartables.c

