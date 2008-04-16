
AUTOMAKE_OPTIONS = foreign

DIST_SUBDIRS = intl pcre lib src po doc modules tests

if LOADABLE_MODULES
SUBDIRS = intl pcre lib src po doc modules tests
else
SUBDIRS = intl pcre lib src po doc tests
endif

EXTRA_DIST = CREDITS README README.pcre bootstrap.sh shtool

deb:
	if test -f ../mp4h_$(VERSION).orig.tar.gz; then \
          :; \
        else \
          $(MAKE) dist && mv mp4h-$(VERSION).tar.gz ../mp4h_$(VERSION).orig.tar.gz; \
        fi
	-rm -rf config.log autom4te.cache
	$(MAKE) distclean && dpkg-buildpackage -i'/CVS|/\.cvsignore|/\.scvsrc|/tags' -rfakeroot

README: configure.ac src/readme.tmpl
	@x='@'; sed -e "s/$${x}PACKAGE_VERSION$${x}/@PACKAGE_VERSION@/" -e "s/$${x}PACKAGE_DATE$${x}/@PACKAGE_DATE@/" src/readme.tmpl > $@
