#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char **argv){

    if(argc != 2){
        fprintf(stderr, "Erro: numero de argumentos invalido\n");
        return 1;
    }
    char *nomeArquivo = argv[1];

    FILE *file = fopen(nomeArquivo, "r");
    if(file == NULL){
        fprintf(stderr, "Erro: nao foi possivel abrir o arquivo %s\n", nomeArquivo);
        return 1;
    }

    char linha[1000];
    char *fim;
    int contadorLinhas = 0;
    long endereco_logico;
    int page;
    int offset;
    while(fgets(linha, sizeof(linha), file)){
        contadorLinhas++;

        linha[strcspn(linha, "\r\n")] = '\0';
        endereco_logico = strtol(linha, &fim, 10);

        while(isspace((unsigned char)*fim)) fim++;
        if(fim == linha||*fim != '\0' || endereco_logico < 0 || endereco_logico > 65535){
            fprintf(stderr, "Erro: erro na linha %d, %s\n", contadorLinhas, linha);
            fclose(file);
            return 1;
        }

        page = (int) endereco_logico / 256;
        offset = (int) endereco_logico % 256;

        printf("page: %d \n", page);
        printf("offset: %d \n", offset);
    }
    fclose(file);
    return 0;
}
