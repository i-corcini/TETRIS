#include <stdbool.h>
#include <stdio.h>
#include "window.h"
#include "jogo.h"
#include "tetraminos.h"
#include "movimentacao.h"
#include "tabuleiro.h"
#include "gravidade.h"

/* O checkpoint representa o inicio de uma peca, nao cada tecla pressionada. */
static void comecar_partida(Jogo *j, PilhaEstados *p, const Configuracao *c,
                           int *alterado) {
    inicializar_com_config(j,c->altura,c->largura,c->dificuldade);
    nova_peca(j);
    if(!j->fim && !pode_mover(j,0,0,j->atual.matriz)) j->fim=1;
    pilha_inicializar(p);
    if(!j->fim) pilha_empilhar(p,j);
    /* Primeira pressao em U sempre recupera o nascimento da peca atual. */
    *alterado=1;
}

static void fechar_jogada(Jogo *j,PilhaEstados *p,int *alterado,Janela *janela) {
    Jogo anterior=*j;
    unsigned int geradas=j->pecas_geradas;
    finalizar_peca(j);
    efeitos_ao_fixar(&janela->efeitos,&anterior,j);
    sons_tocar(&janela->sons,SOM_FIXAR);
    if(j->linhas>anterior.linhas) sons_tocar(&janela->sons,SOM_LINHA);

    if(j->fim && !anterior.fim) sons_tocar(&janela->sons,SOM_FIM);
    if(!j->fim && j->pecas_geradas>geradas) {
        pilha_empilhar(p,j);
        /* Novo checkpoint: o primeiro U volta esta peca ao teto. */
        *alterado=1;
    } else {
        *alterado=1;
    }
}

static void iniciar_com_efeitos(Janela *janela,Jogo *j,PilhaEstados *p,
                                const Configuracao *c,int *alterado) {
    comecar_partida(j,p,c,alterado);
    efeitos_inicializar(&janela->efeitos);
    efeitos_nova_peca(&janela->efeitos,j);
    sons_tocar(&janela->sons,SOM_INICIAR);
}

static int tentar_mover(Jogo *j,int *alterado,int dx,int dy) {
    if(j->fim || !mover(j,dx,dy)) return 0;
    *alterado=1;
    return 1;
}

static int tentar_rotacionar(Jogo *j,int *alterado,int sentido) {
    if(j->fim || !rotacionar_sentido(j,sentido)) return 0;
    *alterado=1;
    return 1;
}

