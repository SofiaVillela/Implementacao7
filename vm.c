#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "arquivos.h"
#include "memoria.h"

#define NUM_FRAMES 256
#define NUM_PAGES 256

signed char memoria[NUM_FRAMES][NUM_PAGES];
int tabelaPages[NUM_PAGES];

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

    char linha[1000];
    int contadorLinhas = 0;
    long enderecoLogico;
    int page = 0;
    int offset = 0;
    int quantidadeTraduzidos = 0;
    int proximoFrame = 0;
    int pageFaults = 0;
    int frames = 0;

    inicializarVetor(tabelaPages, 256, -1);

    while(fgets(linha, sizeof(linha), file)){
        contadorLinhas++;
        int retorno = converter_endereco(linha, &enderecoLogico, &quantidadeTraduzidos);
        if(retorno != 0){
            if(retorno == -1){
                fprintf(stderr, "Erro: erro na linha %d: %s \n", contadorLinhas, linha);
                fclose(file);
                fclose(backingStore);
                return 1;
            }
            else if(retorno == 1) continue;
        }
        page = (int) enderecoLogico / 256;
        offset = (int) enderecoLogico % 256;

        if(tabelaPages[page] == -1) {
            int ler = ler_pagina(backingStore, page, memoria[proximoFrame]);
            if(ler != 0){
                fclose(file);
                fclose(backingStore);
                return 1;
            }
            tabelaPages[page] = proximoFrame;
            proximoFrame++;
            pageFaults++;
        }

        frames = tabelaPages[page];
        int enderecoFisico = (frames * 256) + offset;
        signed char valor = memoria[frames][offset];
        printf("Virtual address: %ld Physical address: %d Value: %d\n", enderecoLogico, enderecoFisico, valor);

    }
    if(quantidadeTraduzidos == 0){
        fprintf(stderr, "Erro: arquivo nao contem enderecos\n");
        fclose(file);
        fclose(backingStore);
        return 1;
    }

    printf("Page Faults = %d\n", pageFaults);
    fclose(backingStore);
    fclose(file);
    return 0;
}
