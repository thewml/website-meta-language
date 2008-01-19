
PROGDIR         = $(top_builddir)/src
PROGRAM         = $(PROGDIR)/mp4h
FLAGS           = -X 0 -I $(srcdir) -D srcdir="$(srcdir)"
DEPS            = list-tags
TIDY            = @TIDY@

man1_MANS       = mp4h.1

SUFFIXES = .info .ps .dvi .texi .1 .pod
.texi.info:
	makeinfo $<
.texi.dvi:
	texi2dvi $<
.dvi.ps:
	dvips -o $@ $<
.pod.1:
	pod2man --section=1 \
            --center="HTML Tools" \
            --release="HTML Tools" \
        $< > $@

all: $(PROGRAM) mp4h.1 mp4h.html

mp4h.html: mp4h.mp4h html.out $(PROGRAM) $(DEPS)
	$(PROGRAM) $(FLAGS) -D format=html $(srcdir)/mp4h.mp4h > $@
	-$(TIDY) $(TIDYOPT) -m $@

mp4h.pod: mp4h.mp4h pod.out $(PROGRAM) $(DEPS)
	$(PROGRAM) $(FLAGS) -D format=pod $(srcdir)/mp4h.mp4h \
        | sed -e 's/_LT_/</g' -e 's/_GT_/>/g' > $@

mp4h.texi: mp4h.mp4h texinfo.out $(PROGRAM) $(DEPS)
	$(PROGRAM) $(FLAGS) -D format=texinfo -D info:gentoc=1 $(srcdir)/mp4h.mp4h |\
   grep '@remove@' | sed -e 's/@remove@//' -e 's/=LT=/</g' > mp4h.chp
	$(PROGRAM) $(FLAGS) -D format=texinfo $(srcdir)/mp4h.mp4h > $@

$(PROGRAM):
	cd $(PROGDIR) && $(MAKE)

EXTRA_DIST = mp4h.mp4h list-tags html.out pod.out texinfo.out

CLEANFILES = \
   mp4h.pod mp4h.1 \
   mp4h.html \
   mp4h.info mp4h.info-* mp4h.texi mp4h.chp mp4h.cp mp4h.fn mp4h.vr \
   mp4h.tp mp4h.ky mp4h.pg mp4h.aux mp4h.log mp4h.toc mp4h.dvi mp4h.ps

# Workaround so html.out won't be deleted on cygwin.
html.out:
	@echo hello
