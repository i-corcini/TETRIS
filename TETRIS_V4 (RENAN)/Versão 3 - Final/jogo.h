#ifndef JOGO_H
#define JOGO_H

/* A matriz tem capacidade fixa; cada partida escolhe a area ativa. */
#define TAM 4
#define MAX_ALTURA 40
#define MAX_LARGURA 16
#define MIN_ALTURA 20
#define MIN_LARGURA 8
#define ALTURA_PADRAO 40
#define LARGURA_PADRAO 10
#define CAPACIDADE_FILA 5
#define CAPACIDADE_PILHA 32

typedef enum {
    DIFICULDADE_FACIL = 0,
    DIFICULDADE_NORMAL = 1,
    DIFICULDADE_DIFICIL = 2
} Dificuldade;

typedef struct {
    int matriz[TAM][TAM];
    int x;
    int y;
    int tipo;
} Tetramino;

/* FIFO circular: proximas cinco pecas. */
typedef struct {
    int itens[CAPACIDADE_FILA];
    int inicio;
    int fim;
    int quantidade;
} FilaPecas;

typedef struct {
    int tabuleiro[MAX_ALTURA][MAX_LARGURA];
    int altura;
    int largura;
    int dificuldade;
    Tetramino atual;
    FilaPecas proximas;
    unsigned int semente;
    unsigned int pecas_geradas;
    int pontos;
    int linhas;
    int fim;
} Jogo;

/* Um checkpoint por peca, separado do estado do jogo. */
typedef struct {
    Jogo estados[CAPACIDADE_PILHA];
    int topo;
} PilhaEstados;

#endif
