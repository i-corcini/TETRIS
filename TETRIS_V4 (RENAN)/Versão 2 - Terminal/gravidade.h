#ifndef GRAVIDADE_H
#define GRAVIDADE_H
#include "jogo.h"

/* Cronometros independentes das regras e do terminal. */
typedef struct {
    double tempo_queda;
    double tempo_contato;
} EstadoGravidade;

enum {
    GRAVIDADE_SEM_MUDANCA = 0,
    GRAVIDADE_MOVEU = 1,
    GRAVIDADE_FIXOU = 2
};

int nivel_atual(const Jogo *j);
double intervalo_gravidade(const Jogo *j);
void aplicar_gravidade(Jogo *j);
void gravidade_reiniciar(EstadoGravidade *g);
int gravidade_atualizar(Jogo *j, EstadoGravidade *g, double delta_segundos);
#endif
