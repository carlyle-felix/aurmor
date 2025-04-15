CFLAGS 		= -Wall -Wextra -Wpedantic -g
INCL		= ./include
SRC			= ./source
BIN			= aurx
PREFIX		= /usr/local
DESTDIR		=


aurx: aurx.o util.o operation.o memory.o list.o rpc.o manager.o \
		pkgdata.o
	gcc -o aurx $(SRC)/aurx.c $(SRC)/util.c $(SRC)/operation.c \
		$(SRC)/memory.c $(SRC)/list.c $(SRC)/rpc.c $(SRC)/manager.c \
		$(SRC)/pkgdata.c -lcurl -ljson-c -lalpm -lpacutils

aurx.o: $(SRC)/aurx.c $(INCL)/operation.h $(INCL)/memory.h \
		$(INCL)/rpc.h $(INCL)/list.h $(INCL)/util.h $(INCL)/manager.h
	gcc -c $(SRC)/aurx.c

util.o: $(SRC)/util.c $(INCL)/util.h $(INCL)/memory.h $(INCL)/list.h
	gcc -c $(SRC)/util.c

operation.o: $(SRC)/operation.c $(INCL)/operation.h $(INCL)/memory.h \
		$(INCL)/util.h $(INCL)/list.h $(INCL)/rpc.h $(INCL)/manager.h \
		$(INCL)/pkgdata.h
	gcc -c $(SRC)/operation.c

list.o: $(SRC)/list.c $(INCL)/list.h $(INCL)/memory.h $(INCL)/util.h \
		$(INCL)/manager.h $(INCL)/pkgdata.h
	gcc -c $(SRC)/list.c

memory.o: $(SRC)/memory.c $(INCL)/memory.h  $(INCL)/manager.h $(INCL)/list.h \
		$(INCL)/rpc.h $(INCL)/util.h
	gcc -c $(SRC)/memory.c

rpc.o: $(SRC)/rpc.c $(INCL)/rpc.h $(INCL)/memory.h $(INCL)/list.h \
		$(INCL)/util.h
	gcc -c $(SRC)/rpc.c

manager.o: $(SRC)/manager.c $(INCL)/manager.h $(INCL)/list.h $(INCL)/util.h \
		$(INCL)/memory.h $(INCL)/pkgdata.h
	gcc -c $(SRC)/manager.c

pkgdata.o: $(SRC)/pkgdata.c $(INCL)/pkgdata.h $(INCL)/memory.h $(INCL)/manager.h \
		$(INCL)/util.h $(INCL)/list.h
	gcc -c $(SRC)/pkgdata.c
	

.PHONY: install clean uninstall
install:
	install -Dm755 $(BIN) $(DESTDIR)$(PREFIX)/bin/$(BIN)

clean:
	rm aurx aurx.o util.o operation.o list.o memory.o \
		rpc.o manager.o pkgdata.o

uninstall:
	rm $(DESTDIR)$(PREFIX)/bin/$(BIN)
