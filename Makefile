CFLAGS 		= -Wall -g
INCL		= ./include
SRC		= ./source
BINDIR		= /usr/local/bin

aurmor: main.o buffer.o package_install.o package_update.o package_uninstall.o \
		memory.o package_list.o
	gcc -o aurmor $(SRC)/main.c $(SRC)/buffer.c $(SRC)/package_install.c \
		$(SRC)/package_update.c $(SRC)/package_uninstall.c $(SRC)/memory.c \
		$(SRC)/package_list.c

main.o: $(SRC)/main.c $(INCL)/install.h $(INCL)/update.h $(INCL)/uninstall.h \
		$(INCL)/memory.h
	gcc -c $(SRC)/main.c

buffer.o: $(SRC)/buffer.c $(INCL)/buffer.h $(INCL)/memory.h
	gcc -c $(SRC)/buffer.c

package_install.o: $(SRC)/package_install.c $(INCL)/install.h $(INCL)/memory.h \
		$(INCL)/buffer.h
	gcc -c $(SRC)/package_install.c

package_update.o: $(SRC)/package_update.c $(INCL)/buffer.h $(INCL)/update.h \
		$(INCL)/install.h $(INCL)/memory.h $(INCL)/list.h
	gcc -c $(SRC)/package_update.c

package_uninstall.o: $(SRC)/package_uninstall.c $(INCL)/uninstall.h $(INCL)/buffer.h \
		$(INCL)/memory.h $(INCL)/list.h
	gcc -c $(SRC)/package_uninstall.c

package_list.o: $(SRC)/package_list.c $(INCL)/list.h $(INCL)/memory.h $(INCL)/buffer.h
	gcc -c $(SRC)/package_list.c

memory.o: $(SRC)/memory.c $(INCL)/memory.h $(INCL)/list.h
	gcc -c $(SRC)/memory.c

.PHONY: install clean uninstall
install:
	install -m 0755 aurmor $(BINDIR)

clean:
	rm aurmor main.o buffer.o package_install.o package_update.o memory.o \
		package_uninstall.o package_list.o

uninstall:
	rm $(BINDIR)/aurmor
