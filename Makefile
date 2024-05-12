all: compile run
compile:
	gcc -Wall -Wpedantic main.c
run:
	./a.out
clean:
	rm -rf main.o a.out