CFLAGS = -Wall -g

aurmgr: main.o collect.o update.o memory.o
	gcc -o aurmgr main.c collect.c update.c memory.c

main.o: main.c update.h
	gcc -c main.c

collect.o: collect.c collect.h memory.h
	gcc -c collect.c

update.o: update.c collect.h update.h memory.h
	gcc -c update.c

memory.o: memory.c memory.h
	gcc -c memory.c
