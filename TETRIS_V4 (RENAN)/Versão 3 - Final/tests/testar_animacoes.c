#include <assert.h>
#include <stdio.h>
#include "jogo.h"
#include "efeitos.h"
#include "tabuleiro.h"

int main(void) {
    Jogo anterior,posterior;
    EfeitosVisuais fx;
    inicializar_com_config(&anterior,20,8,DIFICULDADE_NORMAL);
    anterior.atual.tipo=2;
    anterior.atual.x=0;
    anterior.atual.y=19;
    anterior.atual.matriz[0][0]=1;
    for(int x=1;x<8;x++) anterior.tabuleiro[19][x]=x%7+1;
    posterior=anterior; posterior.linhas++;
    efeitos_inicializar(&fx);
    efeitos_ao_fixar(&fx,&anterior,&posterior);
    assert(fx.linhas_quantidade==1 && fx.linhas_y[0]==19);
    assert(fx.linhas_tempo>0 && fx.impacto_tempo>0);
    efeitos_atualizar(&fx,1.0);
    assert(fx.linhas_tempo==0 && fx.impacto_tempo==0);
    efeitos_desfazer(&fx,&anterior);
    assert(fx.desfazer_tempo>0);
    efeitos_atualizar(&fx,1.0);
    assert(fx.desfazer_tempo==0);
    puts("OK: animacoes de linhas, impacto e desfazer da V3 Final.");
    return 0;
}
