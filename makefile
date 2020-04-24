CC=gcc
CFLAGS = -g 

all: clean sender receiver test

sender:
	$(CC) -Wall sender.c fsm.c communication.c message.c inverse.c shift.c  sub.c mult.c mod.c utilities.c  sha3.c hash.c add.c division.c random.c signature.c p256.c -o sender $(LIBS)

receiver:
	$(CC) -Wall receiver.c fsm.c communication.c message.c inverse.c shift.c  sub.c mult.c mod.c utilities.c  sha3.c hash.c add.c division.c random.c signature.c p256.c -o receiver $(LIBS)

test:
	$(CC) -Wall poly1305.c chacha20.c chacha20_poly1305_interface.c fsm.c communication.c message.c main.c inverse.c shift.c  sub.c mult.c mod.c utilities.c  sha3.c hash.c add.c division.c random.c signature.c p256.c -o test $(LIBS)



clean:
	rm -f sender receiver test *.o