int main(void) {
    Janela janela;
    Jogo jogo;
    PilhaEstados historico;
    Configuracao config={ALTURA_PADRAO,LARGURA_PADRAO,DIFICULDADE_NORMAL,0};
    Tela tela=TELA_MENU;
    bool executando=true,redesenhar=true;
    bool esquerda=false,direita=false,baixo=false;
    int alterado=0;
    double gravidade=0.0,contato=0.0,repeticao_x=0.0,repeticao_baixo=0.0;
    double grace_undo=0.0,ultimo_tempo=0.0;

    if(!iniciar_janela(&janela)) {
        fprintf(stderr,"Erro ao criar a janela Allegro. Confira a instalacao.\n");
        return 1;
    }
    comecar_partida(&jogo,&historico,&config,&alterado);
    ultimo_tempo=al_get_time();

    while(executando) {
        ALLEGRO_EVENT e;
        al_wait_for_event(janela.eventos,&e);

        if(e.type==ALLEGRO_EVENT_DISPLAY_CLOSE) {
            executando=false;
        } else if(e.type==ALLEGRO_EVENT_DISPLAY_RESIZE) {
            al_acknowledge_resize(janela.display);
            redesenhar=true;
        } else if(e.type==ALLEGRO_EVENT_MOUSE_AXES) {
            janela.mouse_x=e.mouse.x;
            janela.mouse_y=e.mouse.y;
            redesenhar=true;
        } else if(e.type==ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && e.mouse.button==1) {
            AcaoInterface acao=identificar_clique(&janela,tela,&jogo,e.mouse.x,e.mouse.y);
            if(acao!=ACAO_NENHUMA && acao!=ACAO_ALTERNAR_EFEITOS && acao!=ACAO_ALTERNAR_MUSICA)
                sons_tocar(&janela.sons,SOM_MENU);
            switch(acao) {
                case ACAO_INICIAR:
                case ACAO_REINICIAR:
                    iniciar_com_efeitos(&janela,&jogo,&historico,&config,&alterado);
                    tela=TELA_JOGO;
                    gravidade=contato=repeticao_x=repeticao_baixo=grace_undo=0.0;
                    esquerda=direita=baixo=false;
                    break;
                case ACAO_CONFIG:tela=TELA_CONFIG;break;
                case ACAO_VOLTAR: case ACAO_MENU:tela=TELA_MENU;break;
                case ACAO_SAIR:executando=false;break;
                case ACAO_FACIL:config.dificuldade=DIFICULDADE_FACIL;break;
                case ACAO_NORMAL:config.dificuldade=DIFICULDADE_NORMAL;break;
                case ACAO_DIFICIL:config.dificuldade=DIFICULDADE_DIFICIL;break;
                case ACAO_ALTURA_MENOS:if(config.altura>MIN_ALTURA)config.altura-=2;break;
                case ACAO_ALTURA_MAIS:if(config.altura<MAX_ALTURA)config.altura+=2;break;
                case ACAO_LARGURA_MENOS:if(config.largura>MIN_LARGURA)config.largura--;break;
                case ACAO_LARGURA_MAIS:if(config.largura<MAX_LARGURA)config.largura++;break;
                case ACAO_ALTERNAR_MUSICA: sons_alternar_musica(&janela.sons); break;
                case ACAO_ALTERNAR_EFEITOS: sons_alternar_efeitos(&janela.sons); break;
                case ACAO_TELA_CHEIA:
                    if(ajustar_tela_cheia(&janela,!config.tela_cheia))config.tela_cheia=!config.tela_cheia;
                    break;
                case ACAO_CONTINUAR:tela=TELA_JOGO;grace_undo=0;break;
                case ACAO_PAUSAR:tela=TELA_PAUSA;esquerda=direita=baixo=false;break;
                case ACAO_DESFAZER:
                    if(pilha_desfazer_peca(&historico,&jogo,&alterado)) {
                        efeitos_desfazer(&janela.efeitos,&jogo);
                        sons_tocar(&janela.sons,SOM_DESFAZER);
                        tela=TELA_JOGO;
                        gravidade=contato=0;
                        grace_undo=0.45;
                        esquerda=direita=baixo=false;
                    }
                    break;
                default:break;
            }
            redesenhar=true;
        } else if(e.type==ALLEGRO_EVENT_KEY_DOWN) {
            int k=e.keyboard.keycode;
            if(k==ALLEGRO_KEY_M) {
                sons_alternar_musica(&janela.sons);
            } else if(k==ALLEGRO_KEY_N) {
                sons_alternar_efeitos(&janela.sons);
            } else if(k==ALLEGRO_KEY_F11) {
                if(ajustar_tela_cheia(&janela,!config.tela_cheia))config.tela_cheia=!config.tela_cheia;
            } else if(k==ALLEGRO_KEY_ESCAPE) {
                if(tela==TELA_MENU) executando=false;
                else if(tela==TELA_CONFIG) tela=TELA_MENU;
                else if(tela==TELA_JOGO) tela=TELA_PAUSA;
                else tela=TELA_MENU;
                esquerda=direita=baixo=false;
            } else if(tela==TELA_MENU) {
                if(k==ALLEGRO_KEY_ENTER || k==ALLEGRO_KEY_SPACE) {
                    iniciar_com_efeitos(&janela,&jogo,&historico,&config,&alterado);
                    tela=TELA_JOGO;
                    gravidade=contato=grace_undo=0;
                }
            } else if(tela==TELA_PAUSA) {
                if(k==ALLEGRO_KEY_P || k==ALLEGRO_KEY_ENTER) tela=TELA_JOGO;
                else if(k==ALLEGRO_KEY_R) {
                    iniciar_com_efeitos(&janela,&jogo,&historico,&config,&alterado);
                    tela=TELA_JOGO;
                    gravidade=contato=grace_undo=0;
                }
            } else if(tela==TELA_FIM) {
                if(k==ALLEGRO_KEY_R || k==ALLEGRO_KEY_ENTER) {
                    iniciar_com_efeitos(&janela,&jogo,&historico,&config,&alterado);
                    tela=TELA_JOGO;
                    gravidade=contato=grace_undo=0;
                } else if(k==ALLEGRO_KEY_U) {
                    if(pilha_desfazer_peca(&historico,&jogo,&alterado)) {
                        efeitos_desfazer(&janela.efeitos,&jogo);
                        sons_tocar(&janela.sons,SOM_DESFAZER);
                        tela=TELA_JOGO;
                        gravidade=contato=0;
                        grace_undo=0.45;
                    }
                }
            } else if(tela==TELA_JOGO) {
                if(k==ALLEGRO_KEY_P) {
                    tela=TELA_PAUSA;
                    esquerda=direita=baixo=false;
                } else if(k==ALLEGRO_KEY_R) {
                    iniciar_com_efeitos(&janela,&jogo,&historico,&config,&alterado);
                    gravidade=contato=grace_undo=0;
                    esquerda=direita=baixo=false;
                } else if(k==ALLEGRO_KEY_U) {
                    if(pilha_desfazer_peca(&historico,&jogo,&alterado)) {
                        efeitos_desfazer(&janela.efeitos,&jogo);
                        sons_tocar(&janela.sons,SOM_DESFAZER);
                        gravidade=contato=0;
                        grace_undo=0.45;
                        esquerda=direita=baixo=false;
                    }
                } else if(k==ALLEGRO_KEY_A || k==ALLEGRO_KEY_LEFT) {
                    if(!esquerda && tentar_mover(&jogo,&alterado,-1,0)) {
                        contato=0; sons_tocar(&janela.sons,SOM_MOVER);
                    }
                    esquerda=true;
                    repeticao_x=-0.16;
                } else if(k==ALLEGRO_KEY_D || k==ALLEGRO_KEY_RIGHT) {
                    if(!direita && tentar_mover(&jogo,&alterado,1,0)) {
                        contato=0; sons_tocar(&janela.sons,SOM_MOVER);
                    }
                    direita=true;
                    repeticao_x=-0.16;
                } else if(k==ALLEGRO_KEY_S || k==ALLEGRO_KEY_DOWN) {
                    if(!baixo) tentar_mover(&jogo,&alterado,0,1);
                    baixo=true;
                    repeticao_baixo=-0.10;
                } else if(k==ALLEGRO_KEY_W || k==ALLEGRO_KEY_UP) {
                    if(tentar_rotacionar(&jogo,&alterado,1)) {
                        contato=0; sons_tocar(&janela.sons,SOM_GIRAR);
                    }
                } else if(k==ALLEGRO_KEY_E) {
                    if(tentar_rotacionar(&jogo,&alterado,-1)) {
                        contato=0; sons_tocar(&janela.sons,SOM_GIRAR);
                    }
                } else if(k==ALLEGRO_KEY_V) {
                    janela.modo_visual=(janela.modo_visual+1)%3;
                } else if(k==ALLEGRO_KEY_SPACE && !jogo.fim) {
                    sons_tocar(&janela.sons,SOM_QUEDA);
                    queda_rapida(&jogo);
                    fechar_jogada(&jogo,&historico,&alterado,&janela);
                    gravidade=contato=0;
                    if(jogo.fim) tela=TELA_FIM;
                }
            }
            redesenhar=true;
        } else if(e.type==ALLEGRO_EVENT_KEY_UP) {
            int k=e.keyboard.keycode;
            if(k==ALLEGRO_KEY_A || k==ALLEGRO_KEY_LEFT)esquerda=false;
            if(k==ALLEGRO_KEY_D || k==ALLEGRO_KEY_RIGHT)direita=false;
            if(k==ALLEGRO_KEY_S || k==ALLEGRO_KEY_DOWN)baixo=false;
        } else if(e.type==ALLEGRO_EVENT_TIMER) {
            double agora=al_get_time();
            double dt=agora-ultimo_tempo;
            ultimo_tempo=agora;
            if(dt<0)dt=0;
            if(dt>0.05)dt=0.05; /* Impede saltos apos arrastar a janela. */
            if(tela==TELA_JOGO) efeitos_atualizar(&janela.efeitos,dt);
            if(tela==TELA_JOGO && !jogo.fim) {
                if(grace_undo>0) {
                    grace_undo-=dt;
                    if(grace_undo<0)grace_undo=0;
                } else {
                    gravidade+=dt;
                    if(esquerda || direita) {
                        repeticao_x+=dt;
                        if(repeticao_x>=0.085) {
                            if(esquerda && !direita && tentar_mover(&jogo,&alterado,-1,0)) {
                                contato=0; sons_tocar(&janela.sons,SOM_MOVER);
                            }
                            if(direita && !esquerda && tentar_mover(&jogo,&alterado,1,0)) {
                                contato=0; sons_tocar(&janela.sons,SOM_MOVER);
                            }
                            repeticao_x=0;
                        }
                    }
                    if(baixo) {
                        repeticao_baixo+=dt;
                        if(repeticao_baixo>=0.060) {
                            tentar_mover(&jogo,&alterado,0,1);
                            repeticao_baixo=0;
                        }
                    }
                    /* Acelera sem pausa aleatoria; bloqueio recebe atraso curto de 0.17s. */
                    if(pode_mover(&jogo,0,1,jogo.atual.matriz)) {
                        contato=0;
                        double intervalo=intervalo_gravidade(&jogo);
                        if(gravidade>=intervalo) {
                            /* Queda automatica nao e uma jogada manual para a pilha. */
                            mover(&jogo,0,1);
                            gravidade-=intervalo;
                        }
                    } else {
                        gravidade=0;
                        contato+=dt;
                        if(contato>=0.17) {
                            fechar_jogada(&jogo,&historico,&alterado,&janela);
                            contato=gravidade=0;
                            if(jogo.fim)tela=TELA_FIM;
                        }
                    }
                }
            }
            redesenhar=true;
        }
        if(redesenhar && al_is_event_queue_empty(janela.eventos)) {
            desenhar_janela(&janela,tela,&jogo,&historico,&config);
            redesenhar=false;
        }
    }
    fechar_janela(&janela);
    return 0;
}
