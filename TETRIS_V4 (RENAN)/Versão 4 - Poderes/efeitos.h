#ifndef EFEITOS_H
#define EFEITOS_H

#include "jogo.h"

/* Animacoes de apresentacao: nao participam do estado nem da pilha do jogo. */
#define EFEITO_MAX_LINHAS 4

typedef struct {
    double linhas_tempo;
    int linhas_quantidade;
    int linhas_y[EFEITO_MAX_LINHAS];
    int linhas_cores[EFEITO_MAX_LINHAS][MAX_LARGURA];
    double poder_tempo;
    TipoPoder poder_tipo;
    int poder_x, poder_y;
    double impacto_tempo;
    int impacto_x, impacto_y;
    double nascer_tempo;
    int nascer_x, nascer_y;
    double desfazer_tempo;
} EfeitosVisuais;

void efeitos_inicializar(EfeitosVisuais *fx);
void efeitos_nova_peca(EfeitosVisuais *fx, const Jogo *j);
void efeitos_ao_fixar(EfeitosVisuais *fx, const Jogo *anterior, const Jogo *atual);
void efeitos_desfazer(EfeitosVisuais *fx, const Jogo *j);
void efeitos_atualizar(EfeitosVisuais *fx, double dt);

#endif
