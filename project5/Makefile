CC=./gcc209
CFLAGS=-D_BSD_SOURCE -D_GNU_SOURCE
CFLAGS=-D_DEFAULT_SOURCE

ish: main.c utils.c dynarray.c dfa.c
	$(CC) $(CFLAGS) -o ish main.c utils.c dynarray.c dfa.c

clean:
	rm -f ./ish
