#ifndef MOVIMENTACAO_H
#define MOVIMENTACAO_H
#include "jogo.h"

int pode_mover(const Jogo *j, int dx, int dy, const int matriz[TAM][TAM]);
int mover(Jogo *j, int dx, int dy);
int rotacionar(Jogo *j);
int rotacionar_sentido(Jogo *j, int sentido); /* +1 horario, -1 anti-horario */
void fixar_peca(Jogo *j);
void queda_rapida(Jogo *j);

/* TAD pilha sequencial: os ultimos estados saem primeiro (LIFO). */
void pilha_inicializar(PilhaEstados *p);
int pilha_empilhar(PilhaEstados *p, const Jogo *j);
int pilha_desempilhar(PilhaEstados *p, Jogo *j);
int pilha_quantidade(const PilhaEstados *p);
int pilha_consultar(const PilhaEstados *p, Jogo *j);
int pilha_desfazer_peca(PilhaEstados *p, Jogo *j, int *peca_modificada);
#endif
