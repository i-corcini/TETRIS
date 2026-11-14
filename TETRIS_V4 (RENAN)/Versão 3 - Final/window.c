#include "window.h"
#include "tetraminos.h"
#include "gravidade.h"
#include "movimentacao.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define JANELA_W 1100
#define JANELA_H 820

typedef struct { float x, y, w, h; } Ret;

static ALLEGRO_COLOR cor(int r, int g, int b) { return al_map_rgb(r,g,b); }
static ALLEGRO_COLOR alfa(int r,int g,int b,int a) { return al_map_rgba(r,g,b,a); }
static float menor(float a,float b) { return a < b ? a : b; }
static float maior(float a,float b) { return a > b ? a : b; }
static Ret ret(float x,float y,float w,float h) { Ret r={x,y,w,h}; return r; }
static int dentro(Ret r,int x,int y) { return x >= r.x && x < r.x+r.w && y >= r.y && y < r.y+r.h; }
static void arredondado(Ret r,float raio,ALLEGRO_COLOR c) {
    al_draw_filled_rounded_rectangle(r.x,r.y,r.x+r.w,r.y+r.h,raio,raio,c);
}
static void contorno(Ret r,float raio,ALLEGRO_COLOR c) {
    al_draw_rounded_rectangle(r.x,r.y,r.x+r.w,r.y+r.h,raio,raio,c,1.2f);
}
static ALLEGRO_COLOR cor_peca(int id) {
    switch (id) {
        case 1: return cor(69,203,216);  /* I */
        case 2: return cor(242,194,86);  /* O */
        case 3: return cor(177,138,244); /* T */
        case 4: return cor(111,203,143); /* S */
        case 5: return cor(239,116,119); /* Z */
        case 6: return cor(250,163,101); /* L */
        case 7: return cor(114,153,243); /* J */
        default:return cor(36,49,66);
    }
}
static const char *dificuldade_nome(int v) {
    if (v==DIFICULDADE_FACIL) return "Fácil";
    if (v==DIFICULDADE_DIFICIL) return "Difícil";
    return "Normal";
}
static void txt(ALLEGRO_FONT *f,ALLEGRO_COLOR c,float x,float y,const char *s) {
    al_draw_text(f,c,x,y,0,s);
}
static void txt_centro(ALLEGRO_FONT *f,ALLEGRO_COLOR c,float x,float y,const char *s) {
    al_draw_text(f,c,x,y,ALLEGRO_ALIGN_CENTER,s);
}
static void botao(const Janela *j, Ret r, const char *rotulo, int destaque, int selecionado) {
    int hover=dentro(r,j->mouse_x,j->mouse_y);
    ALLEGRO_COLOR fundo = destaque ? cor(226,159,112) : cor(34,47,62);
    ALLEGRO_COLOR texto = destaque ? cor(24,29,38) : cor(232,238,242);
    if (selecionado && !destaque) fundo=cor(36,111,108);
    if (hover) fundo=destaque?cor(244,180,133):(selecionado?cor(49,137,131):cor(51,69,83));
    arredondado(r,12,fundo);
    if (selecionado && !destaque) contorno(r,12,cor(111,213,189));
    txt_centro(j->texto,texto,r.x+r.w/2,r.y+(r.h-al_get_font_line_height(j->texto))/2,rotulo);
}
static void fundo(float w,float h) {
    al_clear_to_color(cor(17,26,37));
    /* Padrao discreto de pontos, mais organico que uma grade tecnica. */
    for (int y=24;y<h;y+=38)
        for (int x=25;x<w;x+=38) {
            int shift=(y/38)%2*9;
            al_draw_filled_circle(x+shift,y,1.1f,alfa(170,188,195,29));
        }
    al_draw_filled_circle(w*.88f,h*.15f,120,alfa(41,102,105,17));
    al_draw_filled_circle(w*.08f,h*.88f,145,alfa(181,116,89,13));
}
static void cabecalho(const Janela *j,float w,const char *subtitulo) {
    txt(j->titulo,cor(247,239,225),42,17,"Tetris");
    txt(j->pequena,cor(148,169,177),47,69,subtitulo);
    al_draw_line(40,99,w-40,99,alfa(179,194,198,48),1);
}
static void desenhar_peca_artistica(float x,float y,float tam,int tipo) {
    int mat[TAM][TAM];
    if(!obter_formato(tipo,mat)) return;
    for(int linha=0;linha<TAM;linha++)
        for(int col=0;col<TAM;col++) if(mat[linha][col]) {
            Ret r=ret(x+col*tam,y+linha*tam,tam-2,tam-2);
            arredondado(r,menor(5,tam*.25f),cor_peca(tipo+1));
            al_draw_line(r.x+3,r.y+3,r.x+r.w-3,r.y+3,alfa(255,255,255,105),1.1);
        }
}
static Ret menu_botao(int w,int h,int indice) {
    return ret(w*.09f,h*.46f + indice*75,330,57);
}
static Ret config_painel(int w,int h) {
    float pw=menor(w-64,800), ph=menor(h-145,608);
    return ret((w-pw)/2,(h-ph)/2+30,pw,ph);
}
static Ret config_dif(Ret p,int i) {
    float lacuna=12, largura=(p.w-72-2*lacuna)/3;
    return ret(p.x+36+i*(largura+lacuna),p.y+146,largura,64);
}
static Ret config_altura_menos(Ret p) {return ret(p.x+p.w-176,p.y+264,42,44);}
static Ret config_altura_mais (Ret p) {return ret(p.x+p.w-62,p.y+264,42,44);}
static Ret config_largura_menos(Ret p){return ret(p.x+p.w-176,p.y+331,42,44);}
static Ret config_largura_mais (Ret p){return ret(p.x+p.w-62,p.y+331,42,44);}
static Ret config_full(Ret p) {return ret(p.x+p.w-177,p.y+410,157,43);}
static Ret config_audio(Ret p,int indice) {
    float largura=(p.w-84)/2;
    return ret(p.x+36+indice*(largura+12),p.y+463,largura,42);
}
static Ret config_voltar(Ret p) {return ret(p.x+36,p.y+p.h-60,p.w-72,48);}
/* Controles de audio no cabecalho, sem sobrepor o tabuleiro. */
static Ret audio_topo(int w,int indice) {
    return ret(w-236+indice*118,27,108,40);
}
static void desenhar_audio_topo(const Janela *j,int w) {
    const SonsJogo *s=&j->sons;
    botao(j,audio_topo(w,0),s->musica_ligada?"Musica: ON":"Musica: OFF",0,s->musica_ligada);
    botao(j,audio_topo(w,1),s->efeitos_ligados?"Som: ON":"Som: OFF",0,s->efeitos_ligados);
}


