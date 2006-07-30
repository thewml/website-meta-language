MP4H_DIR = ../wml_backend/p2_mp4h
MP4H_min = $(MP4H_DIR)/Makefile.in
MP4H_cfg = $(MP4H_DIR)/configure

all: run_modules_file $(MP4H_min) $(MP4H_cfg) ../configure noautocache 

../configure: configure.in aclocal.m4
	rm -f ../configure
	autoconf configure.in >../configure
	./configure.fixit ../configure
	chmod a+x ../configure

$(MP4H_min): $(MP4H_DIR)/Makefile.am
	cd $(MP4H_DIR) && automake

$(MP4H_cfg): $(MP4H_DIR)/configure.ac
	cd $(MP4H_DIR) && autoconf

clean:

distclean:
	-rm -f ../config.status ../config.cache ../config.log

realclean: distclean
	-rm -f ../configure

noautocache:
	rm -fr autom4te.cache

run_modules_file:
	sh prep_modules_files.sh

