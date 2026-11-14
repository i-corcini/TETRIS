#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "jogo.h"
#include "efeitos.h"
#include "tabuleiro.h"

static void teste_linha_limpa(void) {
    EfeitosVisuais fx;
    Jogo antes,depois;
    inicializar_com_config(&antes,20,8,DIFICULDADE_NORMAL);
    antes.atual.x=0;
    antes.atual.y=19;
    antes.atual.tipo=2;
    antes.atual.matriz[0][0]=1;
    for(int x=1;x<8;x++) antes.tabuleiro[19][x]=x%7+1;
    depois=antes;
    depois.linhas=1;
    efeitos_inicializar(&fx);
    efeitos_ao_fixar(&fx,&antes,&depois);
    assert(fx.linhas_quantidade==1);
    assert(fx.linhas_y[0]==19);
    assert(fx.linhas_cores[0][0]==antes.atual.tipo+1);
    assert(fx.linhas_cores[0][7]==antes.tabuleiro[19][7]);
    assert(fx.linhas_tempo>0 && fx.impacto_tempo>0);
    efeitos_atualizar(&fx,.12);
    assert(fx.linhas_tempo>0);
    efeitos_atualizar(&fx,1.0);
    assert(fx.linhas_tempo==0 && fx.impacto_tempo==0);
}

static void teste_poder_e_desfazer(void) {
    Jogo antes,depois;
    EfeitosVisuais fx;
    inicializar_com_config(&antes,40,10,DIFICULDADE_NORMAL);
    antes.atual.x=3;
    antes.atual.y=25;
    antes.atual.matriz[1][2]=2;
    depois=antes;
    depois.poderes_ativados=1;
    depois.ultimo_poder=PODER_GELO;
    efeitos_inicializar(&fx);
    efeitos_ao_fixar(&fx,&antes,&depois);
    assert(fx.poder_tempo>0);
    assert(fx.poder_tipo==PODER_GELO);
    assert(fx.poder_x==5 && fx.poder_y==26);
    efeitos_atualizar(&fx,.3);
    assert(fx.poder_tempo>0);
    efeitos_desfazer(&fx,&antes);
    assert(fx.poder_tempo==0 && fx.linhas_tempo==0 && fx.impacto_tempo==0);
    assert(fx.desfazer_tempo>0);
    efeitos_atualizar(&fx,.4);
    assert(fx.desfazer_tempo==0);
}

static void teste_ausencia_de_falso_evento(void) {
    Jogo antes;
    EfeitosVisuais fx;
    inicializar_com_config(&antes,40,10,DIFICULDADE_NORMAL);
    efeitos_inicializar(&fx);
    efeitos_ao_fixar(&fx,&antes,&antes);
    assert(fx.linhas_quantidade==0);
    assert(fx.linhas_tempo==0 && fx.poder_tempo==0);
}

int main(void) {
    teste_linha_limpa();
    teste_poder_e_desfazer();
    teste_ausencia_de_falso_evento();
    puts("OK: animacao de linha, poder, desfazer e temporizadores.");
    return 0;
}
