#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "tabuleiro.h"
#include "tetraminos.h"

#define ALTURA 20
#define LARGURA 10

typedef struct {
    int tabuleiro[ALTURA][LARGURA];
    Tetramino atual;
    int pontos;
    int linhas;
    int fim;
} Jogo;

void limpar_linhas(Jogo *j);

void finalizar_peca(Jogo *j);

void desenhar(Jogo *j)





