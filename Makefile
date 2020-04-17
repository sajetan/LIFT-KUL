CC=gcc
CFLAGS = -g 

all: clean sender receiver test

sender:
	$(CC) communication.c sender.c message.c -o sender $(LIBS)

receiver:
	$(CC) communication.c receiver.c  message.c -o receiver $(LIBS)

test:
	$(CC) communication.c  message.c main.c inverse.c shift.c  sub.c mult.c mod.c utilities.c  sha3.c hash.c add.c division.c random.c signature.c p256.c -o test $(LIBS)

clean:
	rm -f sender receiver  *.o
