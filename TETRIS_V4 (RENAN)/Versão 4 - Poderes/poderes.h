#ifndef PODERES_H
#define PODERES_H
#include "jogo.h"

/* Lista sequencial: inserir, buscar por coordenadas, consultar, remover. */
void lista_inicializar(ListaPoderes *lista);
int lista_inserir(ListaPoderes *lista, PoderBloco poder);
int lista_buscar(const ListaPoderes *lista, int x, int y);
int lista_obter(const ListaPoderes *lista, int indice, PoderBloco *saida);
int lista_remover(ListaPoderes *lista, int indice);

/* Lógica: um bloco especial acompanha cada peça premiada. */
void poderes_preparar_peca(Jogo *j);
void poderes_fixar_peca(Jogo *j);
void poderes_linha_removida(Jogo *j, int linha);
void poderes_atualizar(Jogo *j, double dt);
void poderes_verificar_ativacoes(Jogo *j);
const char *poder_nome(TipoPoder tipo);
#endif
