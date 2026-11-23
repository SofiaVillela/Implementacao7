#ifndef ARQUIVOS_H
#define ARQUIVOS_H

#include <stdio.h>

#define TAM_LINHA 1000

FILE *abrir_arquivo(const char *nomeArquivo, const char *modo);
int ler_linha(FILE *file, char *linha, int numeroLinha);
int converter_endereco(char *linha, long *enderecoLogico);
int validar_arquivo_enderecos(FILE *file);
int ler_pagina(FILE *backingStore, int page, signed char *frame);
void fechar_arquivo(FILE *file, FILE *backingStore);

#endif
