#include "tetraminos.h"
#include "poderes.h"

static const int formatos[7][TAM][TAM] = {
    {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}}, /* I */
    {{1,1,0,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}}, /* O */
    {{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}, /* T */
    {{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}}, /* S */
    {{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}, /* Z */
    {{1,0,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}, /* L */
    {{0,0,1,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}  /* J */
};

void fila_inicializar(FilaPecas *f) {
    for (int i = 0; i < CAPACIDADE_FILA; i++) f->itens[i] = 0;
    f->inicio = 0;
    f->fim = 0;
    f->quantidade = 0;
}

int fila_inserir(FilaPecas *f, int tipo) {
    if (!f || tipo < 0 || tipo >= 7 || f->quantidade >= CAPACIDADE_FILA)
        return 0;
    f->itens[f->fim] = tipo;
    f->fim = (f->fim + 1) % CAPACIDADE_FILA;
    f->quantidade++;
    return 1;
}

int fila_remover(FilaPecas *f, int *tipo) {
    if (!f || !tipo || f->quantidade <= 0) return 0;
    *tipo = f->itens[f->inicio];
    f->inicio = (f->inicio + 1) % CAPACIDADE_FILA;
    f->quantidade--;
    return 1;
}

int fila_consultar(const FilaPecas *f, int *tipo) {
    return fila_elemento(f, 0, tipo);
}

int fila_elemento(const FilaPecas *f, int posicao, int *tipo) {
    if (!f || !tipo || posicao < 0 || posicao >= f->quantidade) return 0;
    *tipo = f->itens[(f->inicio + posicao) % CAPACIDADE_FILA];
    return 1;
}

/* Estado do gerador pertence a Jogo e e restaurado pela pilha. */
static int sortear_tipo(Jogo *j) {
    j->semente = j->semente * 1664525u + 1013904223u;
    return (int)((j->semente >> 16) % 7u);
}

void iniciar_fila_pecas(Jogo *j, unsigned int semente) {
    j->semente = semente ? semente : 1u;
    fila_inicializar(&j->proximas);
    for (int i = 0; i < CAPACIDADE_FILA; i++)
        fila_inserir(&j->proximas, sortear_tipo(j));
}

void nova_peca(Jogo *j) {
    int tipo;
    if (!fila_remover(&j->proximas, &tipo)) {
        j->fim = 1;
        return;
    }
    if (!fila_inserir(&j->proximas, sortear_tipo(j))) {
        j->fim = 1;
        return;
    }
    j->atual.tipo = tipo;
    j->atual.x = (j->largura - TAM) / 2;
    j->atual.y = 0;
    j->pecas_geradas++;
    for (int i = 0; i < TAM; i++)
        for (int k = 0; k < TAM; k++)
            j->atual.matriz[i][k] = formatos[tipo][i][k];
    poderes_preparar_peca(j);
}

int obter_formato(int tipo, int saida[TAM][TAM]) {
    if (tipo < 0 || tipo >= 7 || !saida) return 0;
    for (int y = 0; y < TAM; y++)
        for (int x = 0; x < TAM; x++)
            saida[y][x] = formatos[tipo][y][x];
    return 1;
}
