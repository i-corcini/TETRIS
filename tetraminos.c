//igor
#include "tetraminos.h"

typedef struct {
    int matriz[TAM][TAM];
    int x;
    int y;
    int tipo;
} Tetramino;

const int formatos[7][TAM][TAM] = {
    {
        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {
        {1, 1, 0, 0},
        {1, 1, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {
        {0, 1, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {
        {0, 1, 1, 0},
        {1, 1, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {
        {1, 1, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {
        {1, 0, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {
        {0, 0, 1, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    }
};

void nova_peca(Jogo *j) {
    int i, k;

    j->atual.tipo = rand() % 7;
    j->atual.x = 3;
    j->atual.y = 0;

    for (i = 0; i < TAM; i++) {
        for (k = 0; k < TAM; k++) {
            j->atual.matriz[i][k] =
                formatos[j->atual.tipo][i][k];
        }
    }
}
