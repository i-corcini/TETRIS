CC = gcc
CFLAGS = -std=c11 -Wall -Wextra
PACOTES = allegro-5 allegro_font-5 allegro_ttf-5 allegro_primitives-5 allegro_audio-5 allegro_acodec-5
CFLAGS_ALLEGRO = $(shell pkg-config --cflags $(PACOTES))
LIBS_ALLEGRO = $(shell pkg-config --libs $(PACOTES))
FONTES = main.c tabuleiro.c tetraminos.c movimentacao.c pontuacao.c gravidade.c poderes.c efeitos.c sons.c window.c
LOGICA = tabuleiro.c tetraminos.c movimentacao.c pontuacao.c gravidade.c poderes.c efeitos.c
.PHONY: all run test clean
all: tetris_v4_poderes.exe
tetris_v4_poderes.exe: $(FONTES) jogo.h window.h gravidade.h movimentacao.h tetraminos.h efeitos.h sons.h poderes.h
	$(CC) $(CFLAGS) $(CFLAGS_ALLEGRO) $(FONTES) -o $@ $(LIBS_ALLEGRO) -lm
run: tetris_v4_poderes.exe
	./tetris_v4_poderes.exe
test:
	$(CC) $(CFLAGS) -I. tests/testar_logica.c $(LOGICA) -o testes_logica.exe
	./testes_logica.exe
	$(CC) $(CFLAGS) -I. tests/testar_animacoes.c $(LOGICA) -o testes_animacoes.exe
	./testes_animacoes.exe
clean:
	rm -f tetris_v4_poderes.exe testes_logica.exe testes_animacoes.exe
