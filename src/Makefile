PREFIX = /usr

INCS = `pkg-config --cflags gtk+-2.0 vte`
LIBS = `pkg-config --libs gtk+-2.0 vte`
CFLAGS = -ansi -pedantic -Wall -O3 ${INCS}
LDFLAGS = -s ${LIBS}
CC = cc

SRC = T.c
OBJ = ${SRC:.c=.o}

all: T

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

${OBJ}: config.h

T: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	@echo cleaning
	@rm -rf T ${OBJ}

install: all
	@echo installing executable file to ${DESTDIR}${PREFIX}/bin
	@install -D -m755 T ${DESTDIR}${PREFIX}/bin/T
	@echo installing icon to ${DESTDIR}${PREFIX}/share/applications
	@install -D -m644 T.desktop ${DESTDIR}${PREFIX}/share/applications/T.desktop
