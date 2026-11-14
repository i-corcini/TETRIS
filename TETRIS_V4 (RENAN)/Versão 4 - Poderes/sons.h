#ifndef SONS_H
#define SONS_H
#include <allegro5/allegro_audio.h>

typedef enum {
    SOM_MENU, SOM_INICIAR, SOM_MOVER, SOM_GIRAR, SOM_FIXAR,
    SOM_QUEDA, SOM_LINHA, SOM_GELO, SOM_RAIO, SOM_BONUS,
    SOM_DESFAZER, SOM_FIM, SOM_TOTAL
} TipoSom;

typedef struct {
    ALLEGRO_SAMPLE *efeitos[SOM_TOTAL];
    ALLEGRO_SAMPLE *musica;              /* trilha original em assets/music */
    ALLEGRO_SAMPLE_ID musica_id;         /* usada para interromper somente a musica */
    int disponivel;
    int musica_disponivel;
    int efeitos_ligados;
    int musica_ligada;
    int musica_tocando;
} SonsJogo;

void sons_inicializar(SonsJogo *s);
void sons_tocar(SonsJogo *s, TipoSom tipo);
void sons_alternar_efeitos(SonsJogo *s);
void sons_alternar_musica(SonsJogo *s);
void sons_reproduzir_musica(SonsJogo *s);
void sons_finalizar(SonsJogo *s);
#endif
