#ifndef WINDOW_H
#define WINDOW_H
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include "jogo.h"
#include "efeitos.h"
#include "sons.h"

typedef enum {
    TELA_MENU, TELA_CONFIG, TELA_JOGO, TELA_PAUSA, TELA_FIM
} Tela;

typedef enum {
    ACAO_NENHUMA, ACAO_INICIAR, ACAO_CONFIG, ACAO_VOLTAR, ACAO_SAIR,
    ACAO_FACIL, ACAO_NORMAL, ACAO_DIFICIL,
    ACAO_ALTURA_MENOS, ACAO_ALTURA_MAIS,
    ACAO_LARGURA_MENOS, ACAO_LARGURA_MAIS,
    ACAO_TELA_CHEIA, ACAO_CONTINUAR, ACAO_REINICIAR,
    ACAO_MENU, ACAO_PAUSAR, ACAO_DESFAZER,
    ACAO_ALTERNAR_MUSICA, ACAO_ALTERNAR_EFEITOS
} AcaoInterface;

typedef struct {
    int altura, largura, dificuldade;
    int tela_cheia;
} Configuracao;

typedef struct {
    ALLEGRO_DISPLAY *display;
    ALLEGRO_EVENT_QUEUE *eventos;
    ALLEGRO_TIMER *timer;
    ALLEGRO_FONT *titulo;
    ALLEGRO_FONT *texto;
    ALLEGRO_FONT *pequena;
    ALLEGRO_FONT *marcador;
    ALLEGRO_FONT *destaque;
    SonsJogo sons;
    EfeitosVisuais efeitos;
    int mouse_x, mouse_y;
    int modo_visual; /* 0: completo, 1: parte superior, 2: parte inferior */
} Janela;

int iniciar_janela(Janela *janela);
void fechar_janela(Janela *janela);
int ajustar_tela_cheia(Janela *janela, int ativar);
AcaoInterface identificar_clique(const Janela *janela, Tela tela, const Jogo *jogo,
                                int mouse_x, int mouse_y);
void desenhar_janela(const Janela *janela, Tela tela,
                    const Jogo *jogo, const PilhaEstados *historico,
                    const Configuracao *config);
#endif
