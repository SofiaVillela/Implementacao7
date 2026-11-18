#ifndef ARQUIVOS_H
#define ARQUIVOS_H

#include <stdio.h>

FILE *abrir_arquivo(const char *nomeArquivo, const char *modo);
int converter_endereco(char *linha, long *enderecoLogico, int *quantidadeTraduzidos);

#endif
