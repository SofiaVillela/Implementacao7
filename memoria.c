#include <stdio.h>
#include "memoria.h"
#include "arquivos.h"

static signed char memoria[NUM_FRAMES][TAM_FRAME];
static int tabelaPages[NUM_PAGES];
static int tabelaframe[NUM_FRAMES];
static int ultimoUso[NUM_FRAMES];
static int proximoFrame = 0;
static int tempo = 0;
static int pageFaults = 0;

void inicializarVetor(int *vetor, int tamanho, int valorAdicionar){
    for(int i = 0; i < tamanho; i++){
        vetor[i] = valorAdicionar;
    }
}

void inicializar_memoria(void){
    inicializarVetor(tabelaPages, NUM_PAGES, -1);
    inicializarVetor(tabelaframe, NUM_FRAMES, -1);
}

int pagina_na_memoria(int page){
    return tabelaPages[page] != -1;
}

int escolher_frame(int usarLRU){
    int frameEscolhido;

    if(usarLRU == 1 && pageFaults >= NUM_FRAMES){
        frameEscolhido = 0;
        for(int i = 0; i < NUM_FRAMES; i++){
            if(ultimoUso[i] < ultimoUso[frameEscolhido]) frameEscolhido = i;
        }
    } else {
        frameEscolhido = proximoFrame;
        proximoFrame++;
        if(proximoFrame == NUM_FRAMES) proximoFrame = 0;
    }
    return frameEscolhido;
}

int carregar_pagina(FILE *backingStore, int page, int usarLRU){
    int frameEscolhido = escolher_frame(usarLRU);

    int vitima = tabelaframe[frameEscolhido];
    if(vitima != -1){
        tabelaPages[vitima] = -1;
    }

    if(ler_pagina(backingStore, page, memoria[frameEscolhido]) != 0){
        return -1;
    }

    tabelaPages[page] = frameEscolhido;
    tabelaframe[frameEscolhido] = page;
    pageFaults++;
    return 0;
}

void traduzir(int page, int offset, int *enderecoFisico, signed char *valor){
    int frame = tabelaPages[page];

    tempo++;
    ultimoUso[frame] = tempo;

    *enderecoFisico = (frame * TAM_FRAME) + offset;
    *valor = memoria[frame][offset];
}

int total_page_faults(void){
    return pageFaults;
}
