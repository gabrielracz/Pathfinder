objects = build/pathfinder.o build/pvec.o
all: $(objects) pth

build/pvec.o: src/pvec.c include/pvec.h
	gcc -c src/pvec.c -o build/pvec.o

build/pathfinder.o: src/pathfinder.c
	gcc -c src/pathfinder.c -o build/pathfinder.o

pth: $(objects)
	gcc -o pth $(objects) -lm

clean:
	rm -rf build/*.o
	rm -f pth