typedef struct { Ret placa, tabuleiro, esquerda, direita; float bloco; } LayoutJogo;
static int linhas_visiveis(const Janela *janela, const Jogo *j) {
    return janela->modo_visual==0 || j->altura<=20 ? j->altura : 20;
}
static int primeira_linha(const Janela *janela, const Jogo *j) {
    return janela->modo_visual==2 && j->altura>20 ? j->altura-20 : 0;
}
static LayoutJogo layout_jogo(int w,int h,const Jogo *j, int linhas) {
    LayoutJogo l;
    /* Reserva espaco no alto para cabecalho e controles. */
    float disponivel_altura=maior(220,h-174);
    float disponivel_largura=maior(90,w*.35f);
    /* Celulas em pixels inteiros evitam divisorias 'quebradas' ou sobrepostas. */
    l.bloco=floorf(menor(disponivel_altura/linhas,disponivel_largura/j->largura));
    l.bloco=maior(7,l.bloco);
    float bw=j->largura*l.bloco, bh=linhas*l.bloco;
    l.tabuleiro=ret(roundf(w*.5f-bw*.5f),roundf(115+(h-170-bh)*.5f),bw,bh);
    l.placa=ret(l.tabuleiro.x-9,l.tabuleiro.y-9,bw+18,bh+18);
    l.esquerda=ret(28,117,maior(156,l.placa.x-46),h-178);
    l.direita=ret(l.placa.x+l.placa.w+19,117,maior(145,w-l.placa.x-l.placa.w-47),h-178);
    return l;
}
static Ret botao_jogo(const LayoutJogo *l,int i) {
    return ret(l->direita.x+17,l->direita.y+l->direita.h-167+i*49,l->direita.w-34,41);
}
static Ret modal(int w,int h) {return ret(w/2.f-230,h/2.f-175,460,350);}
static Ret modal_botao(Ret m,int i) {return ret(m.x+47,m.y+148+i*66,m.w-94,54);}

