FLAGS = -Wall -g
OBJS = aux_file.o palindrome.o

clean:
		rm ${OBJS}

palindrome:		aux_file.o palindrome.o
				gcc ${FLAGS} aux_file.o palindrome.o -o palindrome

aux_file.o:		aux_file.c
				gcc ${FLAGS} aux_file.c -c -o aux_file.o

palindrome.o:	palindrome.c	
				gcc ${FLAGS} palindrome.c -c -o palindrome.o