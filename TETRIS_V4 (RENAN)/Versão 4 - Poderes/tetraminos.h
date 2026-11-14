#ifndef TETRAMINOS_H
#define TETRAMINOS_H
#include "jogo.h"
void fila_inicializar(FilaPecas *f);
int fila_inserir(FilaPecas *f, int tipo);
int fila_remover(FilaPecas *f, int *tipo);
int fila_consultar(const FilaPecas *f, int *tipo);
int fila_elemento(const FilaPecas *f, int posicao, int *tipo);
void iniciar_fila_pecas(Jogo *j, unsigned int semente);
void nova_peca(Jogo *j);
int obter_formato(int tipo, int saida[TAM][TAM]);
#endif
