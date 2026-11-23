#include "tlb.h"

int buscar_tlb(int pagina){
    for(int i = 0; i < TAM_TLB; i++){
        if(entrada_tem_pagina(i, pagina)) return i;
    }
    return -1;
}
