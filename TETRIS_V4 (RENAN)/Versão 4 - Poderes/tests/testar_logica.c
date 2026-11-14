#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "jogo.h"
#include "tabuleiro.h"
#include "tetraminos.h"
#include "movimentacao.h"
#include "gravidade.h"
#include "poderes.h"

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

/* Regressao: timers dos poderes e queda automatica NAO podem fazer
   o proximo U restaurar indefinidamente a mesma peca. */
static void testar_undo_com_tempo_e_gravidade(void) {
    Jogo j;
    PilhaEstados p;
    int alterado=1; /* checkpoint inicial da peca */
    inicializar_com_config(&j,40,10,DIFICULDADE_NORMAL);
    iniciar_fila_pecas(&j,77u);
    nova_peca(&j);
    pilha_inicializar(&p);
    assert(pilha_empilhar(&p,&j));
    Jogo primeira=j;

    /* Gravitacao automatica e passagem de tempo sao alteracoes do mundo,
       nao comandos que rearmam o primeiro desfazer da pilha. */
    assert(mover(&j,0,1));
    poderes_atualizar(&j,0.5);
    assert(pilha_desfazer_peca(&p,&j,&alterado));
    assert(j.atual.y==primeira.atual.y && !alterado);
    assert(pilha_quantidade(&p)==1);

    queda_rapida(&j);
    finalizar_peca(&j);
    assert(pilha_empilhar(&p,&j));
    Jogo segunda=j;
    alterado=1; /* novo checkpoint */
    assert(mover(&j,0,1));
    poderes_atualizar(&j,1.0);
    assert(pilha_desfazer_peca(&p,&j,&alterado));
    assert(j.atual.y==segunda.atual.y && !alterado);

    /* Mesmo depois de nova queda/tick, o SEGUNDO U volta a peca anterior. */
    assert(mover(&j,0,1));
    poderes_atualizar(&j,0.3);
    assert(pilha_desfazer_peca(&p,&j,&alterado));
    assert(pilha_quantidade(&p)==1);
    assert(j.atual.tipo==primeira.atual.tipo);
    assert(j.atual.x==primeira.atual.x && j.atual.y==primeira.atual.y);
}

static void testar_pontuacao(void) {
    Jogo j;
    inicializar(&j);
    for(int y=j.altura-4;y<j.altura;y++)
        for(int x=0;x<j.largura;x++)j.tabuleiro[y][x]=1;
    limpar_linhas(&j);
    assert(j.pontos==800 && j.linhas==4);
}

static void testar_poderes(void) {
    ListaPoderes l;
    lista_inicializar(&l);
    PoderBloco p1={1,2,PODER_GELO,60}, p2={3,4,PODER_RAIO,35}, out;
    assert(lista_inserir(&l,p1) && lista_inserir(&l,p2));
    assert(lista_buscar(&l,3,4)==1);
    assert(lista_obter(&l,1,&out) && out.segundos==35);
    assert(lista_remover(&l,0) && lista_buscar(&l,3,4)==0);
    assert(!lista_remover(&l,5));

    Jogo j;
    inicializar_com_config(&j,40,10,DIFICULDADE_NORMAL);
    iniciar_fila_pecas(&j,48);
    nova_peca(&j);
    assert(j.atual.poder_tipo==PODER_GELO);
    int marcadores=0;
    for(int y=0;y<TAM;y++)for(int x=0;x<TAM;x++)
        if(j.atual.matriz[y][x]==2)marcadores++;
    assert(marcadores==1 && j.atual.poder_segundos==60);
    poderes_atualizar(&j,50.3);
    assert(j.atual.poder_segundos>9 && j.atual.poder_segundos<10);
    poderes_atualizar(&j,10);
    assert(j.atual.poder_tipo==PODER_NENHUM && j.poderes_expirados==1);
    marcadores=0;
    for(int y=0;y<TAM;y++)for(int x=0;x<TAM;x++)
        if(j.atual.matriz[y][x]==2)marcadores++;
    assert(marcadores==0); /* O bloco continua; somente o poder some. */

    inicializar_com_config(&j,40,10,DIFICULDADE_NORMAL);
    for(int x=1;x<=5;x++)j.tabuleiro[25][x]=1;
    assert(lista_inserir(&j.poderes,(PoderBloco){3,25,PODER_GELO,60}));
    poderes_verificar_ativacoes(&j);
    assert(j.poderes.quantidade==0 && j.poderes_ativados==1);
    assert(j.congelamento_segundos==6);
    assert(j.tabuleiro[25][1]==1);
    poderes_atualizar(&j,2.5);
    assert(j.congelamento_segundos==3.5);
    poderes_atualizar(&j,20);
    assert(j.congelamento_segundos==0);

    inicializar_com_config(&j,40,10,DIFICULDADE_NORMAL);
    for(int y=20;y<25;y++)j.tabuleiro[y][4]=1;
    assert(lista_inserir(&j.poderes,(PoderBloco){4,22,PODER_RAIO,55}));
    poderes_verificar_ativacoes(&j);
    assert(j.pontos==150 && j.poderes_ativados==1);
    assert(j.poderes.quantidade==0);
    assert(j.tabuleiro[22][4]==1 && j.tabuleiro[20][4]==0);

    inicializar_com_config(&j,40,10,DIFICULDADE_NORMAL);
    for(int x=0;x<5;x++)j.tabuleiro[31][x]=1;
    assert(lista_inserir(&j.poderes,(PoderBloco){2,31,PODER_BONUS,30}));
    poderes_verificar_ativacoes(&j);
    assert(j.pontos==300 && j.poderes_ativados==1);
    assert(j.poderes.quantidade==0);

    inicializar_com_config(&j,40,10,DIFICULDADE_NORMAL);
    j.tabuleiro[20][2]=1;
    j.tabuleiro[22][2]=1;
    assert(lista_inserir(&j.poderes,(PoderBloco){2,20,PODER_GELO,9}));
    assert(lista_inserir(&j.poderes,(PoderBloco){2,22,PODER_RAIO,10}));
    remover_linha_do_tabuleiro(&j,22);
    assert(j.poderes.quantidade==1 && j.poderes.itens[0].y==21);
    poderes_atualizar(&j,9.1);
    assert(j.poderes.quantidade==0 && j.poderes_expirados==1);
    assert(j.tabuleiro[21][2]==1); /* O timer nao apaga o bloco. */

    inicializar_com_config(&j,40,10,DIFICULDADE_NORMAL);
    iniciar_fila_pecas(&j,123);
    nova_peca(&j);
    queda_rapida(&j);
    finalizar_peca(&j);
    assert(j.poderes.quantidade==1); /* Primeiro poder caiu e foi fixado. */
    PilhaEstados historico; int modificado=1;
    pilha_inicializar(&historico);
    pilha_empilhar(&historico,&j);
    Jogo salvo=j;
    poderes_atualizar(&j,5);
    assert(pilha_desfazer_peca(&historico,&j,&modificado));
    assert(memcmp(&j,&salvo,sizeof(Jogo))==0);
}

int main(void) {
    testar_fila();
    testar_tamanhos();
    testar_gravidade();
    testar_pilha_por_peca();
    testar_undo_com_tempo_e_gravidade();
    testar_pontuacao();
    testar_poderes();
    puts("OK: fila, pilha, lista de poderes, temporizadores, ativacao horizontal/vertical, raio, gelo, bonus e undo.");
    return 0;
}
