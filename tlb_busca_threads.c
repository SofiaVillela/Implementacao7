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

static int disparar_thread(pthread_t *thread, ArgBusca *arg, int slot, int pagina){
    arg->slot = slot;
    arg->pagina = pagina;
    arg->achou = 0;

    if(pthread_create(thread, NULL, buscar_no_slot, arg) == 0) return 1;

    buscar_no_slot(arg);
    return 0;
}

int buscar_tlb(int pagina){
    pthread_t threads[TAM_TLB];
    ArgBusca args[TAM_TLB];
    int criada[TAM_TLB];
    int resultado = -1;

    for(int i = 0; i < TAM_TLB; i++){
        criada[i] = disparar_thread(&threads[i], &args[i], i, pagina);
    }

    for(int i = 0; i < TAM_TLB; i++){
        if(criada[i]) pthread_join(threads[i], NULL);
        if(args[i].achou) resultado = i;
    }
    return resultado;
}
