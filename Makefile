CC=gcc
CFLAGS=I

build:
	$(CC) -o pvg pvg.c -lncursesw -lpthread

install:
	install pvg /usr/sbin/pvg -m "755"

clean:
	rm pvg
