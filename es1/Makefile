CFLAGS=-Wall -g -ggdb

.PHONY: clean all

all: es1

es1: es1.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	@rm -f *.o es1
