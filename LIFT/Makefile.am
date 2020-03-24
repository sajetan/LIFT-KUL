SOURCES = src/common/utilities.c \
	  src/common/chacha20_encrypt.c \
	  src/common/chacha20_decrypt.c


if DEBUG
AM_CFLAGS = -g3 -O0
AM_CXXFLAGS = -g3 -O0
else
AM_CFLAGS = -O2
AM_CXXFLAGS = -O2
endif


bin_PROGRAMS = test
test_SOURCES = ${SOURCES}
#test_LDADD = ${LDADD}
