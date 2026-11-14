#ifndef JOGO_H
#define JOGO_H

#define TAM 4
#define ALTURA 20
#define LARGURA 10

typedef struct {
    int matriz[TAM][TAM];
    int x;
    int y;
    int tipo;
} Tetramino;

typedef struct {
    int tabuleiro[ALTURA][LARGURA];
    Tetramino atual;
    int pontos;
    int linhas;
    int fim;
} Jogo;

#endif
