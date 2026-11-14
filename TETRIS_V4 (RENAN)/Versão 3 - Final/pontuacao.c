#include "pontuacao.h"

void inicializar_pontuacao(Jogo *j) {
    j->pontos = 0;
    j->linhas = 0;
}

void atualizar_pontuacao(Jogo *j, int linhas_completas) {
    j->linhas += linhas_completas;

    if (linhas_completas == 1)
        j->pontos += 100;
    else if (linhas_completas == 2)
        j->pontos += 300;
    else if (linhas_completas == 3)
        j->pontos += 500;
    else if (linhas_completas == 4)
        j->pontos += 800;
}
