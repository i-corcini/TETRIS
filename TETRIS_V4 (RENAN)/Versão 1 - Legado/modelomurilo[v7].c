#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ALTURA 20
#define LARGURA 10
#define TAM 4

typedef struct {
    int matriz[TAM][TAM];
    int x;
    int y;
    int tipo;
} Peca;

typedef struct {
    int tabuleiro[ALTURA][LARGURA];
    Peca atual;
    int pontos;
    int linhas;
    int fim;
} Jogo;

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

void inicializar(Jogo *j) {
    int i, k;

    for (i = 0; i < ALTURA; i++) {
        for (k = 0; k < LARGURA; k++) {
            j->tabuleiro[i][k] = 0;
        }
    }

    j->pontos = 0;
    j->linhas = 0;
    j->fim = 0;
}

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

void queda_rapida(Jogo *j) {
    while (pode_mover(j, 0, 1, j->atual.matriz)) {
        j->atual.y++;
    }
}

void finalizar_peca(Jogo *j) {
    fixar_peca(j);
    limpar_linhas(j);

    nova_peca(j);

    if (!pode_mover(j, 0, 0, j->atual.matriz)) {
        j->fim = 1;
    }
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

int main(void) {
    Jogo jogo;
    char comando[20];

    srand(time(NULL));

    inicializar(&jogo);
    nova_peca(&jogo);

    while (!jogo.fim) {
        desenhar(&jogo);

        printf("\nA: esquerda | D: direita");
        printf("\nS: descer | W: girar");
        printf("\nEspaco: queda rapida | Q: sair");
        printf("\nComando: ");

        if (fgets(comando, sizeof(comando), stdin) == NULL)
            break;

        if (comando[0] == 'q' || comando[0] == 'Q')
            break;

        if (comando[0] == 'a' || comando[0] == 'A')
            mover(&jogo, -1, 0);

        else if (comando[0] == 'd' || comando[0] == 'D')
            mover(&jogo, 1, 0);

        else if (comando[0] == 'w' || comando[0] == 'W')
            rotacionar(&jogo);

        else if (comando[0] == 's' || comando[0] == 'S') {
            if (pode_mover(&jogo, 0, 1,
                           jogo.atual.matriz)) {
                mover(&jogo, 0, 1);
            } else {
                finalizar_peca(&jogo);
            }
        }

        else if (comando[0] == ' ') {
            queda_rapida(&jogo);
            finalizar_peca(&jogo);
        }

        if (!jogo.fim &&
            (comando[0] == 'a' || comando[0] == 'A' ||
             comando[0] == 'd' || comando[0] == 'D' ||
             comando[0] == 'w' || comando[0] == 'W')) {
        }
    }

    desenhar(&jogo);

    if (jogo.fim)
        printf("\nGAME OVER!\n");

    printf("Pontuacao final: %d\n", jogo.pontos);
    printf("Linhas eliminadas: %d\n", jogo.linhas);

    return 0;
}
