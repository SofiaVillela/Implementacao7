#ifndef MEMORIA_H
#define MEMORIA_H

#include <stdio.h>

#define TAM_FRAME  256
#define TAM_PAGE   256
#define NUM_FRAMES 128
#define NUM_PAGES  256

void inicializarVetor(int *vetor, int tamanho, int valorAdicionar);
void inicializar_memoria(void);
int  pagina_na_memoria(int page);
int  frame_da_pagina(int page);
int  escolher_frame(int usarLRU);
int  carregar_pagina(FILE *backingStore, int page, int usarLRU, int *paginaRemovida);
void traduzir(int frame, int offset, int *enderecoFisico, signed char *valor);
int  total_page_faults(void);

#endif
