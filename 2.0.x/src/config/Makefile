
all: ../configure

../configure: configure.in aclocal.m4
	rm -f ../configure
	autoconf configure.in >../configure
	./configure.fixit ../configure
	chmod a+x ../configure

clean:

distclean:
	-rm -f ../config.status ../config.cache ../config.log

realclean: distclean
	-rm -f ../configure

