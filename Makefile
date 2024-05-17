CC = gcc
CFLAGS = -Werror -Wpedantic -g

SERVER_SRC =  handlers.c logger.c responses.c utils.c socket_setup.c html_helpers.c file_handling.c
SERVER_OBJS = $(SERVER_SRC:.c=.o)

all: clean compile run

compile: $(SERVER_OBJS) main.o
	$(CC) $(CFLAGS) $(SERVER_OBJS) main.o -o a.out

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run:
	./a.out

.PHONY: clean
clean:
	echo "Cleaned!"
	rm -rf *.o a.out
