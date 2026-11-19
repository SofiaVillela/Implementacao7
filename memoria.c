#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "memoria.h"

void inicializarVetor(int *vetor, int tamanho, int valorAdicionar){
    for(int i = 0; i < tamanho; i++){
        vetor[i] = valorAdicionar;
    }

}
