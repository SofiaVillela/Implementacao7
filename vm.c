#include <stdio.h>
#include <string.h>
#include "arquivos.h"
#include "memoria.h"

static int validar_argumentos(int argc, char **argv, int *usarLRU){
    if(argc != 4){
        fprintf(stderr, "Erro: numero de argumentos invalido\n");
        return -1;
    }
    if(strcmp(argv[2], "fifo") != 0 && strcmp(argv[2], "lru") != 0){
        fprintf(stderr, "Erro: %s (argumento invalido) (use fifo ou lru)\n", argv[2]);
        return -1;
    }
    if(strcmp(argv[3], "fifo") != 0 && strcmp(argv[3], "lru") != 0){
        fprintf(stderr, "Erro: %s (argumento invalido) (use fifo ou lru)\n", argv[3]);
        return -1;
    }
    *usarLRU = (strcmp(argv[2], "lru") == 0);
    return 0;
}

int main(int argc, char **argv){
    int usarLRU;
    if(validar_argumentos(argc, argv, &usarLRU) != 0) return 1;

    FILE *file = abrir_arquivo(argv[1], "r");
    if(file == NULL) return 1;

    FILE *backingStore = abrir_arquivo("BACKING_STORE.bin", "rb");
    if(backingStore == NULL){
        fclose(file);
        return 1;
    }

    inicializar_memoria();

    int contadorLinhas = 0;
    int quantidadeTraduzidos = 0;
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

        if(!pagina_na_memoria(page)){
            if(carregar_pagina(backingStore, page, usarLRU) != 0){
                fechar_arquivo(file, backingStore);
                return 1;
            }
        }

        int enderecoFisico;
        signed char valor;
        traduzir(page, offset, &enderecoFisico, &valor);
        printf("Virtual address: %ld Physical address: %d Value: %d\n", enderecoLogico, enderecoFisico, valor);
    }

    if(quantidadeTraduzidos == 0){
        fprintf(stderr, "Erro: arquivo nao contem enderecos\n");
        fechar_arquivo(file, backingStore);
        return 1;
    }

    printf("Page Faults = %d\n", total_page_faults());
    fechar_arquivo(file, backingStore);
    return 0;
}
