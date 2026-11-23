#ifndef TLB_H
#define TLB_H

#define TAM_TLB 16

void inicializar_tlb(void);

int buscar_tlb(int pagina);

int entrada_tem_pagina(int slot, int pagina);

int  frame_da_tlb(int slot);
void registrar_acerto_tlb(int slot, int usarLRU);
int  inserir_tlb(int pagina, int frame);
void invalidar_pagina_tlb(int pagina);

#endif
