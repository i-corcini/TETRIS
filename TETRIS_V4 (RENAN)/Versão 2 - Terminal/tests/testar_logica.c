#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "jogo.h"
#include "tabuleiro.h"
#include "tetraminos.h"
#include "movimentacao.h"
#include "gravidade.h"

static void testar_fila(void) {
    FilaPecas f;
    int tipo = -1;
    fila_inicializar(&f);
    assert(f.quantidade == 0 && !fila_remover(&f, &tipo));
    for (int i = 0; i < CAPACIDADE_FILA; i++) assert(fila_inserir(&f, i));
    assert(!fila_inserir(&f, 6));
    assert(fila_consultar(&f, &tipo) && tipo == 0);
    assert(fila_remover(&f, &tipo) && tipo == 0);
    assert(fila_remover(&f, &tipo) && tipo == 1);
    assert(fila_inserir(&f, 5));
    assert(fila_inserir(&f, 6));
    for (int i = 2; i <= 6; i++) assert(fila_remover(&f, &tipo) && tipo == i);
    assert(!fila_remover(&f, &tipo));
}

static void testar_pilha(void) {
    PilhaEstados p;
    Jogo jogo;
    inicializar(&jogo);
    pilha_inicializar(&p);
    assert(!pilha_desempilhar(&p, &jogo));
    for (int i = 0; i < CAPACIDADE_PILHA + 3; i++) {
        jogo.pontos = i;
        assert(pilha_empilhar(&p, &jogo));
    }
    assert(pilha_quantidade(&p) == CAPACIDADE_PILHA);
    for (int i = CAPACIDADE_PILHA + 2; i >= 3; i--) {
        assert(pilha_desempilhar(&p, &jogo));
        assert(jogo.pontos == i);
    }
    assert(!pilha_desempilhar(&p, &jogo));
}

static void testar_integracao(void) {
    Jogo jogo;
    PilhaEstados p;
    assert(ALTURA == 40 && LARGURA == 10);
    inicializar(&jogo);
    iniciar_fila_pecas(&jogo, 123u); /* Teste deterministico. */
    nova_peca(&jogo);
    assert(!jogo.fim && jogo.proximas.quantidade == CAPACIDADE_FILA);
    int proxima = -1;
    assert(fila_consultar(&jogo.proximas, &proxima));
    Jogo anterior = jogo;
    pilha_inicializar(&p);
    pilha_empilhar(&p, &jogo);
    queda_rapida(&jogo);
    finalizar_peca(&jogo);
    assert(jogo.atual.tipo == proxima);
    assert(jogo.proximas.quantidade == CAPACIDADE_FILA);
    assert(pilha_desempilhar(&p, &jogo));
    assert(memcmp(&jogo, &anterior, sizeof(Jogo)) == 0);
    assert(jogo.pontos == anterior.pontos);

    int antes_y = jogo.atual.y;
    aplicar_gravidade(&jogo);
    assert(jogo.atual.y == antes_y + 1);
    assert(nivel_atual(&jogo) == 1);

    for (int k = 0; k < LARGURA; k++) jogo.tabuleiro[ALTURA-1][k] = 1;
    limpar_linhas(&jogo);
    assert(jogo.linhas == 1 && jogo.pontos == 100);
    for (int y = ALTURA-4; y < ALTURA; y++)
        for (int k = 0; k < LARGURA; k++) jogo.tabuleiro[y][k] = 1;
    limpar_linhas(&jogo);
    assert(jogo.pontos == 900 && jogo.linhas == 5);
    jogo.linhas = 10;
    assert(nivel_atual(&jogo) == 2 && intervalo_gravidade(&jogo) < 0.80);
}


/* Garante o comportamento solicitado: o historico nao registra cada tecla. */
static void testar_desfazer_por_peca(void) {
    Jogo jogo;
    PilhaEstados p;
    int alterado = 0;
    inicializar(&jogo);
    iniciar_fila_pecas(&jogo, 123u);
    nova_peca(&jogo);
    pilha_inicializar(&p);
    assert(pilha_empilhar(&p, &jogo));
    Jogo primeira = jogo;
    assert(pilha_quantidade(&p) == 1);

    assert(mover(&jogo, 0, 5));
    alterado = 1;
    assert(pilha_desfazer_peca(&p, &jogo, &alterado));
    assert(memcmp(&jogo, &primeira, sizeof(Jogo)) == 0);
    assert(pilha_quantidade(&p) == 1 && !alterado);
    assert(!pilha_desfazer_peca(&p, &jogo, &alterado));

    int proxima;
    assert(fila_consultar(&jogo.proximas, &proxima));
    queda_rapida(&jogo);
    finalizar_peca(&jogo);
    assert(jogo.atual.tipo == proxima);
    assert(pilha_empilhar(&p, &jogo));
    Jogo segunda = jogo;
    assert(pilha_quantidade(&p) == 2);
    assert(mover(&jogo, 0, 4));
    alterado = 1;

    assert(pilha_desfazer_peca(&p, &jogo, &alterado));
    assert(memcmp(&jogo, &segunda, sizeof(Jogo)) == 0);
    assert(pilha_desfazer_peca(&p, &jogo, &alterado));
    assert(memcmp(&jogo, &primeira, sizeof(Jogo)) == 0);
    assert(pilha_quantidade(&p) == 1);

    jogo.pontos = 800;
    jogo.proximas.itens[0] = 5;
    alterado = 1;
    assert(pilha_desfazer_peca(&p, &jogo, &alterado));
    assert(jogo.pontos == primeira.pontos);
    assert(memcmp(&jogo.proximas, &primeira.proximas, sizeof(FilaPecas)) == 0);
    assert(jogo.semente == primeira.semente);
}

static void testar_tempo_gravidade(void) {
    Jogo jogo;
    EstadoGravidade g;
    inicializar(&jogo);
    iniciar_fila_pecas(&jogo, 456u);
    nova_peca(&jogo);
    gravidade_reiniciar(&g);

    int y = jogo.atual.y;
    for (int i = 0; i < 11; i++)
        assert(gravidade_atualizar(&jogo, &g, 0.05) == GRAVIDADE_SEM_MUDANCA);
    assert(jogo.atual.y == y);
    assert(gravidade_atualizar(&jogo, &g, 0.05) == GRAVIDADE_MOVEU);
    assert(jogo.atual.y == y + 1);

    queda_rapida(&jogo);
    assert(!pode_mover(&jogo, 0, 1, jogo.atual.matriz));
    int tipo = -1;
    assert(fila_consultar(&jogo.proximas, &tipo));
    gravidade_reiniciar(&g);
    for (int i = 0; i < 3; i++)
        assert(gravidade_atualizar(&jogo, &g, 0.05) == GRAVIDADE_SEM_MUDANCA);
    assert(gravidade_atualizar(&jogo, &g, 0.05) == GRAVIDADE_FIXOU);
    assert(jogo.atual.tipo == tipo);
    assert(jogo.atual.y == 0);
    assert(g.tempo_queda == 0.0 && g.tempo_contato == 0.0);
}

int main(void) {
    testar_fila();
    testar_pilha();
    testar_integracao();
    testar_desfazer_por_peca();
    testar_tempo_gravidade();
    puts("OK: fila FIFO, pilha por peca, desfazer, gravidade, fixacao 0.17s e tabuleiro 40x10.");
    return 0;
}
