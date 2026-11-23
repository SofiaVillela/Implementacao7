#include <stdio.h>
#include <pthread.h>
#include "tlb.h"

typedef struct {
    int slot;
    int pagina;
    int achou;
} ArgBusca;

static void *buscar_no_slot(void *arg){
    ArgBusca *a = (ArgBusca *) arg;
    a->achou = entrada_tem_pagina(a->slot, a->pagina);
    return NULL;
}

int buscar_tlb(int pagina){
    pthread_t threads[TAM_TLB];
    ArgBusca args[TAM_TLB];
    int criada[TAM_TLB];
    int resultado = -1;

    for(int i = 0; i < TAM_TLB; i++){
        args[i].slot = i;
        args[i].pagina = pagina;
        args[i].achou = 0;
        criada[i] = (pthread_create(&threads[i], NULL, buscar_no_slot, &args[i]) == 0);
        if(!criada[i]){
            buscar_no_slot(&args[i]);
        }
    }

    for(int i = 0; i < TAM_TLB; i++){
        if(criada[i]) pthread_join(threads[i], NULL);
        if(args[i].achou) resultado = i;
    }
    return resultado;
}
