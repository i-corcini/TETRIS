#ifndef JOGO_H
#define JOGO_H

#define TAM 4
#define ALTURA 40
#define LARGURA 10
#define CAPACIDADE_FILA 5
#define CAPACIDADE_PILHA 32

typedef struct {
    int matriz[TAM][TAM];
    int x;
    int y;
    int tipo;
} Tetramino;

/* FIFO: as proximas cinco pecas, em um vetor circular. */
typedef struct {
    int itens[CAPACIDADE_FILA];
    int inicio;
    int fim;
    int quantidade;
} FilaPecas;

typedef struct {
    int tabuleiro[ALTURA][LARGURA];
    Tetramino atual;
    FilaPecas proximas;
    unsigned int semente;  /* Sorteio reproduzivel ao desfazer. */
    int pontos;
    int linhas;
    int fim;
} Jogo;

/* Pilha fora de Jogo: evita uma estrutura que contenha a si mesma. */
typedef struct {
    Jogo estados[CAPACIDADE_PILHA];
    int topo;
} PilhaEstados;

#endif
