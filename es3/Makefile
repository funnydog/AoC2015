CFLAGS=-Wall -g -ggdb

.PHONY: clean all

all: es3

es3: es3.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	@rm -f *.o es3
