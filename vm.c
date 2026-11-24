#include <stdio.h>
#include <string.h>
#include "arquivos.h"
#include "memoria.h"
#include "tlb.h"

#define ARQ_SAIDA "correct.txt"

static int algoritmo_valido(const char *nome){
    return strcmp(nome, "fifo") == 0 || strcmp(nome, "lru") == 0;
}

static int validar_argumentos(int argc, char **argv, int *usarLRU, int *usarLRUTLB){
    if(argc != 4){
        fprintf(stderr, "Erro: numero de argumentos invalido\n");
        return -1;
    }
    if(!algoritmo_valido(argv[2])){
        fprintf(stderr, "Erro: %s (argumento invalido) (use fifo ou lru)\n", argv[2]);
        return -1;
    }
    if(!algoritmo_valido(argv[3])){
        fprintf(stderr, "Erro: %s (argumento invalido) (use fifo ou lru)\n", argv[3]);
        return -1;
    }
    *usarLRU = (strcmp(argv[2], "lru") == 0);
    *usarLRUTLB = (strcmp(argv[3], "lru") == 0);
    return 0;
}

static int frame_pela_tabela(FILE *backingStore, int page, int usarLRU){
    if(!pagina_na_memoria(page)){
        int paginaRemovida;
        if(carregar_pagina(backingStore, page, usarLRU, &paginaRemovida) != 0){
            return -1;
        }

        if(paginaRemovida != -1) invalidar_pagina_tlb(paginaRemovida);
    }
    return frame_da_pagina(page);
}

static void imprimir_resumo(FILE *saida, int traduzidos, int tlbHits){
    int pageFaults = total_page_faults();

    fprintf(saida, "Number of Translated Addresses = %d\n", traduzidos);
    fprintf(saida, "Page Faults = %d\n", pageFaults);
    fprintf(saida, "Page Fault Rate = %.3f\n", (double) pageFaults / traduzidos);

    if(tlb_ativa()){
        fprintf(saida, "TLB Hits = %d\n", tlbHits);
        fprintf(saida, "TLB Hit Rate = %.3f\n", (double) tlbHits / traduzidos);
    }
}

static void imprimir_traducao(FILE *saida, long enderecoLogico, int slot, int enderecoFisico, signed char valor){
    if(tlb_ativa()){
        fprintf(saida, "Virtual address: %ld TLB: %d Physical address: %d Value: %d\n",
                enderecoLogico, slot, enderecoFisico, valor);
    } else {
        fprintf(saida, "Virtual address: %ld Physical address: %d Value: %d\n",
                enderecoLogico, enderecoFisico, valor);
    }
}

static int abortar(FILE *file, FILE *backingStore, FILE *saida){
    fechar_arquivo(file, backingStore);
    fclose(saida);
    remove(ARQ_SAIDA);
    return 1;
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

    if(validar_arquivo_enderecos(file) == -1){
        fechar_arquivo(file, backingStore);
        return 1;
    }

    FILE *saida = abrir_arquivo(ARQ_SAIDA, "w");
    if(saida == NULL){
        fechar_arquivo(file, backingStore);
        return 1;
    }

    inicializar_memoria();
    inicializar_tlb();

    int contadorLinhas = 0;
    int quantidadeTraduzidos = 0;
    int tlbHits = 0;
    char linha[TAM_LINHA];
    int lido;

    while((lido = ler_linha(file, linha, contadorLinhas + 1)) == 1){
        contadorLinhas++;

        long enderecoLogico;
        int retorno = converter_endereco(linha, &enderecoLogico);
        if(retorno == 1) continue;
        if(retorno == -1) return abortar(file, backingStore, saida);
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
            frame = frame_pela_tabela(backingStore, page, usarLRU);
            if(frame == -1) return abortar(file, backingStore, saida);
            slot = inserir_tlb(page, frame);
        }

        int enderecoFisico;
        signed char valor;
        traduzir(frame, offset, &enderecoFisico, &valor);
        imprimir_traducao(saida, enderecoLogico, slot, enderecoFisico, valor);
    }
    if(lido == -1) return abortar(file, backingStore, saida);

    imprimir_resumo(saida, quantidadeTraduzidos, tlbHits);
    fechar_arquivo(file, backingStore);

    if(fclose(saida) != 0){
        fprintf(stderr, "Erro: nao foi possivel gravar %s\n", ARQ_SAIDA);
        return 1;
    }

    imprimir_resumo(stdout, quantidadeTraduzidos, tlbHits);
    printf("Resultado gravado em %s\n", ARQ_SAIDA);
    return 0;
}
