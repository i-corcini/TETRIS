#ifndef TABULEIRO_H
#define TABULEIRO_H
#include "jogo.h"
void inicializar(Jogo *j);
void limpar_linhas(Jogo *j);
void finalizar_peca(Jogo *j);
void desenhar(const Jogo *j, const PilhaEstados *historico, int vista_completa);
#endif