int iniciar_janela(Janela *j) {
    memset(j,0,sizeof(*j));
    if(!al_init() || !al_install_keyboard() || !al_install_mouse()) return 0;
    if(!al_init_primitives_addon()) return 0;
    al_init_font_addon();
    int ttf=al_init_ttf_addon();
    al_set_new_display_flags(ALLEGRO_RESIZABLE | ALLEGRO_WINDOWED);
    j->display=al_create_display(JANELA_W,JANELA_H);
    if(!j->display) goto falha;
    al_set_window_title(j->display,"Tetris V3 - Final");
    if(al_set_window_constraints(j->display,940,730,0,0))
        al_apply_window_constraints(j->display,true);
    if(ttf) {
        j->titulo=al_load_ttf_font("C:/Windows/Fonts/segoeuib.ttf",47,0);
        j->texto=al_load_ttf_font("C:/Windows/Fonts/segoeui.ttf",18,0);
        j->pequena=al_load_ttf_font("C:/Windows/Fonts/segoeui.ttf",14,0);
        j->marcador=al_load_ttf_font("C:/Windows/Fonts/segoeuib.ttf",10,0);
        j->destaque=al_load_ttf_font("C:/Windows/Fonts/segoeuib.ttf",25,0);
        if(!j->titulo) j->titulo=al_load_ttf_font("C:/Windows/Fonts/arialbd.ttf",47,0);
        if(!j->texto) j->texto=al_load_ttf_font("C:/Windows/Fonts/arial.ttf",18,0);
        if(!j->pequena) j->pequena=al_load_ttf_font("C:/Windows/Fonts/arial.ttf",14,0);
        if(!j->marcador) j->marcador=al_load_ttf_font("C:/Windows/Fonts/arialbd.ttf",10,0);
        if(!j->destaque) j->destaque=al_load_ttf_font("C:/Windows/Fonts/arialbd.ttf",25,0);
    }
    if(!j->titulo) j->titulo=al_create_builtin_font();
    if(!j->texto) j->texto=al_create_builtin_font();
    if(!j->pequena) j->pequena=al_create_builtin_font();
    if(!j->marcador) j->marcador=al_create_builtin_font();
    if(!j->destaque) j->destaque=al_create_builtin_font();
    j->timer=al_create_timer(1.0/60.0);
    j->eventos=al_create_event_queue();
    if(!j->titulo || !j->texto || !j->pequena || !j->marcador || !j->destaque || !j->timer || !j->eventos) goto falha;
    al_register_event_source(j->eventos,al_get_display_event_source(j->display));
    al_register_event_source(j->eventos,al_get_keyboard_event_source());
    al_register_event_source(j->eventos,al_get_mouse_event_source());
    al_register_event_source(j->eventos,al_get_timer_event_source(j->timer));
    efeitos_inicializar(&j->efeitos);
    sons_inicializar(&j->sons);
    al_start_timer(j->timer);
    return 1;
falha:
    fechar_janela(j);
    return 0;
}

int ajustar_tela_cheia(Janela *j,int ativar) {
    if(!al_set_display_flag(j->display,ALLEGRO_FULLSCREEN_WINDOW,ativar != 0))
        return 0;
    if(!ativar) al_resize_display(j->display,JANELA_W,JANELA_H);
    return 1;
}

