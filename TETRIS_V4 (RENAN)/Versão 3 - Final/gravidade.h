#ifndef GRAVIDADE_H
#define GRAVIDADE_H
#include "jogo.h"
int nivel_atual(const Jogo *j);
double intervalo_gravidade(const Jogo *j);
void aplicar_gravidade(Jogo *j);
#endif
