all: build

build: mandel-tiles-pthread.c
	gcc -o prog fila.c mandel-tiles-pthread.c -lpthread -lm

run: build
	./prog $(ARGS)

clean: 
	/bin/rm -f *.o prog
