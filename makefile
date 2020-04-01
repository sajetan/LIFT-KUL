main: main.c main.h inverse.c inverse.h shift.c shift.h random.c random.h hash.c hash.h  sub.h sub.c add.c add.h  utilities.h utilities.c sha3.c sha3.h mod.c mod.h mult.c mult.h 
	gcc -Wall -o main main.c inverse.c shift.c  sub.c mult.c mod.c utilities.c  sha3.c hash.c add.c random.c -I.
	./main
