#include <stdio.h>
#include <string.h>
#include "arquivos.h"
#include "memoria.h"
#include "tlb.h"

static int validar_argumentos(int argc, char **argv, int *usarLRU, int *usarLRUTLB){
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
    *usarLRUTLB = (strcmp(argv[3], "lru") == 0);
    return 0;
}

int main(int argc, char **argv){
    int usarLRU, usarLRUTLB;
    if(validar_argumentos(argc, argv, &usarLRU, &usarLRUTLB) != 0) return 1;

    FILE *file = abrir_arquivo(argv[1], "r");
    if(file == NULL) return 1;

    FILE *backingStore = abrir_arquivo("BACKING_STORE.bin", "rb");
    if(backingStore == NULL){
        fclose(file);
        return 1;
    }

    inicializar_memoria();
    inicializar_tlb();

    int contadorLinhas = 0;
    int quantidadeTraduzidos = 0;
    int tlbHits = 0;
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

        int frame;
        int slot = buscar_tlb(page);

        if(slot != -1){
            tlbHits++;
            frame = frame_da_tlb(slot);
            registrar_acerto_tlb(slot, usarLRUTLB);
        } else {
            if(!pagina_na_memoria(page)){
                int paginaRemovida;
                if(carregar_pagina(backingStore, page, usarLRU, &paginaRemovida) != 0){
                    fechar_arquivo(file, backingStore);
                    return 1;
                }
                if(paginaRemovida != -1) invalidar_pagina_tlb(paginaRemovida);
            }
            frame = frame_da_pagina(page);
            slot = inserir_tlb(page, frame, usarLRUTLB);
        }

        int enderecoFisico;
        signed char valor;
        traduzir(frame, offset, &enderecoFisico, &valor);
        printf("Virtual address: %ld TLB: %d Physical address: %d Value: %d\n",
               enderecoLogico, slot, enderecoFisico, valor);
    }

    if(quantidadeTraduzidos == 0){
        fprintf(stderr, "Erro: arquivo nao contem enderecos\n");
        fechar_arquivo(file, backingStore);
        return 1;
    }

    int pageFaults = total_page_faults();
    printf("Number of Translated Addresses = %d\n", quantidadeTraduzidos);
    printf("Page Faults = %d\n", pageFaults);
    printf("Page Fault Rate = %.3f\n", (double) pageFaults / quantidadeTraduzidos);
    printf("TLB Hits = %d\n", tlbHits);
    printf("TLB Hit Rate = %.3f\n", (double) tlbHits / quantidadeTraduzidos);

    fechar_arquivo(file, backingStore);
    return 0;
}
