FLAGS = -Wall -g
OBJS = queue.o telephonelist.o

telephonelist:	queue.o telephonelist.o
				gcc ${FLAGS} queue.o telephonelist.o -o telephonelist 

queue.o:		queue.c
				gcc ${FLAGS} queue.c -c -o queue.o

telephonelist.o:	telephonelist.c	
					gcc ${FLAGS} telephonelist.c -c -o telephonelist.o
