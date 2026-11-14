#include "sons.h"
#include <allegro5/allegro_acodec.h>
#include <string.h>
#include <stdio.h>

static const char *nomes[SOM_TOTAL]={
    "menu", "inicio", "mover", "girar", "fixar", "queda", "linha",
    "gelo", "raio", "bonus", "desfazer", "fim"
};
static const float volumes[SOM_TOTAL]={
    .40f,.55f,.14f,.24f,.32f,.58f,.72f,.60f,.62f,.54f,.38f,.53f
};

/* Trilha e efeitos nao compartilham o mesmo interruptor. */
void sons_reproduzir_musica(SonsJogo *s) {
    if(!s || !s->musica_ligada || !s->musica_disponivel || s->musica_tocando)
        return;
    if(al_play_sample(s->musica, .23f, 0.0f, 1.0f,
                      ALLEGRO_PLAYMODE_LOOP, &s->musica_id)) {
        s->musica_tocando=1;
    } else {
        fprintf(stderr,"Aviso: nao foi possivel iniciar a trilha musical.\n");
    }
}

void sons_inicializar(SonsJogo *s) {
    if(!s)return;
    memset(s,0,sizeof(*s));
    s->efeitos_ligados=1;
    s->musica_ligada=1;
    /* Se o audio falhar, o jogo continua funcionando normalmente. */
    if(!al_install_audio()) {
        fprintf(stderr,"Aviso: subsistema de audio indisponivel.\n");
        return;
    }
    if(!al_init_acodec_addon() || !al_reserve_samples(20)) {
        fprintf(stderr,"Aviso: codecs/mixer Allegro indisponiveis.\n");
        return;
    }
    int carregados=0;
    for(int i=0;i<SOM_TOTAL;i++) {
        char arquivo[128];
        snprintf(arquivo,sizeof(arquivo),"assets/sfx/%s.wav",nomes[i]);
        s->efeitos[i]=al_load_sample(arquivo);
        if(s->efeitos[i])carregados++;
    }
    if(carregados==0)
        fprintf(stderr,"Aviso: WAVs de efeitos nao encontrados (assets/sfx).\n");
    s->disponivel=carregados>0;
    s->musica=al_load_sample("assets/music/tema_original.wav");
    s->musica_disponivel=s->musica!=NULL;
    if(!s->musica_disponivel)
        fprintf(stderr,"Aviso: trilha nao encontrada em assets/music/tema_original.wav.\n");
    sons_reproduzir_musica(s);
}

void sons_tocar(SonsJogo *s, TipoSom tipo) {
    if(!s || !s->efeitos_ligados || !s->disponivel ||
       tipo<0 || tipo>=SOM_TOTAL || !s->efeitos[tipo])return;
    al_play_sample(s->efeitos[tipo],volumes[tipo],0.0f,1.0f,
                   ALLEGRO_PLAYMODE_ONCE,NULL);
}

void sons_alternar_efeitos(SonsJogo *s) {
    if(s)s->efeitos_ligados=!s->efeitos_ligados;
}

void sons_alternar_musica(SonsJogo *s) {
    if(!s)return;
    s->musica_ligada=!s->musica_ligada;
    if(!s->musica_ligada) {
        if(s->musica_tocando)al_stop_sample(&s->musica_id);
        s->musica_tocando=0;
    } else sons_reproduzir_musica(s);
}

void sons_finalizar(SonsJogo *s) {
    if(!s)return;
    if(s->musica_tocando) al_stop_sample(&s->musica_id);
    if(s->musica) al_destroy_sample(s->musica);
    for(int i=0;i<SOM_TOTAL;i++) {
        if(s->efeitos[i])al_destroy_sample(s->efeitos[i]);
    }
    if(al_is_audio_installed())al_uninstall_audio();
    memset(s,0,sizeof(*s));
}
