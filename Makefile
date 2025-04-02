CFLAGS 		= -Wall -g

INCL		= ./include
SRC		= ./source
BINDIR		= /usr/local

aurmor: main.o buffer.o install.o update.o uninstall.o \
		memory.o list.o rpc.o
	gcc -o aurmor $(SRC)/main.c $(SRC)/buffer.c $(SRC)/install.c \
		$(SRC)/update.c $(SRC)/uninstall.c $(SRC)/memory.c \
		$(SRC)/list.c $(SRC)/rpc.c -lcurl -ljson-c

main.o: $(SRC)/main.c $(INCL)/install.h $(INCL)/update.h $(INCL)/uninstall.h \
		$(INCL)/memory.h $(INCL)/rpc.h $(INCL)/list.h $(INCL)/buffer.h
	gcc -c $(SRC)/main.c

buffer.o: $(SRC)/buffer.c $(INCL)/buffer.h $(INCL)/memory.h
	gcc -c $(SRC)/buffer.c

install.o: $(SRC)/install.c $(INCL)/install.h $(INCL)/memory.h \
		$(INCL)/buffer.h
	gcc -c $(SRC)/install.c

update.o: $(SRC)/update.c $(INCL)/buffer.h $(INCL)/update.h \
		$(INCL)/install.h $(INCL)/memory.h $(INCL)/list.h
	gcc -c $(SRC)/update.c

uninstall.o: $(SRC)/uninstall.c $(INCL)/uninstall.h $(INCL)/buffer.h \
		$(INCL)/memory.h $(INCL)/list.h
	gcc -c $(SRC)/uninstall.c

list.o: $(SRC)/list.c $(INCL)/list.h $(INCL)/memory.h $(INCL)/buffer.h
	gcc -c $(SRC)/list.c

memory.o: $(SRC)/memory.c $(INCL)/memory.h $(INCL)/list.h $(INCL)/rpc.h
	gcc -c $(SRC)/memory.c

rpc.o: $(SRC)/rpc.c $(INCL)/rpc.h $(INCL)/memory.h $(INCL)/list.h \
		$(INCL)/buffer.h
	gcc -c $(SRC)/rpc.c

.PHONY: install clean uninstall
install:
	install -m 0755 aurmor $(BINDIR)

clean:
	rm aurmor main.o buffer.o install.o update.o memory.o \
		uninstall.o list.o rpc.o

uninstall:
	rm $(BINDIR)/bin/aurmor
