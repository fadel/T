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
	@rm -f Tc Td ${OBJ}

install: all
	@echo installing executable files in ${DESTDIR}${PREFIX}/bin
	@install -D -m755 Tc ${DESTDIR}${PREFIX}/bin/Tc
	@install -D -m755 Td ${DESTDIR}${PREFIX}/bin/Td

uninstall:
	@echo removing executable files from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/Tc
	@rm -f ${DESTDIR}${PREFIX}/bin/Td

.PHONY: all clean install uninstall
