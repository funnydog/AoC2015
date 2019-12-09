CFLAGS=-Wall -g -ggdb

.PHONY: clean all

all: es13

es13: es13.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	@rm -f *.o es13
