#include "movimentacao.h"

int pode_mover(Jogo *j, int dx, int dy, int matriz[TAM][TAM]) {
    int i, k;
    int nx, ny;

    for (i = 0; i < TAM; i++) {
        for (k = 0; k < TAM; k++) {

            if (matriz[i][k] == 0)
                continue;

            nx = j->atual.x + k + dx;
            ny = j->atual.y + i + dy;

            if (nx < 0 || nx >= LARGURA)
                return 0;

            if (ny < 0 || ny >= ALTURA)
                return 0;

            if (j->tabuleiro[ny][nx] != 0)
                return 0;
        }
    }

    return 1;
}

void mover(Jogo *j, int dx, int dy) {
    if (pode_mover(j, dx, dy, j->atual.matriz)) {
        j->atual.x += dx;
        j->atual.y += dy;
    }
}

void rotacionar(Jogo *j) {
    int nova[TAM][TAM] = {0};
    int i, k;

    for (i = 0; i < TAM; i++) {
        for (k = 0; k < TAM; k++) {
            nova[k][TAM - 1 - i] =
                j->atual.matriz[i][k];
        }
    }

    if (pode_mover(j, 0, 0, nova)) {
        for (i = 0; i < TAM; i++) {
            for (k = 0; k < TAM; k++) {
                j->atual.matriz[i][k] = nova[i][k];
            }
        }
    }
}

void fixar_peca(Jogo *j) {
    int i, k;
    int x, y;

    for (i = 0; i < TAM; i++) {
        for (k = 0; k < TAM; k++) {

            if (j->atual.matriz[i][k] == 0)
                continue;

            x = j->atual.x + k;
            y = j->atual.y + i;

            j->tabuleiro[y][x] =
                j->atual.tipo + 1;
        }
    }
}

void queda_rapida(Jogo *j) {
    while (pode_mover(j, 0, 1, j->atual.matriz)) {
        j->atual.y++;
    }
}
