CFLAGS 		= -Wall -g
INCLUDE		= ./include
SRC			= ./src
BINDIR		= /usr/local/bin

aurmgr: main.o buffer.o package_install.o package_update.o memory.o
	gcc -o aurmgr $(SRC)/main.c $(SRC)/buffer.c $(SRC)/package_install.c \
			$(SRC)/package_update.c $(SRC)/memory.c

main.o: $(SRC)/main.c $(INCLUDE)/update.h
	gcc -c $(SRC)/main.c

buffer.o: $(SRC)/buffer.c $(INCLUDE)/buffer.h $(INCLUDE)/memory.h
	gcc -c $(SRC)/buffer.c

package_install.o: $(SRC)/package_install.c $(INCLUDE)/install.h \
			 $(INCLUDE)/memory.h
	gcc -c $(SRC)/package_install.c

package_update.o: $(SRC)/package_update.c $(INCLUDE)/buffer.h \
			$(INCLUDE)/update.h $(INCLUDE)/memory.h
	gcc -c $(SRC)/package_update.c

memory.o: $(SRC)/memory.c $(INCLUDE)/memory.h
	gcc -c $(SRC)/memory.c

.PHONY: install clean
install:
	install -m 0555 aurmgr $(BINDIR)

clean:
	rm aurmgr main.o buffer.o package_install.o package_update.o memory.o