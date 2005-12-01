# Makefile - for tidy

CC= gcc

CFLAGS= -O

# Makefile - for tidy

CC= gcc

CFLAGS= -O

INSTALLDIR= /usr/local/

OFILES=		attrs.o         istack.o        parser.o        tags.o \
		entities.o      lexer.o         pprint.o        clean.o \
		localize.o      config.o        tidy.o

CFILES=		attrs.c         istack.c        parser.c        tags.c \
		entities.c      lexer.c         pprint.c        clean.c \
		localize.c      config.c        tidy.c

HFILES=		platform.h html.h

tidy:		$(OFILES)
		$(CC) $(CFLAGS) -o tidy  $(OFILES) -lc

$(OFILES):	$(HFILES)  Makefile

tab2space:	tab2space.c
		$(CC) $(CFLAGS) -o tab2space tab2space.c -lc

all:		tidy tab2space

clean:
		rm -f $(OFILES) tab2space.o  tidy tab2space

install:
	cp -f tidy $(INSTALLDIR)bin
	cp -f man_page.txt $(INSTALLDIR)man/man1/tidy.1
	cd $(INSTALLDIR)bin; \
	chmod 755 tidy; \
	chgrp bin tidy; \
	chown bin tidy;



