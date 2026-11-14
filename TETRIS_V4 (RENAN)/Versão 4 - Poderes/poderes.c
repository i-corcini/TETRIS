#include "poderes.h"
#include "tabuleiro.h"
#include <string.h>
#include <math.h>

void lista_inicializar(ListaPoderes *l) {
    if (l) memset(l, 0, sizeof(*l));
}
int lista_inserir(ListaPoderes *l, PoderBloco p) {
    if (!l || l->quantidade == CAPACIDADE_PODERES) return 0;
    l->itens[l->quantidade++] = p;
    return 1;
}
int lista_buscar(const ListaPoderes *l, int x, int y) {
    if (!l) return -1;
    for (int i=0; i<l->quantidade; i++)
        if (l->itens[i].x==x && l->itens[i].y==y) return i;
    return -1;
}
int lista_obter(const ListaPoderes *l, int indice, PoderBloco *saida) {
    if (!l || !saida || indice<0 || indice>=l->quantidade) return 0;
    *saida=l->itens[indice];
    return 1;
}
int lista_remover(ListaPoderes *l, int indice) {
    if (!l || indice<0 || indice>=l->quantidade) return 0;
    if (indice<l->quantidade-1)
        memmove(&l->itens[indice], &l->itens[indice+1],
                (size_t)(l->quantidade-indice-1)*sizeof(l->itens[0]));
    l->quantidade--;
    return 1;
}

const char *poder_nome(TipoPoder tipo) {
    switch(tipo) {
        case PODER_GELO: return "Gelo";
        case PODER_RAIO: return "Raio";
        case PODER_BONUS: return "Bonus";
        default: return "Nenhum";
    }
}

void poderes_preparar_peca(Jogo *j) {
    j->atual.poder_tipo = PODER_NENHUM;
    j->atual.poder_segundos = 0;
    /* A primeira peca sempre tem um poder. Depois, uma em cada tres. */
    if (j->pecas_geradas != 1 && j->pecas_geradas % 3 != 1) return;
    int rodada = (int)((j->pecas_geradas-1)/3);
    j->atual.poder_tipo = (TipoPoder)(1 + rodada % 3);
    j->atual.poder_segundos = TEMPO_PODER_SEGUNDOS;
    int escolhido = rodada % 4, encontrados = 0;
    for (int y=0; y<TAM; y++) for (int x=0; x<TAM; x++) {
        if (j->atual.matriz[y][x]) {
            if (encontrados++ == escolhido) { j->atual.matriz[y][x]=2; return; }
        }
    }
}

void poderes_fixar_peca(Jogo *j) {
    if (j->atual.poder_tipo==PODER_NENHUM || j->atual.poder_segundos<=0) return;
    for (int y=0; y<TAM; y++) for (int x=0; x<TAM; x++)
        if(j->atual.matriz[y][x]==2) {
            PoderBloco p={j->atual.x+x, j->atual.y+y,
                          j->atual.poder_tipo, j->atual.poder_segundos};
            lista_inserir(&j->poderes,p);
            return;
        }
}

void poderes_linha_removida(Jogo *j, int linha) {
    for(int i=j->poderes.quantidade-1;i>=0;i--) {
        PoderBloco *p=&j->poderes.itens[i];
        if(p->y==linha) lista_remover(&j->poderes,i);
        else if(p->y<linha) p->y++;
    }
}

void poderes_atualizar(Jogo *j, double dt) {
    if(!j || dt<=0) return;
    if(j->congelamento_segundos>0) {
        j->congelamento_segundos-=dt;
        if(j->congelamento_segundos<0)j->congelamento_segundos=0;
    }
    if(j->aviso_segundos>0) {
        j->aviso_segundos-=dt;
        if(j->aviso_segundos<0)j->aviso_segundos=0;
    }
    if(j->atual.poder_tipo!=PODER_NENHUM) {
        j->atual.poder_segundos-=dt;
        if(j->atual.poder_segundos<=0) {
            j->atual.poder_tipo=PODER_NENHUM;
            j->atual.poder_segundos=0;
            for(int y=0;y<TAM;y++)for(int x=0;x<TAM;x++)
                if(j->atual.matriz[y][x]==2) j->atual.matriz[y][x]=1;
            j->poderes_expirados++;
        }
    }
    for(int i=j->poderes.quantidade-1;i>=0;i--) {
        j->poderes.itens[i].segundos-=dt;
        if(j->poderes.itens[i].segundos<=0) {
            lista_remover(&j->poderes,i);
            j->poderes_expirados++;
        }
    }
}

static int contar(const Jogo *j,int x,int y,int dx,int dy) {
    int n=0;
    while(x>=0 && x<j->largura && y>=0 && y<j->altura && j->tabuleiro[y][x]) {
        ++n; x+=dx; y+=dy;
    }
    return n;
}
static int cinco_em_linha(const Jogo *j,const PoderBloco *p) {
    int h=contar(j,p->x,p->y,1,0)+contar(j,p->x-1,p->y,-1,0);
    int v=contar(j,p->x,p->y,0,1)+contar(j,p->x,p->y-1,0,-1);
    return h>=5 || v>=5;
}
static void ativar(Jogo *j, PoderBloco p) {
    j->ultimo_poder=p.tipo;
    j->aviso_segundos=3.0;
    j->poderes_ativados++;
    switch(p.tipo) {
        case PODER_GELO:
            j->congelamento_segundos+=6.0;
            if(j->congelamento_segundos>15.0)j->congelamento_segundos=15.0;
            break;
        case PODER_RAIO:
            remover_linha_do_tabuleiro(j,p.y);
            j->pontos+=150;
            break;
        case PODER_BONUS:
            j->pontos+=300;
            break;
        default: break;
    }
}

void poderes_verificar_ativacoes(Jogo *j) {
    /* Reavalia apos cada efeito porque o raio pode deslocar as linhas. */
    for (int tentativas=0;tentativas<CAPACIDADE_PODERES;tentativas++) {
        int indice=-1;
        for (int i=0; i<j->poderes.quantidade; i++) {
            if (cinco_em_linha(j,&j->poderes.itens[i])) { indice=i; break; }
        }
        if(indice<0) break;
        PoderBloco p=j->poderes.itens[indice];
        lista_remover(&j->poderes,indice);
        ativar(j,p);
    }
}
