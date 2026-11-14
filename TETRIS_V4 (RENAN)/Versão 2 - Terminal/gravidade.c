#include "gravidade.h"
#include "movimentacao.h"
#include "tabuleiro.h"

/* Mesma velocidade Normal escolhida para a Versao 3. */
int nivel_atual(const Jogo *j) { return 1 + j->linhas / 10; }

double intervalo_gravidade(const Jogo *j) {
    int nivel = nivel_atual(j);
    double intervalo = 0.58;
    for (int i = 1; i < nivel; i++) intervalo *= 0.88;
    if (intervalo < 0.08) intervalo = 0.08;
    return intervalo;
}

/* Mantida para quem usa o TAD diretamente e para testes legados. */
void aplicar_gravidade(Jogo *j) {
    if (!j->fim && !mover(j, 0, 1)) finalizar_peca(j);
}

void gravidade_reiniciar(EstadoGravidade *g) {
    if (!g) return;
    g->tempo_queda = 0.0;
    g->tempo_contato = 0.0;
}

/* Retorna 0: nenhuma mudanca; 1: desceu; 2: fixou e nasceu outra peca.
 * O atraso para fixar acontece somente quando a peca toca o piso/blocos.
 * Nao espera um intervalo inteiro de gravidade antes de fixar.
 */
int gravidade_atualizar(Jogo *j, EstadoGravidade *g, double delta_segundos) {
    if (!j || !g || j->fim || delta_segundos <= 0) return GRAVIDADE_SEM_MUDANCA;
    if (delta_segundos > 0.05) delta_segundos = 0.05;
    if (!pode_mover(j, 0, 1, j->atual.matriz)) {
        g->tempo_queda = 0;
        g->tempo_contato += delta_segundos;
        if (g->tempo_contato >= 0.17) {
            finalizar_peca(j);
            gravidade_reiniciar(g);
            return GRAVIDADE_FIXOU;
        }
        return GRAVIDADE_SEM_MUDANCA;
    }
    g->tempo_contato = 0.0;
    g->tempo_queda += delta_segundos;
    double intervalo = intervalo_gravidade(j);
    if (g->tempo_queda >= intervalo) {
        g->tempo_queda -= intervalo;
        if (mover(j, 0, 1)) return GRAVIDADE_MOVEU;
    }
    return GRAVIDADE_SEM_MUDANCA;
}
