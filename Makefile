DESTDIR = /
PREFIX = usr

INCS = `pkg-config --cflags gtk+-2.0 gdk-2.0 vte`
LIBS = `pkg-config --libs gtk+-2.0 gdk-2.0 vte`
CFLAGS = -std=c99 -pedantic -Wall -O2 ${INCS}
LDFLAGS = -s ${LIBS}
VER = 0.9
CC = cc

SRC = T.c
OBJ = ${SRC:.c=.o}

all: T

%.o: %.c
	@echo CC $<
	@${CC} -c ${CFLAGS} -o $@ $<

${OBJ}: config.h

T: T.o
	@echo CC -o $@
	@${CC} -o $@ $^ ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f T ${OBJ}

install: all
	@echo installing executable files in ${DESTDIR}${PREFIX}/bin
	@install -D -m755 T ${DESTDIR}${PREFIX}/bin/T

uninstall:
	@echo removing executable files from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/T

.PHONY: all clean install uninstall
