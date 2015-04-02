DESTDIR = /
PREFIX = usr

INCS = `pkg-config --cflags gtk+-2.0 gdk-2.0 vte`
LIBS = `pkg-config --libs gtk+-2.0 gdk-2.0 vte`
CFLAGS = -std=c99 -pedantic -Wall -O2 ${INCS}
LDFLAGS = -s ${LIBS}
VER = 0.9
CC = cc

SRC = T.c Tc.c Td.c
OBJ = ${SRC:.c=.o}

all: Tc Td

%.o: %.c
	@echo CC $<
	@${CC} -c ${CFLAGS} -o $@ $<

${OBJ}: config.h T.h

Tc: T.o Tc.o
	@echo CC -o $@
	@${CC} -o $@ $^ ${LDFLAGS}

Td: T.o Td.o
	@echo CC -o $@
	@${CC} -o $@ $^ ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f Tc Td ${OBJ} T-${VER}.tar.bz2

dist: clean
	#TODO: remove .desktop file?
	@echo creating distributable tarball
	@mkdir T-${VER}
	@cp -R src/ T.desktop Makefile T-${VER}
	@tar cf T-${VER}.tar.bz2 T-${VER}
	@rm -rf T-${VER}

install: all
	@echo installing executable files in ${DESTDIR}${PREFIX}/bin
	@install -D -m755 Tc ${DESTDIR}${PREFIX}/bin/Tc
	@install -D -m755 Td ${DESTDIR}${PREFIX}/bin/Td
	#@echo installing desktop file in ${DESTDIR}${PREFIX}/share/applications
	#@install -D -m644 T.desktop ${DESTDIR}${PREFIX}/share/applications/T.desktop

uninstall:
	@echo removing executables files from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/Tc
	@rm -f ${DESTDIR}${PREFIX}/bin/Td

.PHONY: all clean dist install uninstall
