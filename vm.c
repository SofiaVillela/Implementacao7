#include <stdio.h>
#include "arquivos.h"
#include "memoria.h"

#define TAM_FRAME 256
#define TAM_PAGE 256

#define NUM_FRAMES 128
#define NUM_PAGES 256

signed char memoria[NUM_FRAMES][TAM_FRAME];
int tabelaPages[NUM_PAGES];
int tabelaframe[NUM_FRAMES];

int main(int argc, char **argv){

    if(argc != 2){
        fprintf(stderr, "Erro: numero de argumentos invalido\n");
        return 1;
    }
    char *nomeArquivo = argv[1];

    FILE *file = abrir_arquivo(nomeArquivo, "r");
    if(file == NULL) return 1;

    FILE *backingStore = abrir_arquivo("BACKING_STORE.bin", "rb");
    if(backingStore == NULL){
        fclose(file);
        return 1;
    }

    int contadorLinhas = 0;
    int quantidadeTraduzidos = 0;
    int proximoFrame = 0;
    int pageFaults = 0;

    inicializarVetor(tabelaPages, NUM_PAGES, -1);
    inicializarVetor(tabelaframe, NUM_FRAMES, -1);

    char linha[1000];
    while(fgets(linha, sizeof(linha), file)){
        contadorLinhas++;

        long enderecoLogico;
        int retorno = converter_endereco(linha, &enderecoLogico);

        if(retorno == -1){
            fprintf(stderr, "Erro: erro na linha %d: %s \n", contadorLinhas, linha);
            fechar_arquivo(file, backingStore);
            return 1;
        }

        if(retorno == 1) continue;
        quantidadeTraduzidos++;

        int page = (int) enderecoLogico / TAM_PAGE;
        int offset = (int) enderecoLogico % TAM_PAGE;

        if(tabelaPages[page] == -1) {
            int vitima = tabelaframe[proximoFrame];
            if(vitima != -1){
                tabelaPages[vitima] = -1;
            }

            int ler = ler_pagina(backingStore, page, memoria[proximoFrame]);
            if(ler != 0){
                fechar_arquivo(file, backingStore);
                return 1;
            }

            tabelaPages[page] = proximoFrame;
            tabelaframe[proximoFrame] = page ;
            proximoFrame++;
            if(proximoFrame == NUM_FRAMES) proximoFrame = 0;
            pageFaults++;
        }

        int frame = tabelaPages[page];
        int enderecoFisico = (frame * TAM_FRAME) + offset;
        signed char valor = memoria[frame][offset];
        printf("Virtual address: %ld Physical address: %d Value: %d\n", enderecoLogico, enderecoFisico, valor);

    }

    if(quantidadeTraduzidos == 0){
        fprintf(stderr, "Erro: arquivo nao contem enderecos\n");
        fechar_arquivo(file, backingStore);
        return 1;
    }

    printf("Page Faults = %d\n", pageFaults);
    fechar_arquivo(file, backingStore);
    return 0;
}
