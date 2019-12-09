CFLAGS=-Wall -g -ggdb
LDFLAGS=-lcrypto

.PHONY: clean all

all: es4

es4: es4.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	@rm -f *.o es4
