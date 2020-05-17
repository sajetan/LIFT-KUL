CC=gcc
CFLAGS = -g -O2 -Wno-unused-result -Wall -pedantic -ansi -std=gnu11 -funsigned-char -Wstrict-aliasing

all: clean sender receiver

sender:
	$(CC) communication.c \
			sender.c  \
			../src/common/utilities.c \
			../src/random/random_gen.c \
			../src/aead/chacha20_poly1305_interface.c \
			../src/aead/chacha20.c \
			../src/hash/hash.c \
			../src/hash/sha3.c \
			../src/aead/poly1305.c  -o sender $(LIBS) $(CFLAGS)

receiver:
	$(CC) communication.c \
			receiver.c \
			../src/common/utilities.c \
			../src/random/random_gen.c \
			../src/aead/chacha20_poly1305_interface.c \
			../src/aead/chacha20.c \
			../src/hash/hash.c \
			../src/hash/sha3.c \
			../src/aead/poly1305.c -o receiver $(LIBS) $(CFLAGS)

clean:
	rm -f sender receiver *.o
