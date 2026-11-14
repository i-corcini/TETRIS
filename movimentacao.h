#ifndef MOVIMENTACAO_H
#define MOVIMENTACAO_H

#include "jogo.h"

int pode_mover(Jogo *j, int dx, int dy, int matriz[TAM][TAM]);
void mover(Jogo *j, int dx, int dy);
void rotacionar(Jogo *j);
void fixar_peca(Jogo *j);
void queda_rapida(Jogo *j);

#endif