AcaoInterface identificar_clique(const Janela *j,Tela tela,const Jogo *g,int x,int y) {
    int w=al_get_display_width(j->display),h=al_get_display_height(j->display);
    if(dentro(audio_topo(w,0),x,y))return ACAO_ALTERNAR_MUSICA;
    if(dentro(audio_topo(w,1),x,y))return ACAO_ALTERNAR_EFEITOS;
    if(tela==TELA_MENU) {
        if(dentro(menu_botao(w,h,0),x,y)) return ACAO_INICIAR;
        if(dentro(menu_botao(w,h,1),x,y)) return ACAO_CONFIG;
        if(dentro(menu_botao(w,h,2),x,y)) return ACAO_SAIR;
    } else if(tela==TELA_CONFIG) {
        Ret p=config_painel(w,h);
        for(int i=0;i<3;i++) if(dentro(config_dif(p,i),x,y)) return (AcaoInterface)(ACAO_FACIL+i);
        if(dentro(config_altura_menos(p),x,y))return ACAO_ALTURA_MENOS;
        if(dentro(config_altura_mais(p),x,y))return ACAO_ALTURA_MAIS;
        if(dentro(config_largura_menos(p),x,y))return ACAO_LARGURA_MENOS;
        if(dentro(config_largura_mais(p),x,y))return ACAO_LARGURA_MAIS;
        if(dentro(config_full(p),x,y))return ACAO_TELA_CHEIA;
        if(dentro(config_audio(p,0),x,y))return ACAO_ALTERNAR_MUSICA;
        if(dentro(config_audio(p,1),x,y))return ACAO_ALTERNAR_EFEITOS;
        if(dentro(config_voltar(p),x,y))return ACAO_VOLTAR;
    } else if(tela==TELA_JOGO) {
        LayoutJogo l=layout_jogo(w,h,g,linhas_visiveis(j,g));
        if(dentro(botao_jogo(&l,0),x,y))return ACAO_PAUSAR;
        if(dentro(botao_jogo(&l,1),x,y))return ACAO_DESFAZER;
        if(dentro(botao_jogo(&l,2),x,y))return ACAO_REINICIAR;
    } else if(tela==TELA_PAUSA || tela==TELA_FIM) {
        Ret m=modal(w,h);
        if(dentro(modal_botao(m,0),x,y))return tela==TELA_PAUSA?ACAO_CONTINUAR:ACAO_REINICIAR;
        if(dentro(modal_botao(m,1),x,y))return ACAO_MENU;
    }
    return ACAO_NENHUMA;
}

static void desenhar_menu(const Janela *j,int w,int h) {
    txt(j->pequena,cor(109,205,180),w*.09f,h*.25f,"VERSÃO 3");
    txt(j->titulo,cor(246,238,226),w*.09f,h*.31f,"TETRIS");
    
    botao(j,menu_botao(w,h,0),"Jogar",1,0);
    botao(j,menu_botao(w,h,1),"Configurações",0,0);
    botao(j,menu_botao(w,h,2),"Sair",0,0);
    
    
    Ret painel=ret(w*.60f,h*.20f,w*.32f,h*.59f);
    arredondado(painel,28,cor(29,49,61));
    contorno(painel,28,alfa(121,168,167,60));
    desenhar_peca_artistica(painel.x+painel.w*.18f,painel.y+64,42,2);
    desenhar_peca_artistica(painel.x+painel.w*.45f,painel.y+205,37,0);
    desenhar_peca_artistica(painel.x+painel.w*.14f,painel.y+340,32,5);
    
}

static void desenhar_config(const Janela *j,int w,int h,const Configuracao *c) {
    Ret p=config_painel(w,h);
    arredondado(p,24,cor(27,40,52));
    contorno(p,24,alfa(113,154,164,69));
    txt(j->titulo,cor(246,238,225),p.x+36,p.y+26,"Configurações");
    txt(j->pequena,cor(166,184,189),p.x+38,p.y+100,"Dificuldade");
    for(int i=0;i<3;i++) botao(j,config_dif(p,i),dificuldade_nome(i),0,c->dificuldade==i);
    txt(j->texto,cor(236,231,224),p.x+38,p.y+271,"Altura do tabuleiro");
    txt(j->texto,cor(236,231,224),p.x+38,p.y+338,"Largura do tabuleiro");
    botao(j,config_altura_menos(p),"-",0,0);
    botao(j,config_altura_mais(p),"+",0,0);
    botao(j,config_largura_menos(p),"-",0,0);
    botao(j,config_largura_mais(p),"+",0,0);
    al_draw_textf(j->texto,cor(245,233,215),p.x+p.w-104,p.y+277,ALLEGRO_ALIGN_CENTER,"%d",c->altura);
    al_draw_textf(j->texto,cor(245,233,215),p.x+p.w-104,p.y+344,ALLEGRO_ALIGN_CENTER,"%d",c->largura);
    txt(j->texto,cor(236,231,224),p.x+38,p.y+418,"Tela cheia");
    botao(j,config_full(p),c->tela_cheia?"Ativada":"Desativada",0,c->tela_cheia);
    botao(j,config_audio(p,0),j->sons.musica_ligada?"Musica ligada [M]":"Musica desligada [M]",0,j->sons.musica_ligada);
    botao(j,config_audio(p,1),j->sons.efeitos_ligados?"Efeitos ligados [N]":"Efeitos desligados [N]",0,j->sons.efeitos_ligados);
    botao(j,config_voltar(p),"Voltar",1,0);
}



