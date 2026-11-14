#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "jogo.h"
#include "tabuleiro.h"
#include "tetraminos.h"
#include "movimentacao.h"
#include "gravidade.h"

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
static HANDLE saida_console;
static DWORD modo_original;
static int modo_alterado = 0;
static void iniciar_terminal(void) {
    DWORD modo;
    saida_console = GetStdHandle(STD_OUTPUT_HANDLE);
    if (saida_console != INVALID_HANDLE_VALUE &&
        GetConsoleMode(saida_console, &modo)) {
        modo_original = modo;
        modo_alterado = SetConsoleMode(saida_console,
                                      modo | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
    printf("\033[?1049h\033[?25l");
}
static void restaurar_terminal(void) {
    printf("\033[?25h\033[?1049l\n");
    if (modo_alterado) SetConsoleMode(saida_console, modo_original);
}
static int ler_tecla(void) {
    if (!_kbhit()) return -1;
    int tecla = _getch();
    if (tecla == 0 || tecla == 224) { (void)_getch(); return -1; }
    return tecla;
}
static uint64_t relogio_ms(void) { return (uint64_t)GetTickCount64(); }
static void dormir_ms(unsigned int ms) { Sleep(ms); }
#else
/* Compatibilidade para testes em Linux/macOS. */
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>
static struct termios original;
static int configurado = 0;
static void iniciar_terminal(void) {
    if (isatty(STDIN_FILENO) && tcgetattr(STDIN_FILENO, &original) == 0) {
        struct termios novo = original;
        novo.c_lflag &= (tcflag_t)~(ICANON | ECHO);
        novo.c_cc[VMIN] = 0;
        novo.c_cc[VTIME] = 0;
        if (tcsetattr(STDIN_FILENO, TCSANOW, &novo) == 0) configurado = 1;
    }
    printf("\033[?1049h\033[?25l");
}
static void restaurar_terminal(void) {
    if (configurado) tcsetattr(STDIN_FILENO, TCSANOW, &original);
    printf("\033[?25h\033[?1049l\n");
}
static int ler_tecla(void) {
    fd_set conjunto;
    struct timeval espera = {0,0};
    unsigned char tecla;
    FD_ZERO(&conjunto);
    FD_SET(STDIN_FILENO, &conjunto);
    if (select(STDIN_FILENO+1, &conjunto, NULL, NULL, &espera) > 0 &&
        read(STDIN_FILENO, &tecla, 1) == 1) return (int)tecla;
    return -1;
}
static uint64_t relogio_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000u + (uint64_t)tv.tv_usec / 1000u;
}
static void dormir_ms(unsigned int ms) {
    struct timeval tv;
    tv.tv_sec = (long)(ms/1000u);
    tv.tv_usec = (long)(ms%1000u)*1000L;
    select(0, NULL, NULL, NULL, &tv);
}
#endif

/* Um checkpoint por nova peca: acompanha estado, pontos, fila e semente. */
static void iniciar_partida(Jogo *j, PilhaEstados *historico, int *alterado) {
    inicializar(j);
    nova_peca(j);
    if (!j->fim && !pode_mover(j, 0, 0, j->atual.matriz)) j->fim = 1;
    pilha_inicializar(historico);
    if (!j->fim) pilha_empilhar(historico, j);
    *alterado = 0;
}

static void finalizar_jogada(Jogo *j, PilhaEstados *historico, int *alterado) {
    if (j->fim) return;
    finalizar_peca(j);
    if (!j->fim) {
        pilha_empilhar(historico, j);
        *alterado = 0;
    } else {
        *alterado = 1;
    }
}

/* Nao bloqueia a leitura de teclas e nao cria checkpoint por movimento. */
static int processar_tecla(Jogo *j, PilhaEstados *historico,
                           EstadoGravidade *gravidade, int tecla,
                           int *pausado, int *vista, int *sair,
                           int *alterado, double *pausa_desfazer) {
    if (tecla >= 'a' && tecla <= 'z') tecla += 'A' - 'a';
    if (tecla == 'Q') { *sair = 1; return 0; }
    if (tecla == 'R') {
        iniciar_partida(j, historico, alterado);
        gravidade_reiniciar(gravidade);
        *pausado = 0;
        *pausa_desfazer = 0;
        return 1;
    }
    if (tecla == 'V') { *vista = (*vista + 1) % 3; return 1; }
    if (tecla == 'U') {
        if (pilha_desfazer_peca(historico, j, alterado)) {
            gravidade_reiniciar(gravidade);
            *pausado = 0;
            *pausa_desfazer = 0.75; /* Tempo para repetir U, sem atrasar jogadas normais. */
            return 1;
        }
        return 0;
    }
    if (tecla == 'P' && !j->fim) { *pausado = !*pausado; return 1; }
    if (*pausado || j->fim) return 0;

    if (tecla == 'A' || tecla == 'D' || tecla == 'S') {
        int dx = tecla == 'A' ? -1 : tecla == 'D' ? 1 : 0;
        int dy = tecla == 'S' ? 1 : 0;
        if (mover(j, dx, dy)) {
            *alterado = 1;
            if (pode_mover(j, 0, 1, j->atual.matriz))
                gravidade->tempo_contato = 0;
            return 1;
        }
        return 0; /* Solo: o intervalo curto de travamento cuida da fixacao. */
    }
    if (tecla == 'W' || tecla == 'E') {
        if (!rotacionar_sentido(j, tecla == 'W' ? 1 : -1)) return 0;
        *alterado = 1;
        if (pode_mover(j, 0, 1, j->atual.matriz))
            gravidade->tempo_contato = 0;
        return 1;
    }
    if (tecla == ' ') {
        queda_rapida(j);
        finalizar_jogada(j, historico, alterado);
        gravidade_reiniciar(gravidade);
        return 1;
    }
    return 0;
}

int main(void) {
    Jogo jogo;
    PilhaEstados historico;
    EstadoGravidade gravidade;
    int sair = 0, pausado = 0, vista = 0, redesenhar = 1;
    int alterado = 0;
    double pausa_desfazer = 0.0;
    iniciar_partida(&jogo, &historico, &alterado);
    gravidade_reiniciar(&gravidade);
    iniciar_terminal();
    atexit(restaurar_terminal);
    uint64_t ultimo = relogio_ms();
    while (!sair) {
        int tecla = ler_tecla();
        uint64_t agora = relogio_ms();
        double dt = agora >= ultimo ? (double)(agora - ultimo) / 1000.0 : 0.0;
        ultimo = agora;
        if (dt > 0.05) dt = 0.05; /* Nao salta apos travamento do terminal. */

        if (tecla >= 0) {
            if (processar_tecla(&jogo, &historico, &gravidade, tecla,
                                &pausado, &vista, &sair, &alterado, &pausa_desfazer))
                redesenhar = 1;
        }
        if (!jogo.fim && !pausado) {
            if (pausa_desfazer > 0.0) {
                pausa_desfazer -= dt;
                if (pausa_desfazer < 0.0) pausa_desfazer = 0.0;
            } else {
                int resultado = gravidade_atualizar(&jogo, &gravidade, dt);
                if (resultado == GRAVIDADE_MOVEU) {
                    alterado = 1;
                    redesenhar = 1;
                } else if (resultado == GRAVIDADE_FIXOU) {
                    if (!jogo.fim) {
                        pilha_empilhar(&historico, &jogo);
                        alterado = 0;
                    } else alterado = 1;
                    redesenhar = 1;
                }
            }
        }
        if (redesenhar && !sair) {
            desenhar(&jogo, &historico, vista);
            if (pausado) printf("PAUSADO - pressione P para continuar.\n");
            redesenhar = 0;
        }
        dormir_ms(12);
    }
    printf("\033[2J\033[HPartida encerrada. Pontos: %d | Linhas: %d\n",
           jogo.pontos, jogo.linhas);
    return 0;
}
