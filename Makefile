CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread

vm: *.c *.h
	$(CC) $(CFLAGS) -o vm *.c

clean:
	rm -f vm

.PHONY: clean
