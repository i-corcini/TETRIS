#ifndef TABULEIRO_H
#define TABULEIRO_H
#include "jogo.h"
void inicializar(Jogo *j);
void inicializar_com_config(Jogo *j, int altura, int largura, int dificuldade);
void limpar_linhas(Jogo *j);
void remover_linha_do_tabuleiro(Jogo *j, int linha);
void finalizar_peca(Jogo *j);
#endif
