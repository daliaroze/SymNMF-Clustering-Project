CC = gcc
CFLAGS = -ansi -Wall -Wextra -Werror -pedantic-errors

symnmf: symnmf.o support_functions.o symnmf_operations.o 
	$(CC) -o symnmf symnmf.o support_functions.o symnmf_operations.o $(CFLAGS) -lm

symnmf.o: symnmf.c symnmf.h
	$(CC) -c symnmf.c $(CFLAGS)

support_functions.o: support_functions.c symnmf.h
	$(CC) -c support_functions.c $(CFLAGS)

symnmf_operations.o: symnmf_operations.c symnmf.h
	$(CC) -c symnmf_operations.c $(CFLAGS)

clean:
	rm -f *.o symnmf