CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread

BASE = vm.c arquivos.c memoria.c
COMUM = $(BASE) tlb.c

vm: $(COMUM) tlb_busca_threads.c *.h
	$(CC) $(CFLAGS) -o vm $(COMUM) tlb_busca_threads.c

vm_sem_threads: $(COMUM) tlb_busca_seq.c *.h
	$(CC) $(CFLAGS) -o vm_sem_threads $(COMUM) tlb_busca_seq.c

vm_sem_tlb: $(BASE) tlb_desativada.c *.h
	$(CC) $(CFLAGS) -o vm_sem_tlb $(BASE) tlb_desativada.c

vm_sem_tlb_256: $(BASE) tlb_desativada.c *.h
	$(CC) $(CFLAGS) -DNUM_FRAMES=256 -o vm_sem_tlb_256 $(BASE) tlb_desativada.c

clean:
	rm -f vm vm_sem_threads vm_sem_tlb vm_sem_tlb_256

.PHONY: clean
