CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread

COMUM = vm.c arquivos.c memoria.c tlb.c

vm: $(COMUM) tlb_busca_threads.c *.h
	$(CC) $(CFLAGS) -o vm $(COMUM) tlb_busca_threads.c

vm_sem_threads: $(COMUM) tlb_busca_seq.c *.h
	$(CC) $(CFLAGS) -o vm_sem_threads $(COMUM) tlb_busca_seq.c

clean:
	rm -f vm vm_sem_threads

.PHONY: clean
