#include <string.h>
#include <time.h>
#include "tabuleiro.h"
#include "tetraminos.h"
#include "movimentacao.h"
#include "pontuacao.h"

void inicializar(Jogo *j) {
    memset(j, 0, sizeof(*j));
    inicializar_pontuacao(j);
    iniciar_fila_pecas(j, (unsigned int)time(NULL));
}

void limpar_linhas(Jogo *j) {
    int completas = 0;
    for (int i = ALTURA - 1; i >= 0; i--) {
        int cheia = 1;
        for (int x = 0; x < LARGURA; x++)
            if (!j->tabuleiro[i][x]) { cheia = 0; break; }
        if (!cheia) continue;
        completas++;
        for (int y = i; y > 0; y--)
            memcpy(j->tabuleiro[y], j->tabuleiro[y - 1], sizeof(j->tabuleiro[y]));
        memset(j->tabuleiro[0], 0, sizeof(j->tabuleiro[0]));
        i++; /* Reavalia a linha, pois o conteudo de cima desceu. */
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

#include <stdio.h>
#include "gravidade.h"

void desenhar(const Jogo *j, const PilhaEstados *historico, int vista) {
    const char nomes[] = "IOTSZLJ";
    int inicio = 0, fim = ALTURA;
    /* Janelas FIXAS, sem camera que desloque o tabuleiro para cima.
     * V: todas as linhas -> metade superior -> metade inferior. */
    if (vista == 1) { inicio = 0; fim = ALTURA / 2; }
    if (vista == 2) { inicio = ALTURA / 2; fim = ALTURA; }
    printf("\033[H\033[2J");
    printf("TETRIS | V2 TERMINAL | TABULEIRO %dx%d\n", ALTURA, LARGURA);
    printf("Pontos: %d | Linhas: %d | Nivel: %d | Passo: %.2fs\n",
           j->pontos, j->linhas, nivel_atual(j), intervalo_gravidade(j));
    printf("Proximas pecas [FILA]: ");
    for (int i = 0; i < j->proximas.quantidade; i++) {
        int tipo;
        if (fila_elemento(&j->proximas, i, &tipo))
            printf("%s%c", i ? " > " : "", nomes[tipo]);
    }
    printf("\nHistorico [PILHA]: %d peca(s) guardada(s)\n",
           pilha_quantidade(historico));
    printf("Vista: %s | linhas %d-%d (tecla V para alternar)\n",
           vista == 0 ? "inteira" : vista == 1 ? "superior" : "inferior",
           inicio+1, fim);
    printf("    +--------------------+\n");
    for (int i = inicio; i < fim; i++) {
        printf("%02d  |", i + 1);
        for (int k = 0; k < LARGURA; k++) {
            int ocupado = j->tabuleiro[i][k];
            int px = k - j->atual.x;
            int py = i - j->atual.y;
            if (px >= 0 && px < TAM && py >= 0 && py < TAM &&
                j->atual.matriz[py][px]) ocupado = j->atual.tipo + 1;
            printf(ocupado ? "[]" : " .");
        }
        printf("|\n");
    }
    printf("    +--------------------+\n");
    if (j->fim) printf("FIM DE JOGO! U para desfazer ou R para reiniciar.\n");
    printf("A/D mover | W/E girar | S descer | Espaco queda rapida\n");
    printf("U desfazer peca | P pausa | V vista | R reiniciar | Q sair\n");
    printf("Sem Enter. Para ver o tabuleiro inteiro, amplie a altura do terminal.\n");
    fflush(stdout);
}
