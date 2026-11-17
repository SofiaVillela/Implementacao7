vm: *.c
	gcc -Wall -pthread -o vm *.c

clean:
	rm -f vm
