#include <stdio.h>
#include "tlb.h"

static int tlbPagina[TAM_TLB];
static int tlbFrame[TAM_TLB];
static int tlbTempo[TAM_TLB];
static int relogioTLB = 0;

static int novo_carimbo(void){
    relogioTLB++;
    return relogioTLB;
}

static void esvaziar_slot(int slot){
    tlbPagina[slot] = -1;
    tlbFrame[slot] = -1;
    tlbTempo[slot] = -1;
}

void inicializar_tlb(void){
    for(int i = 0; i < TAM_TLB; i++){
        esvaziar_slot(i);
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
        tlbTempo[slot] = novo_carimbo();
    }
}

static int escolher_slot_vitima(void){
    int vitima = 0;
    for(int i = 1; i < TAM_TLB; i++){
        if(tlbTempo[i] < tlbTempo[vitima]) vitima = i;
    }
    return vitima;
}

int inserir_tlb(int pagina, int frame){
    int slot = escolher_slot_vitima();

    tlbPagina[slot] = pagina;
    tlbFrame[slot] = frame;
    tlbTempo[slot] = novo_carimbo();
    return slot;
}

void invalidar_pagina_tlb(int pagina){
    for(int i = 0; i < TAM_TLB; i++){
        if(tlbPagina[i] == pagina) esvaziar_slot(i);
    }
}
