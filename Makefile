SRC = ttytimer.c
CC ?= gcc
BIN = ttytimer
PREFIX ?= /usr/local
INSTALLPATH = ${DESTDIR}${PREFIX}/bin

ifeq ($(shell sh -c 'which ncurses5-config>/dev/null 2>/dev/null && echo y'), y)
	CFLAGS += -Wall -g $$(ncurses5-config --cflags)
	LDFLAGS += $$(ncurses5-config --libs)
else ifeq ($(shell sh -c 'which ncursesw5-config>/dev/null 2>/dev/null && echo y'), y)
	CFLAGS += -Wall -g $$(ncursesw5-config --cflags)
	LDFLAGS += $$(ncursesw5-config --libs)
else
	CFLAGS += -Wall -g $$(pkg-config --cflags ncurses)
	LDFLAGS += $$(pkg-config --libs ncurses)
endif

ttytimer : ${SRC}
	@echo "building ${SRC}"
	${CC} ${CFLAGS} ${SRC} -o ${BIN} ${LDFLAGS}

install : ${BIN}
	@echo "installing binary file to ${INSTALLPATH}/${BIN}"
	@mkdir -p ${INSTALLPATH}
	@cp ${BIN} ${INSTALLPATH}
	@chmod 0755 ${INSTALLPATH}/${BIN}
	@echo "installed"

uninstall:
	@echo "uninstalling binary file (${INSTALLPATH})"
	@rm -f ${INSTALLPATH}/${BIN}
	@echo "${BIN} uninstalled"

clean:
	@echo "cleaning ${BIN}"
	@rm -f ${BIN}
	@echo "${BIN} cleaned"