#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "jogo.h"
#include "tabuleiro.h"
#include "tetraminos.h"
#include "movimentacao.h"
#include "gravidade.h"

static void testar_fila(void) {
    FilaPecas fila;
    int tipo=-1;
    fila_inicializar(&fila);
    assert(fila.quantidade==0 && !fila_remover(&fila,&tipo));
    for(int i=0;i<CAPACIDADE_FILA;i++) assert(fila_inserir(&fila,i));
    assert(!fila_inserir(&fila,6));
    assert(fila_consultar(&fila,&tipo) && tipo==0);
    assert(fila_remover(&fila,&tipo) && tipo==0);
    assert(fila_remover(&fila,&tipo) && tipo==1);
    assert(fila_inserir(&fila,5) && fila_inserir(&fila,6));
    for(int i=2;i<=6;i++) assert(fila_remover(&fila,&tipo) && tipo==i);
    assert(!fila_remover(&fila,&tipo));
}

static void testar_tamanhos(void) {
    Jogo j;
    inicializar(&j);
    assert(j.altura==40 && j.largura==10);
    inicializar_com_config(&j,20,8,DIFICULDADE_FACIL);
    assert(j.altura==20 && j.largura==8 && j.dificuldade==DIFICULDADE_FACIL);
    nova_peca(&j);
    assert(j.atual.x==2 && pode_mover(&j,0,0,j.atual.matriz));
    for(int i=0;i<j.largura;i++) j.tabuleiro[j.altura-1][i]=1;
    limpar_linhas(&j);
    assert(j.pontos==100 && j.linhas==1);
    inicializar_com_config(&j,25,16,DIFICULDADE_DIFICIL);
    nova_peca(&j);
    assert(j.altura==25 && j.largura==16 && j.atual.x==6);
    assert(pode_mover(&j,0,0,j.atual.matriz));
    assert(!pode_mover(&j,-100,0,j.atual.matriz));
    assert(!pode_mover(&j,100,0,j.atual.matriz));
    inicializar_com_config(&j,0,200,100);
    assert(j.altura==MIN_ALTURA && j.largura==MAX_LARGURA);
    assert(j.dificuldade==DIFICULDADE_NORMAL);
}

static void testar_gravidade(void) {
    Jogo j;
    inicializar_com_config(&j,40,10,DIFICULDADE_FACIL);
    nova_peca(&j);
    double facil=intervalo_gravidade(&j);
    j.dificuldade=DIFICULDADE_NORMAL;
    double normal=intervalo_gravidade(&j);
    j.dificuldade=DIFICULDADE_DIFICIL;
    double dificil=intervalo_gravidade(&j);
    assert(facil>normal && normal>dificil);
    int y=j.atual.y;
    aplicar_gravidade(&j);
    assert(j.atual.y==y+1);
    j.linhas=10;
    assert(nivel_atual(&j)==2 && intervalo_gravidade(&j)<dificil);
}

static void testar_pilha_por_peca(void) {
    Jogo j;
    PilhaEstados p;
    int modificado=0;
    inicializar_com_config(&j,40,10,DIFICULDADE_NORMAL);
    iniciar_fila_pecas(&j,123u);
    nova_peca(&j);
    pilha_inicializar(&p);
    pilha_empilhar(&p,&j);
    Jogo primeira=j;
    assert(pilha_quantidade(&p)==1);

    /* Primeira vez: restaura a mesma peca no teto. */
    assert(mover(&j,0,5));
    modificado=1;
    assert(pilha_desfazer_peca(&p,&j,&modificado));
    assert(memcmp(&j,&primeira,sizeof(j))==0);
    assert(pilha_quantidade(&p)==1 && !modificado);
    assert(!pilha_desfazer_peca(&p,&j,&modificado)); /* Nao ha anterior. */

    /* Trava a primeira peca e comeca uma nova. */
    int proximo=-1;
    assert(fila_consultar(&j.proximas,&proximo));
    queda_rapida(&j);
    finalizar_peca(&j);
    assert(j.atual.tipo==proximo);
    pilha_empilhar(&p,&j);
    Jogo segunda=j;
    assert(pilha_quantidade(&p)==2);
    assert(mover(&j,0,3));
    modificado=1;

    /* Primeiro desfazer restaura a segunda peca, depois volta para a primeira. */
    assert(pilha_desfazer_peca(&p,&j,&modificado));
    assert(memcmp(&j,&segunda,sizeof(j))==0);
    assert(pilha_desfazer_peca(&p,&j,&modificado));
    assert(memcmp(&j,&primeira,sizeof(j))==0);
    assert(pilha_quantidade(&p)==1);

    /* Estados incluem pontuacao, fila e semente. */
    j.pontos=800;
    j.proximas.itens[0]=5;
    modificado=1;
    assert(pilha_desfazer_peca(&p,&j,&modificado));
    assert(j.pontos==primeira.pontos && j.semente==primeira.semente);
    assert(memcmp(&j.proximas,&primeira.proximas,sizeof(FilaPecas))==0);

    /* Historico com limite: descarta o estado mais antigo, mantem LIFO. */
    pilha_inicializar(&p);
    for(int i=0;i<CAPACIDADE_PILHA+4;i++) {
        j.pontos=i;
        assert(pilha_empilhar(&p,&j));
    }
    assert(pilha_quantidade(&p)==CAPACIDADE_PILHA);
    for(int i=CAPACIDADE_PILHA+3;i>=4;i--) {
        assert(pilha_desempilhar(&p,&j));
        assert(j.pontos==i);
    }
    assert(!pilha_desempilhar(&p,&j));
}

static void testar_pontuacao(void) {
    Jogo j;
    inicializar(&j);
    for(int y=j.altura-4;y<j.altura;y++)
        for(int x=0;x<j.largura;x++)j.tabuleiro[y][x]=1;
    limpar_linhas(&j);
    assert(j.pontos==800 && j.linhas==4);
}

int main(void) {
    testar_fila();
    testar_tamanhos();
    testar_gravidade();
    testar_pilha_por_peca();
    testar_pontuacao();
    puts("OK: fila FIFO, pilha por peca, desfazer, tabuleiros dinamicos, gravidade, dificuldade e pontos.");
    return 0;
}
