#include <stdio.h>
#include "tlb.h"

static int tlbPagina[TAM_TLB];
static int tlbFrame[TAM_TLB];
static int tlbTempo[TAM_TLB];
static int relogioTLB = 0;

void inicializar_tlb(void){
    for(int i = 0; i < TAM_TLB; i++){
        tlbPagina[i] = -1;
        tlbFrame[i] = -1;
        tlbTempo[i] = -1;
    }
}

int entrada_tem_pagina(int slot, int pagina){
    return tlbPagina[slot] != -1 && tlbPagina[slot] == pagina;
}

int frame_da_tlb(int slot){
    return tlbFrame[slot];
}

void registrar_acerto_tlb(int slot, int usarLRU){
    if(usarLRU == 1){
        relogioTLB++;
        tlbTempo[slot] = relogioTLB;
    }
}

int inserir_tlb(int pagina, int frame, int usarLRU){
    (void) usarLRU;

    int vitima = 0;
    for(int i = 1; i < TAM_TLB; i++){
        if(tlbTempo[i] < tlbTempo[vitima]) vitima = i;
    }

    relogioTLB++;
    tlbPagina[vitima] = pagina;
    tlbFrame[vitima] = frame;
    tlbTempo[vitima] = relogioTLB;
    return vitima;
}

void invalidar_pagina_tlb(int pagina){
    for(int i = 0; i < TAM_TLB; i++){
        if(tlbPagina[i] == pagina){
            tlbPagina[i] = -1;
            tlbFrame[i] = -1;
            tlbTempo[i] = -1;
        }
    }
}
