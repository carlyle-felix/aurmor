CFLAGS 		= -Wall -Wextra -Wpedantic -g
INCL		= ./include
SRC			= ./source
BIN			= aurx
PREFIX		= /usr/local
DESTDIR		=


aurx: main.o util.o operation.o memory.o list.o rpc.o
	gcc -o aurx $(SRC)/main.c $(SRC)/util.c $(SRC)/operation.c \
		$(SRC)/memory.c $(SRC)/list.c $(SRC)/rpc.c -lcurl -ljson-c

main.o: $(SRC)/main.c $(INCL)/operation.h $(INCL)/memory.h \
		$(INCL)/rpc.h $(INCL)/list.h $(INCL)/util.h
	gcc -c $(SRC)/main.c

util.o: $(SRC)/util.c $(INCL)/util.h $(INCL)/memory.h
	gcc -c $(SRC)/util.c

operation.o: $(SRC)/operation.c $(INCL)/operation.h $(INCL)/memory.h \
		$(INCL)/util.h $(INCL)/list.h $(INCL)/rpc.h
	gcc -c $(SRC)/operation.c

list.o: $(SRC)/list.c $(INCL)/list.h $(INCL)/memory.h $(INCL)/util.h
	gcc -c $(SRC)/list.c

memory.o: $(SRC)/memory.c $(INCL)/memory.h $(INCL)/list.h $(INCL)/rpc.h
	gcc -c $(SRC)/memory.c

rpc.o: $(SRC)/rpc.c $(INCL)/rpc.h $(INCL)/memory.h $(INCL)/list.h \
		$(INCL)/util.h
	gcc -c $(SRC)/rpc.c

.PHONY: install clean uninstall
install:
	install -Dm755 $(BIN) $(DESTDIR)$(PREFIX)/bin/$(BIN)

clean:
	rm aurx main.o util.o operation.o list.o memory.o \
		rpc.o

uninstall:
	rm $(DESTDIR)$(PREFIX)/bin/$(BIN)