/* Simbolo desenhado com primitivas: sem dependencias de imagens ou emojis. */




static void bloco(float x,float y,float t,int id,int fantasma) {
    if(fantasma) {
        contorno(ret(x+1,y+1,t-2,t-2),menor(3,t*.18f),alfa(149,210,190,115));
        return;
    }
    Ret r=ret(x+1,y+1,t-2,t-2);
    arredondado(r,menor(3,t*.18f),cor_peca(id));
    if(t>=11) {
        al_draw_line(x+3,y+3,x+t-4,y+3,alfa(255,255,255,104),1);
        al_draw_line(x+3,y+t-4,x+t-4,y+t-4,alfa(5,12,19,100),1);
    }
}



/* Efeitos visuais sao desenhados depois do tabuleiro, mas NAO modificam
   suas celulas: a colisao, a pontuacao e os TADs continuam independentes. */

static void desenhar_efeitos_grade(const Janela *j,LayoutJogo l,
                                  int linha_inicio,int linhas_visiveis,const Jogo *g) {
    const EfeitosVisuais *fx=&j->efeitos;
    float tamanho=l.bloco;
    if(fx->linhas_tempo>0 && fx->linhas_quantidade>0) {
        float progresso=1.f-(float)(fx->linhas_tempo/.44);
        if(progresso<0)progresso=0;
        if(progresso>1)progresso=1;
        float intensidade=(1.f-progresso);
        for(int k=0;k<fx->linhas_quantidade;k++) {
            int y=fx->linhas_y[k];
            if(y<linha_inicio || y>=linha_inicio+linhas_visiveis) continue;
            float py=l.tabuleiro.y+(y-linha_inicio)*tamanho;
            al_draw_filled_rectangle(l.tabuleiro.x,py,l.tabuleiro.x+l.tabuleiro.w,
                                     py+tamanho,alfa(248,244,218,(int)(155*intensidade)));
            float faixa=l.tabuleiro.x+l.tabuleiro.w*progresso;
            al_draw_line(faixa,py,faixa,py+tamanho,alfa(255,255,255,(int)(230*intensidade)),3.f);
            for(int x=0;x<g->largura;x++) {
                float px=l.tabuleiro.x+(x+.5f)*tamanho;
                float dispersao=(progresso*(8.f+(x%3)*4.f));
                float direcao=(x%2)?1.f:-1.f;
                al_draw_filled_circle(px+direcao*dispersao,py+tamanho*.5f-(x%3-1)*dispersao,
                         1.f+2.f*intensidade,alfa(255,225,155,(int)(210*intensidade)));
            }
        }
    }

    if(fx->impacto_tempo>0 && fx->impacto_y>=linha_inicio && fx->impacto_y<linha_inicio+linhas_visiveis) {
        float progresso=1.f-(float)(fx->impacto_tempo/.24);
        float cx=l.tabuleiro.x+(fx->impacto_x+.5f)*tamanho;
        float cy=l.tabuleiro.y+(fx->impacto_y-linha_inicio+.8f)*tamanho;
        for(int i=0;i<7;i++) {
            float dx=(i-3)*tamanho*.38f*progresso;
            float dy=-(1.f-(i%3)*.22f)*tamanho*.75f*progresso;
            al_draw_filled_circle(cx+dx,cy+dy,maior(.5f,2.f*(1.f-progresso)),
                                  alfa(233,217,177,(int)(140*(1.f-progresso))));
        }
    }
    if(fx->nascer_tempo>0 && fx->nascer_y>=linha_inicio && fx->nascer_y<linha_inicio+linhas_visiveis) {
        float progresso=1.f-(float)(fx->nascer_tempo/.32);
        float cx=l.tabuleiro.x+(fx->nascer_x+.5f)*tamanho;
        float cy=l.tabuleiro.y+(fx->nascer_y-linha_inicio+.5f)*tamanho;
        float raio=tamanho*(.55f+1.7f*progresso);
        al_draw_circle(cx,cy,raio,alfa(192,230,224,(int)(155*(1.f-progresso))),2.f);
    }
    if(fx->desfazer_tempo>0) {
        float intensidade=(float)(fx->desfazer_tempo/.28);
        al_draw_rounded_rectangle(l.placa.x-2,l.placa.y-2,l.placa.x+l.placa.w+2,
                   l.placa.y+l.placa.h+2,11,11,alfa(107,231,210,(int)(185*intensidade)),3.f);
    }
}

