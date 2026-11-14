#include "movimentacao.h"
#include "poderes.h"
#include <string.h>

int pode_mover(const Jogo *j, int dx, int dy, const int matriz[TAM][TAM]) {
    for (int i = 0; i < TAM; i++) {
        for (int k = 0; k < TAM; k++) {
            if (matriz[i][k] == 0) continue;
            int nx = j->atual.x + k + dx;
            int ny = j->atual.y + i + dy;
            if (nx < 0 || nx >= j->largura || ny < 0 || ny >= j->altura)
                return 0;
            if (j->tabuleiro[ny][nx]) return 0;
        }
    }
    return 1;
}

int mover(Jogo *j, int dx, int dy) {
    if (!pode_mover(j, dx, dy, j->atual.matriz)) return 0;
    j->atual.x += dx;
    j->atual.y += dy;
    return 1;
}

int rotacionar_sentido(Jogo *j, int sentido) {
    int nova[TAM][TAM] = {{0}};
    for (int i = 0; i < TAM; i++)
        for (int k = 0; k < TAM; k++) {
            if (sentido >= 0) nova[k][TAM - 1 - i] = j->atual.matriz[i][k];
            else nova[TAM - 1 - k][i] = j->atual.matriz[i][k];
        }
    if (!pode_mover(j, 0, 0, nova)) return 0;
    memcpy(j->atual.matriz, nova, sizeof(nova));
    return 1;
}

int rotacionar(Jogo *j) { return rotacionar_sentido(j, 1); }

void fixar_peca(Jogo *j) {
    for (int i = 0; i < TAM; i++)
        for (int k = 0; k < TAM; k++)
            if (j->atual.matriz[i][k]) {
                int x = j->atual.x + k;
                int y = j->atual.y + i;
                j->tabuleiro[y][x] = j->atual.tipo + 1;
            }
    poderes_fixar_peca(j);
}

void queda_rapida(Jogo *j) {
    while (mover(j, 0, 1)) { /* Cai ate encostar. */ }
}

void pilha_inicializar(PilhaEstados *p) { if (p) p->topo = -1; }

int pilha_empilhar(PilhaEstados *p, const Jogo *j) {
    if (!p || !j) return 0;
    if (p->topo == CAPACIDADE_PILHA - 1) {
        /* Mantem os 32 estados mais recentes ao atingir a capacidade. */
        memmove(&p->estados[0], &p->estados[1],
                (CAPACIDADE_PILHA - 1) * sizeof(p->estados[0]));
        p->topo--;
    }
    p->estados[++p->topo] = *j;
    return 1;
}

int pilha_desempilhar(PilhaEstados *p, Jogo *j) {
    if (!p || !j || p->topo < 0) return 0;
    *j = p->estados[p->topo--];
    return 1;
}

int pilha_quantidade(const PilhaEstados *p) {
    return p ? p->topo + 1 : 0;
}

int pilha_consultar(const PilhaEstados *p, Jogo *j) {
    if (!p || !j || p->topo < 0) return 0;
    *j = p->estados[p->topo];
    return 1;
}

/* Desfaz por peca: primeiro restaura o topo, depois volta aos checkpoints anteriores. */
int pilha_desfazer_peca(PilhaEstados *p, Jogo *j, int *peca_modificada) {
    if (!p || !j || !peca_modificada || p->topo < 0) return 0;
    if (*peca_modificada) {
        *peca_modificada = 0;
        return pilha_consultar(p, j);
    }
    if (p->topo == 0) return 0;
    p->topo--;
    *peca_modificada = 0;
    return pilha_consultar(p, j);
}
