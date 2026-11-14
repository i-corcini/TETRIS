#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "gravidade.h"
#include "tetraminos.h"
#include "movimentacao.h"
#include "tabuleiro.h"
#include "pontuacao.h"
#include "window.h"

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
