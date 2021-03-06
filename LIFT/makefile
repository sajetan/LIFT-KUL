CC=gcc
CFLAGS = -g -O2 -Wno-unused-result -Wall -pedantic -ansi -std=gnu11 -funsigned-char -Wstrict-aliasing
LIBS = -lpthread

all: clean control_launcher drone_launcher

control_launcher:
	$(CC)   src/common/communication.c \
		src/control/control_launcher.c \
		src/common/message.c \
		src/ecc/p256.c \
		src/drone/drone_utils.c \
		src/control/control_utils.c \
		src/ecc/p256_parameters.c \
		src/common/fsm.c  \
		src/common/fsm_utils.c \
		src/common/inverse.c \
		src/common/shift.c  \
		src/common/sub.c \
		src/common/mult.c \
		src/common/mod.c \
		src/common/crc.c \
		src/common/utilities.c  \
		src/hash/sha3.c \
		src/hash/hash.c \
		src/common/add.c \
		src/common/division.c \
		src/random/random_gen.c \
		src/ecc/signature.c \
		src/aead/chacha20_poly1305_interface.c \
		src/aead/chacha20.c \
		src/aead/poly1305.c  -o control_launcher  $(LIBS) $(CFLAGS)

drone_launcher:
	$(CC)   src/common/communication.c \
		src/drone/drone_launcher.c  \
		src/common/message.c \
		src/ecc/p256.c \
		src/ecc/p256_parameters.c \
		src/common/fsm.c  \
		src/common/fsm_utils.c \
		src/common/inverse.c \
		src/common/shift.c  \
		src/common/sub.c \
		src/common/mult.c \
		src/common/mod.c \
		src/common/crc.c \
		src/common/utilities.c  \
		src/hash/sha3.c \
		src/hash/hash.c \
		src/common/add.c \
		src/common/division.c \
		src/random/random_gen.c \
		src/ecc/signature.c \
		src/aead/chacha20_poly1305_interface.c \
		src/aead/chacha20.c \
		src/aead/poly1305.c  \
		src/drone/drone_utils.c -o drone_launcher  $(LIBS) $(CFLAGS)

#test:
#	$(CC)  fsm.c communication.c message.c main.c inverse.c shift.c  sub.c mult.c mod.c utilities.c  sha3.c hash.c add.c division.c random_gen.c signature.c p256.c chacha20_poly1305_interface.c chacha20.c poly1305.c -o test $(LIBS)

clean:
	rm -f control_launcher drone_launcher *.o
