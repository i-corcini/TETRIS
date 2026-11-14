#include "efeitos.h"
#include <string.h>

#define DURACAO_LINHA 0.44
#define DURACAO_IMPACTO 0.24
#define DURACAO_NASCER 0.32
#define DURACAO_DESFAZER 0.28

void efeitos_inicializar(EfeitosVisuais *fx) {
    if(fx) memset(fx,0,sizeof(*fx));
}

void efeitos_nova_peca(EfeitosVisuais *fx, const Jogo *j) {
    if(!fx || !j || j->fim) return;
    fx->nascer_tempo=DURACAO_NASCER;
    fx->nascer_x=j->atual.x+1;
    fx->nascer_y=j->atual.y+1;
}

/* Reconstrucao leve do tabuleiro imediatamente ANTES da limpeza, usada
   somente para animar a linha real que foi completada. */
static void capturar_linhas(EfeitosVisuais *fx,const Jogo *j,int limite) {
    int copia[MAX_ALTURA][MAX_LARGURA];
    memcpy(copia,j->tabuleiro,sizeof(copia));
    for(int y=0;y<TAM;y++) for(int x=0;x<TAM;x++) {
        if(!j->atual.matriz[y][x]) continue;
        int coluna=j->atual.x+x, linha=j->atual.y+y;
        if(coluna>=0 && coluna<j->largura && linha>=0 && linha<j->altura)
            copia[linha][coluna]=j->atual.tipo+1;
    }
    fx->linhas_quantidade=0;
    /* Linhas eliminadas aparecem na ordem de baixo para cima. */
    for(int linha=j->altura-1;linha>=0 && fx->linhas_quantidade<limite;linha--) {
        int cheia=1;
        for(int x=0;x<j->largura;x++) {
            if(copia[linha][x]==0) { cheia=0;break; }
        }
        if(!cheia) continue;
        int destino=fx->linhas_quantidade++;
        fx->linhas_y[destino]=linha;
        memcpy(fx->linhas_cores[destino],copia[linha],sizeof(copia[linha]));
    }
    fx->linhas_tempo=fx->linhas_quantidade?DURACAO_LINHA:0;
}

void efeitos_ao_fixar(EfeitosVisuais *fx,const Jogo *antes,const Jogo *depois) {
    if(!fx || !antes || !depois) return;
    fx->impacto_tempo=DURACAO_IMPACTO;
    fx->impacto_x=antes->atual.x+1;
    fx->impacto_y=antes->atual.y+3;
    if(fx->impacto_x<0) fx->impacto_x=0;
    if(fx->impacto_y>=antes->altura) fx->impacto_y=antes->altura-1;

    int eliminadas=depois->linhas-antes->linhas;
    if(eliminadas>0) {
        if(eliminadas>EFEITO_MAX_LINHAS) eliminadas=EFEITO_MAX_LINHAS;
        capturar_linhas(fx,antes,eliminadas);
    }

    efeitos_nova_peca(fx,depois);
}

void efeitos_desfazer(EfeitosVisuais *fx,const Jogo *j) {
    if(!fx)return;
    efeitos_inicializar(fx);
    fx->desfazer_tempo=DURACAO_DESFAZER;
    efeitos_nova_peca(fx,j);
}

void efeitos_atualizar(EfeitosVisuais *fx,double dt) {
    if(!fx || dt<=0) return;
    double *t[]={&fx->linhas_tempo,&fx->impacto_tempo,
                 &fx->nascer_tempo,&fx->desfazer_tempo};
    for(unsigned i=0;i<sizeof(t)/sizeof(t[0]);i++) {
        if(*t[i]>0) {
            *t[i]-=dt;
            if(*t[i]<0) *t[i]=0;
        }
    }
}
