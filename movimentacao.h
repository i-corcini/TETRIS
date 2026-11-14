#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int pode_mover(Jogo *j, int dx, int dy, int matriz[4][4]);

void mover(Jogo *j, int dx, int dy);

void rotacionar(Jogo *j);

void fixar_peca(Jogo *j);

void queda_rapida(Jogo *j);