static void desenhar_jogo(const Janela *j,int w,int h,const Jogo *g,const PilhaEstados *p) {
    LayoutJogo l=layout_jogo(w,h,g,linhas_visiveis(j,g));
    arredondado(l.esquerda,18,cor(29,42,54));
    arredondado(l.direita,18,cor(29,42,54));
    contorno(l.esquerda,18,alfa(137,170,175,40));
    contorno(l.direita,18,alfa(137,170,175,40));
    arredondado(l.placa,8,cor(73,92,101));
    arredondado(l.tabuleiro,3,cor(15,26,41));
    int inicio=primeira_linha(j,g), quantidade=linhas_visiveis(j,g);
    for(int y=inicio;y<inicio+quantidade;y++) for(int x=0;x<g->largura;x++) {
        float px=l.tabuleiro.x+x*l.bloco,py=l.tabuleiro.y+(y-inicio)*l.bloco;
        if(g->tabuleiro[y][x]) {
            bloco(px,py,l.bloco,g->tabuleiro[y][x],0);

        } else {
            /* Célula sem espaços grossos: grade fina em cinza neutro. */
            al_draw_filled_rectangle(px,py,px+l.bloco,py+l.bloco,cor(27,34,45));
            al_draw_line(px+.5f,py+.5f,px+l.bloco-.5f,py+.5f,
                         alfa(132,134,139,55),.5f);
            al_draw_line(px+.5f,py+.5f,px+.5f,py+l.bloco-.5f,
                         alfa(132,134,139,55),.5f);
        }
    }
    int sombra=0;
    if(!g->fim) while(pode_mover(g,0,sombra+1,g->atual.matriz)) sombra++;
    if(!g->fim) for(int y=0;y<TAM;y++) for(int x=0;x<TAM;x++) if(g->atual.matriz[y][x]) {
        int cx=g->atual.x+x;
        int cy=g->atual.y+y;
        if(cx<0||cx>=g->largura||cy<0||cy>=g->altura)continue;
        float px=l.tabuleiro.x+cx*l.bloco,py=l.tabuleiro.y+(cy-inicio)*l.bloco;
        int sombra_visivel=cy+sombra>=inicio && cy+sombra<inicio+quantidade;
        if(!g->fim && sombra>0 && sombra_visivel)
            bloco(px,py+sombra*l.bloco,l.bloco,g->atual.tipo+1,1);
        if(cy<inicio || cy>=inicio+quantidade)continue;
        bloco(px,py,l.bloco,g->atual.tipo+1,0);
    }
    desenhar_efeitos_grade(j,l,inicio,quantidade,g);
    float sx=l.esquerda.x+22;
    txt(j->pequena,cor(137,195,183),sx,l.esquerda.y+22,"PLACAR");
    txt(j->pequena,cor(164,184,190),sx,l.esquerda.y+66,"Pontos");
    al_draw_textf(j->titulo,cor(248,237,220),sx,l.esquerda.y+88,0,"%d",g->pontos);
    al_draw_line(sx,l.esquerda.y+164,l.esquerda.x+l.esquerda.w-22,l.esquerda.y+164,alfa(173,188,191,40),1);
    al_draw_textf(j->texto,cor(243,235,223),sx,l.esquerda.y+184,0,"Linhas   %d",g->linhas);
    al_draw_textf(j->texto,cor(243,235,223),sx,l.esquerda.y+222,0,"Nivel    %d",nivel_atual(g));
    txt(j->pequena,cor(154,175,179),sx,l.esquerda.y+266,"Próximas peças");
    for(int i=0;i<g->proximas.quantidade;i++) {
        int tipo;
        if(!fila_elemento(&g->proximas,i,&tipo))continue;
        float passo=menor(53,(l.esquerda.h-357)/4.f);
        float py=l.esquerda.y+301+i*passo;
        Ret item=ret(sx,py,l.esquerda.w-44,47);
        arredondado(item,8,cor(38,54,65));
        if(i==0) contorno(item,8,alfa(106,209,174,102));
        desenhar_peca_artistica(sx+12,py+2,10,tipo);
        al_draw_textf(j->pequena,cor(195,208,207),item.x+item.w-21,py+13,ALLEGRO_ALIGN_RIGHT,"%d/5",i+1);
    }
    float dx=l.direita.x+22;
    txt(j->pequena,cor(137,195,183),dx,l.direita.y+22,"CONTROLES");
    txt(j->pequena,cor(195,209,210),dx,l.direita.y+72,"A / D: mover   W / E: girar");
    txt(j->pequena,cor(195,209,210),dx,l.direita.y+103,"S: descer   Espaco: soltar");
    al_draw_textf(j->pequena,cor(195,209,210),dx,l.direita.y+134,0,
                  "U: desfazer (%d estados)",pilha_quantidade(p));
    txt(j->pequena,cor(195,209,210),dx,l.direita.y+165,"P: pausar   R: reiniciar");
    txt(j->pequena,cor(195,209,210),dx,l.direita.y+196,"V: zoom   F11: tela cheia");
    txt(j->pequena,cor(195,209,210),dx,l.direita.y+227,"M: musica   N: efeitos");
    /* Audio e musica continuam controlados pelos botoes do cabecalho. */
    botao(j,botao_jogo(&l,0),"Pausar [P]",0,0);
    botao(j,botao_jogo(&l,1),"Desfazer [U]",0,0);
    botao(j,botao_jogo(&l,2),"Reiniciar [R]",0,0);
    const char *vista=j->modo_visual==0?"Visao completa":
            j->modo_visual==1?"Zoom: parte superior":"Zoom: parte inferior";
    al_draw_textf(j->pequena,cor(138,159,166),w/2.f,h-33,ALLEGRO_ALIGN_CENTER,
                  "%d x %d - %s - %s - %.2fs por linha",g->altura,g->largura,
                  dificuldade_nome(g->dificuldade),vista,intervalo_gravidade(g));
}

