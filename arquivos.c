#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
FILE *abrir_arquivo(const char *nomeArquivo, const char *modo){
    FILE *file = fopen(nomeArquivo, modo);
    if(file == NULL){
        fprintf(stderr, "Erro: nao foi possivel abrir o arquivo %s\n", nomeArquivo);
        return NULL;
    }
    return file;
}

int converter_endereco(char *linha, long *enderecoLogico, int *quantidadeTraduzidos){

    char *fim;
    char *vazio = linha;

    linha[strcspn(linha, "\r\n")] = '\0';
    while(isspace((unsigned char)*vazio)) vazio++;
    if(*vazio == '\0') return 1;
    *enderecoLogico = strtol(linha, &fim, 10);

    while(isspace((unsigned char)*fim)) fim++;
        if(fim == linha||*fim != '\0' || *enderecoLogico < 0 || *enderecoLogico > 65535){
            return -1;
        }

    (*quantidadeTraduzidos)++;
    return 0;
}
