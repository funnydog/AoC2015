CFLAGS=-Wall -g -ggdb

.PHONY: clean all

all: es7 es7b

es7: es7.o
	$(CC) $(CFLAGS) -o $@ $^

es7b: es7b.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	@rm -f *.o es7 es7b
