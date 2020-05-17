make clean
make distclean
aclocal
autoheader
automake --add-missing
autoconf

./configure
make
