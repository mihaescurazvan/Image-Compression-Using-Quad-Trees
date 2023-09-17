build: quadtree

quadtree: main.o
	gcc -Wall -o quadtree main.o

main.o: main.c
	gcc -Wall main.c -lm -std=c11 -ggdb3 -c

run: quadtree
	./quadtree
.PHONY: clean

clean:
	rm -f quadtree