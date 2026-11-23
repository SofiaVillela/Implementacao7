#include "tlb.h"

int tlb_ativa(void){
    return 0;
}

void inicializar_tlb(void){
}

int buscar_tlb(int pagina){
    (void) pagina;
    return -1;
}

int entrada_tem_pagina(int slot, int pagina){
    (void) slot;
    (void) pagina;
    return 0;
}

int frame_da_tlb(int slot){
    (void) slot;
    return -1;
}

void registrar_acerto_tlb(int slot, int usarLRU){
    (void) slot;
    (void) usarLRU;
}

int inserir_tlb(int pagina, int frame){
    (void) pagina;
    (void) frame;
    return -1;
}

void invalidar_pagina_tlb(int pagina){
    (void) pagina;
}
