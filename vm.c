#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "arquivos.h"

int main(int argc, char **argv){

    if(argc != 2){
        fprintf(stderr, "Erro: numero de argumentos invalido\n");
        return 1;
    }
    char *nomeArquivo = argv[1];

    FILE *file = abrir_arquivo(nomeArquivo, "r");
    if(file == NULL) return 1;
    char linha[1000];
    int contadorLinhas = 0;
    long enderecoLogico;
    int page = 0;
    int offset = 0;
    int quantidadeTraduzidos = 0;

    while(fgets(linha, sizeof(linha), file)){
        contadorLinhas++;
        int retorno = converter_endereco(linha, &enderecoLogico, &quantidadeTraduzidos);
        if(retorno != 0){
            if(retorno == -1){
                fprintf(stderr, "Erro: erro na linha %d: %s \n", contadorLinhas, linha);
                fclose(file);
                return 1;
            }
            else if(retorno == 1) continue;
        }

        page = (int) enderecoLogico / 256;
        offset = (int) enderecoLogico % 256;

    }
    if(quantidadeTraduzidos == 0){
        fprintf(stderr, "Erro: arquivo nao contem enderecos\n");
        fclose(file);
        return 1;
    }
    printf("teste pag e offset: %d  %d\n", page, offset);
    fclose(file);
    return 0;
}
