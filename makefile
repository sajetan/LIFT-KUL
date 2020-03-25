main: main.c main.h inverse.c inverse.h word.c word.h gcd.c gcd.h begcd.c begcd.h sub.h sub.c mod.h mod.c add.h add.c utilities.h utilities.c
	gcc -o main main.c inverse.c word.c gcd.c begcd.c mod.c sub.c add.c utilities.c  -I.
	./main
