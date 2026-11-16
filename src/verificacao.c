/*
 * verificacao.c - Verificacoes semanticas da gramatica (etapa 2).
 *
 * Ja implementados (use como modelo):
 *   - verif_indefinidos: nao terminais usados mas nao definidos;
 *   - verif_terminais:   terminais nomeados sem declaracao.
 *
 * Implementado pelo grupo:
 *   - verif_inalcancaveis      (aviso)
 *   - verif_improdutivos       (erro)
 *   - verif_recursao_esquerda  (erro; direta e indireta)
 *
 * A duplicidade de definicoes ja e detectada durante a leitura
 * (ebnf_parser.c).
 */
#include <stdlib.h>
#include <string.h>
#include "verificacao.h"
#include "microc_mapa.h"
#include "util.h"

/* ------------------------------------------------------------------ */
/* Fornecido                                                          */
/* ------------------------------------------------------------------ */

static int verif_indefinidos(Gramatica *g)
{
    int i, erros = 0;
    for (i = 0; i < g->nnts; i++) {
        if (g->nts[i].regra == NULL) {
            diag_erro(g->nts[i].linha, g->nts[i].coluna,
                      "nao terminal %s usado mas nao definido", g->nts[i].nome);
            erros++;
        }
    }
    return erros;
}

static int verif_terminais(Gramatica *g, int modo_microc)
{
    int i, erros = 0, carregado = 0;
    Terminal *t;

    if (modo_microc)
        carregado = microc_carregar_tokens_h(g->tokens_externos) == 0;

    for (i = 1; i < g->nterms; i++) {
        t = &g->terms[i];
        if (modo_microc) {
            if (t->literal && microc_nome_literal(t->nome) == NULL) {
                diag_erro(t->linha, t->coluna, "literal %s nao corresponde a nenhum "
                          "token de Micro C", t->exibicao);
                erros++;
            } else if (!t->literal && carregado && !microc_token_existe(t->nome)) {
                diag_erro(t->linha, t->coluna, "terminal %s nao existe na enumeracao "
                          "TokenType de %s", t->nome, g->tokens_externos);
                erros++;
            }
        } else if (!t->declarado) {
            diag_erro(t->linha, t->coluna, "terminal %s nao declarado em TOKENS", t->nome);
            erros++;
        }
    }
    return erros;
}

/* ------------------------------------------------------------------ */
/* Alcancabilidade, produtividade e recursao a esquerda               */
/* ------------------------------------------------------------------ */

/* Marca, em profundidade, cada nao terminal que aparece em e. A regra de
 * um nao terminal indefinido e NULL: o erro dele ja foi reportado. */
static void marcar_alcancaveis(const Gramatica *g, const Expr *e, char *marcado)
{
    int i;
    if (e == NULL)
        return;
    if (e->tipo == E_NAOTERM && !marcado[e->simbolo]) {
        marcado[e->simbolo] = 1;
        marcar_alcancaveis(g, g->nts[e->simbolo].regra, marcado);
    }
    for (i = 0; i < e->nfilhos; i++)
        marcar_alcancaveis(g, e->filhos[i], marcado);
}

/* So emite avisos. */
static void verif_inalcancaveis(Gramatica *g)
{
    int i;
    char *marcado = (char *) xcalloc((size_t) g->nnts, 1);

    marcado[g->inicial] = 1;
    marcar_alcancaveis(g, g->nts[g->inicial].regra, marcado);
    for (i = 0; i < g->nnts; i++)
        if (!marcado[i])
            diag_aviso(g->nts[i].linha, g->nts[i].coluna, "nao terminal %s inalcancavel a partir de %s",
                       g->nts[i].nome, g->nts[g->inicial].nome);
    free(marcado);
}

static int produtiva(const Expr *e, const char *produtivo)
{
    int i;
    switch (e->tipo) {
    case E_NAOTERM:
        return produtivo[e->simbolo];
    case E_SEQ:
        for (i = 0; i < e->nfilhos; i++)
            if (!produtiva(e->filhos[i], produtivo))
                return 0;
        return 1;
    case E_ALT:
        for (i = 0; i < e->nfilhos; i++)
            if (produtiva(e->filhos[i], produtivo))
                return 1;
        return 0;
    default:            /* E_TERM, E_OPC, E_REP, E_VAZIO */
        return 1;
    }
}

static int verif_improdutivos(Gramatica *g)
{
    int i, mudou = 1, erros = 0;
    char *produtivo = (char *) xcalloc((size_t) g->nnts, 1);

    while (mudou) {
        mudou = 0;
        for (i = 0; i < g->nnts; i++)
            if (!produtivo[i] && produtiva(g->nts[i].regra, produtivo))
                produtivo[i] = mudou = 1;
    }
    for (i = 0; i < g->nnts; i++)
        if (!produtivo[i]) {
            diag_erro(g->nts[i].linha_def, 0, "nao terminal %s improdutivo: nao deriva nenhuma "
                      "cadeia de terminais", g->nts[i].nome);
            erros++;
        }
    free(produtivo);
    return erros;
}

/* esquerda[b] = 1 para cada nao terminal b que pode iniciar e sem que
 * nenhum token seja consumido antes (inclusive depois de prefixo anulavel). */
static void coletar_esquerda(const Expr *e, char *esquerda)
{
    int i;
    switch (e->tipo) {
    case E_NAOTERM:
        esquerda[e->simbolo] = 1;
        break;
    case E_SEQ:
        for (i = 0; i < e->nfilhos; i++) {
            coletar_esquerda(e->filhos[i], esquerda);
            if (!e->filhos[i]->anulavel)
                break;
        }
        break;
    default:            /* E_ALT, E_OPC, E_REP; E_TERM e E_VAZIO nao tem filhos */
        for (i = 0; i < e->nfilhos; i++)
            coletar_esquerda(e->filhos[i], esquerda);
        break;
    }
}

int verif_recursao_esquerda(Gramatica *g)
{
    int n = g->nnts, a, b, k, erros = 0;
    char *alcanca = (char *) xcalloc((size_t) n * (size_t) n, 1);

    for (a = 0; a < n; a++)
        coletar_esquerda(g->nts[a].regra, alcanca + a * n);
    /* Fecho transitivo (Warshall): alcanca[a][b] se a chega a b pela esquerda. */
    for (k = 0; k < n; k++)
        for (a = 0; a < n; a++)
            if (alcanca[a * n + k])
                for (b = 0; b < n; b++)
                    if (alcanca[k * n + b])
                        alcanca[a * n + b] = 1;
    for (a = 0; a < n; a++) {
        if (!alcanca[a * n + a])
            continue;
        /* refaz so a linha direta de a para separar direta de indireta */
        memset(alcanca + a * n, 0, (size_t) n);
        coletar_esquerda(g->nts[a].regra, alcanca + a * n);
        diag_erro(g->nts[a].linha_def, 0, "nao terminal %s tem recursao a esquerda %s",
                  g->nts[a].nome, alcanca[a * n + a] ? "direta" : "indireta");
        erros++;
    }
    free(alcanca);
    return erros;
}

int verif_basica(Gramatica *g, int modo_microc)
{
    int erros = 0;
    if (g->inicial < 0) {
        diag_erro(0, 0, "a gramatica nao tem nenhuma regra");
        return 1;
    }
    erros += verif_indefinidos(g);
    erros += verif_terminais(g, modo_microc);
    verif_inalcancaveis(g);
    if (erros == 0)
        erros += verif_improdutivos(g);
    return erros;
}
