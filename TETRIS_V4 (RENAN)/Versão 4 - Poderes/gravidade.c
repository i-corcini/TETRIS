#include "gravidade.h"
#include "movimentacao.h"
#include "tabuleiro.h"

int nivel_atual(const Jogo *j) { return 1 + j->linhas / 10; }

double intervalo_gravidade(const Jogo *j) {
    int nivel = nivel_atual(j);
    double intervalo = 0.58;
    if (j->dificuldade == DIFICULDADE_FACIL) intervalo = 0.86;
    if (j->dificuldade == DIFICULDADE_DIFICIL) intervalo = 0.34;
    for (int i = 1; i < nivel; i++) intervalo *= 0.88;
    if (intervalo < 0.08) intervalo = 0.08;
    return intervalo;
}

void aplicar_gravidade(Jogo *j) {
    if (!j->fim) {
        if (!mover(j, 0, 1)) finalizar_peca(j);
    }
}
