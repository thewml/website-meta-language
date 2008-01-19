
thisdir  = system

#   Architecture-independent modules
mp4mshareddir = $(pkgdatadir)/$(thisdir)

#   Architecture-dependent modules
mp4mdir = $(pkglibdir)/@VERSION@/$(thisdir)

SRCFILES  = sh.src exec.src
TESTFILES = sh.mp4h

man3ext   = 3

INCLUDES  = -I$(top_srcdir)/lib  -I$(top_srcdir)/src @INTLINCL@
LDFLAGS   = -no-undefined -module -avoid-version -rpath $(mp4mdir) @DLLDFLAGS@

if DO_MODULE_SYSTEM
mp4m_LTLIBRARIES = sh.la exec.la

mp4mshared_DATA = $(SRCFILES:.src=.mp4hp)
man3_MANS       = $(thisdir)sh.$(man3ext) $(thisdir)exec.$(man3ext)
endif

sh_la_SOURCES =  sh.c
sh_la_LDFLAGS = -module

exec_la_SOURCES =  exec.c
exec_la_LDFLAGS = -module

EXTRA_DIST = $(SRCFILES) $(TESTFILES)

CLEANFILES = $(mp4mshared_DATA) $(man3_MANS)

SUFFIXES = .src .mp4hp

.src.mp4hp:
	sed -e '/^=head1/,/^=cut/d' -e '/^##EOF##/,$$d' $< > $@
	cat $< > $(thisdir):$*.pod && pod2man --section=$(man3ext) \
   --center="mp4h modules" \
   --release="mp4h modules" \
   $(thisdir):$*.pod > $(thisdir)$*.$(man3ext) && rm -f $(thisdir):$*.pod

$(man3_MANS): $(mp4mshared_DATA)

install-data-hook:
	list='$(man3_MANS)'; for f in $${list}; do \
      mv $(DESTDIR)$(man3dir)/$$f $(DESTDIR)$(man3dir)/`echo $$f | sed -e 's/^$(thisdir)/$(thisdir):/'`; \
    done

