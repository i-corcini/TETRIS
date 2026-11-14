#include <string.h>
#include <time.h>
#include "tabuleiro.h"
#include "tetraminos.h"
#include "movimentacao.h"
#include "pontuacao.h"

void inicializar_com_config(Jogo *j, int altura, int largura, int dificuldade) {
    memset(j, 0, sizeof(*j));
    if (altura < MIN_ALTURA) altura = MIN_ALTURA;
    if (altura > MAX_ALTURA) altura = MAX_ALTURA;
    if (largura < MIN_LARGURA) largura = MIN_LARGURA;
    if (largura > MAX_LARGURA) largura = MAX_LARGURA;
    if (dificuldade < DIFICULDADE_FACIL || dificuldade > DIFICULDADE_DIFICIL)
        dificuldade = DIFICULDADE_NORMAL;
    j->altura = altura;
    j->largura = largura;
    j->dificuldade = dificuldade;
    inicializar_pontuacao(j);
    iniciar_fila_pecas(j, (unsigned int)time(NULL));
}

void inicializar(Jogo *j) {
    inicializar_com_config(j, ALTURA_PADRAO, LARGURA_PADRAO, DIFICULDADE_NORMAL);
}

void limpar_linhas(Jogo *j) {
    int completas = 0;
    for (int i = j->altura - 1; i >= 0; i--) {
        int cheia = 1;
        for (int x = 0; x < j->largura; x++)
            if (!j->tabuleiro[i][x]) { cheia = 0; break; }
        if (!cheia) continue;
        completas++;
        for (int y = i; y > 0; y--)
            memcpy(j->tabuleiro[y], j->tabuleiro[y-1], sizeof(j->tabuleiro[y]));
        memset(j->tabuleiro[0], 0, sizeof(j->tabuleiro[0]));
        i++;
    }
    atualizar_pontuacao(j, completas);
}

void finalizar_peca(Jogo *j) {
    if (j->fim) return;
    fixar_peca(j);
    limpar_linhas(j);
    nova_peca(j);
    if (!j->fim && !pode_mover(j, 0, 0, j->atual.matriz)) j->fim = 1;
}
