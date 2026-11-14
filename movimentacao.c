#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "tetraminos.h"
#include "tabuleiro.h"

int pode_mover(Jogo *j, int dx, int dy,
               int matriz[TAM][TAM]) {
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
