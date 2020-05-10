CC=gcc
CFLAGS = -g

all: clean sender receiver test

sender:
	$(CC)   src/common/communication.c \
		src/control/sender.c \
		src/common/message.c \
		src/ecc/p256.c \
		src/common/fsm.c  \
		src/common/inverse.c \
		src/common/shift.c  \
		src/common/sub.c \
		src/common/mult.c \
		src/common/mod.c \
		src/common/utilities.c  \
		src/hash/sha3.c \
		src/hash/hash.c \
		src/common/add.c \
		src/common/division.c \
		src/random/random.c \
		src/ecc/signature.c \
		src/aead/chacha20_poly1305_interface.c \
		src/aead/chacha20.c \
		src/aead/poly1305.c  -o sender $(LIBS)

receiver:
	$(CC)   src/common/communication.c \
		src/drone/receiver.c  \
                src/common/message.c \
                src/ecc/p256.c \
                src/common/fsm.c  \
                src/common/inverse.c \
                src/common/shift.c  \
                src/common/sub.c \
                src/common/mult.c \
                src/common/mod.c \
                src/common/utilities.c  \
                src/hash/sha3.c \
                src/hash/hash.c \
                src/common/add.c \
                src/common/division.c \
                src/random/random.c \
                src/ecc/signature.c \
                src/aead/chacha20_poly1305_interface.c \
                src/aead/chacha20.c \
                src/aead/poly1305.c  -o receiver $(LIBS)

#test:
#	$(CC)  fsm.c communication.c message.c main.c inverse.c shift.c  sub.c mult.c mod.c utilities.c  sha3.c hash.c add.c division.c random.c signature.c p256.c chacha20_poly1305_interface.c chacha20.c poly1305.c -o test $(LIBS)

clean:
	rm -f sender receiver test *.o
