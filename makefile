main: main.c main.h inverse.c inverse.h shift.c shift.h hash.c hash.h gcd.c gcd.h sub.h sub.c mod.h mod.c add.h add.c utilities.h utilities.c sha3.c sha3.h 
	gcc -o main main.c inverse.c shift.c gcd.c  mod.c sub.c add.c utilities.c  sha3.c hash.c -I.
	./main
