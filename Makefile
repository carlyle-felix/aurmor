CFLAGS 		= -Wall -g
INCLUDE		= ./include
SRC			= ./src

aurmgr: main.o buffer.o install.o update.o memory.o
	gcc -o aurmgr $(SRC)/main.c $(SRC)/buffer.c $(SRC)/install.c \
			$(SRC)/update.c $(SRC)/memory.c

main.o: $(SRC)/main.c $(INCLUDE)/update.h
	gcc -c $(SRC)/main.c

buffer.o: $(SRC)/buffer.c $(INCLUDE)/buffer.h $(INCLUDE)/memory.h
	gcc -c $(SRC)/buffer.c

install.o: $(SRC)/install.c $(INCLUDE)/install.h $(INCLUDE)/memory.h
	gcc -c $(SRC)/install.c

update.o: $(SRC)/update.c $(INCLUDE)/buffer.h $(INCLUDE)/update.h \
			$(INCLUDE)/memory.h
	gcc -c $(SRC)/update.c

memory.o: $(SRC)/memory.c $(INCLUDE)/memory.h
	gcc -c $(SRC)/memory.c
