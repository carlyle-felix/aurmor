CFLAGS 		= -Wall -g
INCLUDE		= ./include
SRC			= ./src
BINDIR		= /usr/local/bin

aurmor: main.o buffer.o package_install.o package_update.o package_uninstall.o \
		memory.o
	gcc -o aurmor $(SRC)/main.c $(SRC)/buffer.c $(SRC)/package_install.c \
		$(SRC)/package_update.c $(SRC)/package_uninstall.c $(SRC)/memory.c 

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

package_uninstall.o: $(SRC)/package_uninstall.c $(INCLUDE)/uninstall.h
	gcc -c $(SRC)/package_uninstall.c

memory.o: $(SRC)/memory.c $(INCLUDE)/memory.h
	gcc -c $(SRC)/memory.c

.PHONY: install clean uninstall
install:
	install -m 0555 aurmor $(BINDIR)

clean:
	rm aurmor main.o buffer.o package_install.o package_update.o memory.o \
		package_uninstall.o

uninstall:
	rm $(BINDIR)/aurmor