all: clean compile run
compile:
	gcc -Wall -Wpedantic main.c
run:
	./a.out
clean:
	echo "Cleaned!"
	rm -rf main.o a.out