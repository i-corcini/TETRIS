CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic
FONTES = main.c tabuleiro.c tetraminos.c movimentacao.c pontuacao.c gravidade.c
LOGICA = tabuleiro.c tetraminos.c movimentacao.c pontuacao.c gravidade.c
.PHONY: all run test clean
all: tetris_terminal.exe
tetris_terminal.exe: $(FONTES)
	$(CC) $(CFLAGS) $(FONTES) -o $@
run: tetris_terminal.exe
	./tetris_terminal.exe
test:
	$(CC) $(CFLAGS) -I. tests/testar_logica.c $(LOGICA) -o testes_logica.exe
	./testes_logica.exe
clean:
	rm -f tetris_terminal.exe testes_logica.exe
