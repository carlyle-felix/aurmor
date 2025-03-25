CFLAGS = -Wall -g

aurmgr: main.o buffer.o install.o update.o memory.o
	gcc -o aurmgr main.c buffer.c install.c update.c memory.c

main.o: main.c update.h
	gcc -c main.c

buffer.o: buffer.c buffer.h memory.h
	gcc -c buffer.c

install.o: install.c install.h memory.h
	gcc -c install.c

update.o: update.c buffer.h update.h memory.h
	gcc -c update.c

memory.o: memory.c memory.h
	gcc -c memory.c
