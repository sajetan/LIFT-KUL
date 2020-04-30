CC=gcc
CFLAGS = -g

all: clean sender receiver test

sender:
	$(CC) -Wall communication.c sender.c message.c p256.c fsm.c  inverse.c shift.c  sub.c mult.c mod.c utilities.c  sha3.c hash.c add.c division.c random.c signature.c chacha20_poly1305_interface.c chacha20.c poly1305.c  -o sender $(LIBS)

receiver:
	$(CC) -Wall  communication.c receiver.c  message.c p256.c fsm.c  inverse.c shift.c  sub.c mult.c mod.c utilities.c  sha3.c hash.c add.c division.c random.c signature.c  chacha20_poly1305_interface.c chacha20.c poly1305.c  -o receiver $(LIBS)

test:
	$(CC) -Wall fsm.c communication.c message.c main.c inverse.c shift.c  sub.c mult.c mod.c utilities.c  sha3.c hash.c add.c division.c random.c signature.c p256.c chacha20_poly1305_interface.c chacha20.c poly1305.c -o test $(LIBS)

clean:
	rm -f sender receiver test *.o
