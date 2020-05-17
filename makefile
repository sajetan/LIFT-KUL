CC=gcc
CFLAGS = -g

all: clean sender receiver test

sender:
	$(CC)  communication.c sender.c message.c  p256.c p256_parameters.c  fsm.c  inverse.c shift.c  sub.c mult.c mod.c utilities.c  sha3.c hash.c add.c division.c random_gen.c signature.c chacha20_poly1305_interface.c chacha20.c poly1305.c crc.c -o sender -lpthread $(LIBS)

receiver:
	$(CC)   communication.c receiver.c  message.c  p256.c p256_parameters.c fsm.c  inverse.c shift.c  sub.c mult.c mod.c utilities.c  sha3.c hash.c add.c division.c random_gen.c signature.c chacha20_poly1305_interface.c chacha20.c poly1305.c crc.c  -o receiver -lpthread $(LIBS)

test:
	$(CC)  fsm.c communication.c message.c main.c inverse.c shift.c  sub.c mult.c mod.c utilities.c  sha3.c hash.c add.c division.c random_gen.c signature.c p256.c p256_parameters.c chacha20_poly1305_interface.c chacha20.c poly1305.c crc.c -o  test -lpthread $(LIBS)

clean:
	rm -f sender receiver test *.o
