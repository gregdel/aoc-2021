CC=gcc
CFLAGS = -c -Wall
LDFLAGS =

%.o: %.c
	$(CC) -o $@ $< $(CFLAGS)

puzzle-1-1: puzzle-1-1.o
	$(CC) -o $@ $< $(LDFLAGS)

puzzle-1-2: puzzle-1-2.o
	$(CC) -o $@ $< $(LDFLAGS)

puzzle-2-1: puzzle-2-1.o
	$(CC) -o $@ $< $(LDFLAGS)

puzzle-2-2: puzzle-2-2.o
	$(CC) -o $@ $< $(LDFLAGS)

puzzle-3-1: puzzle-3-1.o
	$(CC) -o $@ $< $(LDFLAGS)

puzzle-3-2: puzzle-3-2.o
	$(CC) -o $@ $< $(LDFLAGS)

tags:
	ctags -R /usr/include/microhttpd.h .

clean:
	rm *.o tags
	find . -maxdepth 1 -type f -executable -delete

.PHONY: clean tags
