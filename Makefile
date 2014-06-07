PREFIX = /usr

INCS = `pkg-config --cflags gtk+-2.0 vte`
LIBS = `pkg-config --libs gtk+-2.0 vte`
CFLAGS = -ansi -pedantic -Wall -O2 ${INCS}
LDFLAGS = -s ${LIBS}
VER = 0.7
CC = cc

SRC = src/T.c
OBJ = ${SRC:.c=.o}

all: T

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} -o $@ $<

${OBJ}: src/config.h

T: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f T ${OBJ} T-${VER}.tar.bz2

dist: PKGBUILD
	@echo creating distributable tarball
	@mkdir T-${VER}
	@cp -R src/ T.desktop Makefile T-${VER}
	@tar cf T-${VER}.tar.bz2 T-${VER}
	@rm -rf T-${VER}

install: all
	@echo installing executable file in ${DESTDIR}${PREFIX}/bin
	@install -D -m755 T ${DESTDIR}${PREFIX}/bin/T
	@echo installing desktop file in ${DESTDIR}${PREFIX}/share/applications
	@install -D -m644 T.desktop ${DESTDIR}${PREFIX}/share/applications/T.desktop
