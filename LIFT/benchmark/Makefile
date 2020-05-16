CC=gcc
CFLAGS = -g 

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
                ../src/aead/poly1305.c  -o sender $(LIBS)

receiver:
	$(CC) communication.c \
		receiver.c \
		../src/common/utilities.c \
		../src/random/random_gen.c \
                ../src/aead/chacha20_poly1305_interface.c \
                ../src/aead/chacha20.c \
		../src/hash/hash.c \
		../src/hash/sha3.c \
                ../src/aead/poly1305.c -o receiver $(LIBS)

clean:
	rm -f sender receiver *.o
