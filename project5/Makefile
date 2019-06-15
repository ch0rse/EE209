CC=./gcc209
CFLAGS=-D_BSD_SOURCE -D_GNU_SOURCE

ish: main.c utils.c dynarray.c dfa.c
	$(CC) -o ish $(CLFAGS) main.c utils.c dynarray.c dfa.c

clean:
	rm -f ./ish
