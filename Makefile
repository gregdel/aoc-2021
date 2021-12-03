CC=gcc
CFLAGS = -c -Wall
LDFLAGS =

%.o: %.c
	$(CC) -o $@ $< $(CFLAGS)

puzzle-1: puzzle-1.o
	$(CC) -o $@ $< $(LDFLAGS)

puzzle-2: puzzle-2.o
	$(CC) -o $@ $< $(LDFLAGS)

tags:
	ctags -R /usr/include/microhttpd.h .

clean:
	rm *.o tags
