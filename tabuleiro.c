#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "tabuleiro.h"
#include "tetraminos.h"

void limpar_linhas(Jogo *j) {
    int i, k, linha;
    int completas = 0;

    for (i = ALTURA - 1; i >= 0; i--) {
        linha = 1;

        for (k = 0; k < LARGURA; k++) {
            if (j->tabuleiro[i][k] == 0) {
                linha = 0;
                break;
            }
        }

        if (linha) {
            completas++;

            for (int y = i; y > 0; y--) {
                for (k = 0; k < LARGURA; k++) {
                    j->tabuleiro[y][k] =
                        j->tabuleiro[y - 1][k];
                }
            }

            for (k = 0; k < LARGURA; k++) {
                j->tabuleiro[0][k] = 0;
            }

            i++;
        }
    }

    j->linhas += completas;

    if (completas == 1)
        j->pontos += 100;
    else if (completas == 2)
        j->pontos += 300;
    else if (completas == 3)
        j->pontos += 500;
    else if (completas == 4)
        j->pontos += 800;
}

void desenhar(Jogo *j) {
    int i, k;
    int ocupado;

    printf("\n");

    for (i = 0; i < ALTURA; i++) {
        printf("|");

        for (k = 0; k < LARGURA; k++) {

            ocupado = j->tabuleiro[i][k];

            int px = k - j->atual.x;
            int py = i - j->atual.y;

            if (px >= 0 && px < TAM &&
                py >= 0 && py < TAM &&
                j->atual.matriz[py][px]) {
                ocupado = 1;
            }

            if (ocupado)
                printf("[]");
            else
                printf(" .");
        }

        printf(" |\n");
    }

    printf("----------------------\n");
    printf("Pontos: %d\n", j->pontos);
    printf("Linhas: %d\n", j->linhas);
}