static void sobreposicao(const Janela *j,int w,int h,int fim) {
    al_draw_filled_rectangle(0,0,w,h,alfa(12,21,30,215));
    Ret m=modal(w,h);
    arredondado(m,22,cor(30,45,56));
    contorno(m,22,alfa(149,185,179,83));
    txt_centro(j->titulo,cor(246,237,226),w/2.f,m.y+33,fim?"Fim de jogo":"Pausado");
    
    botao(j,modal_botao(m,0),fim?"Reiniciar":"Continuar",1,0);
    botao(j,modal_botao(m,1),"Menu",0,0);
}

void desenhar_janela(const Janela *j,Tela tela,const Jogo *g,
                    const PilhaEstados *p,const Configuracao *c) {
    int w=al_get_display_width(j->display),h=al_get_display_height(j->display);
    fundo(w,h);
    cabecalho(j,w,tela==TELA_MENU?"MENU":tela==TELA_CONFIG?"CONFIGURAÇÕES":"JOGO");
    if(tela==TELA_MENU) desenhar_menu(j,w,h);
    else if(tela==TELA_CONFIG) desenhar_config(j,w,h,c);
    else {
        desenhar_jogo(j,w,h,g,p);
        if(tela==TELA_PAUSA || tela==TELA_FIM) sobreposicao(j,w,h,tela==TELA_FIM);
    }
    /* Desenhado por ultimo para continuar acessivel ate durante a pausa. */
    desenhar_audio_topo(j,w);
    al_flip_display();
}

void fechar_janela(Janela *j) {
    sons_finalizar(&j->sons);
    if(j->eventos) al_destroy_event_queue(j->eventos);
    if(j->timer) al_destroy_timer(j->timer);
    if(j->titulo) al_destroy_font(j->titulo);
    if(j->texto) al_destroy_font(j->texto);
    if(j->pequena) al_destroy_font(j->pequena);
    if(j->marcador) al_destroy_font(j->marcador);
    if(j->destaque) al_destroy_font(j->destaque);
    if(j->display) al_destroy_display(j->display);
    memset(j,0,sizeof(*j));
}
