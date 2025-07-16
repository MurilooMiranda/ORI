#ifndef TABELAHASH_H
#define TABELAHASH_H

#include <stdio.h>
#include <stdlib.h>

struct tweet {
    int id;
    char texto[256];
    char **palavras;
    int qtd_palavras;
};

typedef struct hash Hash;

Hash *criaHash(int TABLE_SIZE);

void liberaHash(Hash *ha);

int insereHash_ComPalavras(Hash *ha, struct tweet tt);

int *buscaPorPalavra(Hash *ha, const char *palavra);

void exportaHashParaCSV(Hash *ha, const char *nomeArquivo);

void extrairPalavras(const char *frase, struct tweet *tw);

#endif