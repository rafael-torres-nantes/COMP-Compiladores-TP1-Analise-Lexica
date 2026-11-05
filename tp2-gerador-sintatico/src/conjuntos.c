/*
 * conjuntos.c - Calculo de anulavel, FIRST e FOLLOW (etapa 3).
 *
 * Fornecido: conj_iniciar e conj_imprimir.
 *
 * Implementado pelo grupo: conj_anulavel, conj_first e conj_follow. Os tres sao
 * calculados por iteracao ate ponto fixo: repita uma passada sobre todas
 * as regras enquanto algum conjunto (ou flag) mudar. bitset_unir devolve
 * 1 quando o conjunto de destino muda, o que ajuda a detectar o ponto fixo.
 */
#include <stdio.h>
#include <string.h>
#include "conjuntos.h"
#include "util.h"

/* ------------------------------------------------------------------ */
/* Fornecido                                                          */
/* ------------------------------------------------------------------ */

static void iniciar_expr(Expr *e, void *ctx)
{
    int n = *(int *) ctx;
    bitset_liberar(&e->first);
    bitset_liberar(&e->follow);
    bitset_iniciar(&e->first, n);
    bitset_iniciar(&e->follow, n);
    e->anulavel = 0;
}

void conj_iniciar(Gramatica *g)
{
    int i, n = g->nterms;
    for (i = 0; i < g->nnts; i++) {
        bitset_liberar(&g->nts[i].first);
        bitset_liberar(&g->nts[i].follow);
        bitset_iniciar(&g->nts[i].first, n);
        bitset_iniciar(&g->nts[i].follow, n);
        g->nts[i].anulavel = 0;
        expr_percorrer(g->nts[i].regra, iniciar_expr, &n);
    }
}

void conj_imprimir(FILE *f, const Gramatica *g)
{
    int i;
    size_t largura = strlen("NAO TERMINAL");
    for (i = 0; i < g->nnts; i++)
        if (strlen(g->nts[i].nome) > largura)
            largura = strlen(g->nts[i].nome);
    fprintf(f, "%-*s  ANULAVEL  FIRST / FOLLOW\n", (int) largura, "NAO TERMINAL");
    for (i = 0; i < g->nnts; i++) {
        fprintf(f, "%-*s  %-8s  FIRST  = ", (int) largura, g->nts[i].nome,
                g->nts[i].anulavel ? "sim" : "nao");
        gram_imprimir_conjunto(f, g, &g->nts[i].first);
        fprintf(f, "\n%-*s            FOLLOW = ", (int) largura, "");
        gram_imprimir_conjunto(f, g, &g->nts[i].follow);
        fputc('\n', f);
    }
}

/* ------------------------------------------------------------------ */
/* A implementar                                                      */
/* ------------------------------------------------------------------ */

/* Os tres calculos sao pontos fixos: cada funcao *_expr faz uma passada
 * pela arvore e devolve 1 se algum valor mudou. */

static int anulavel_expr(Gramatica *g, Expr *e)
{
    int i, mudou = 0, v;

    for (i = 0; i < e->nfilhos; i++)
        mudou |= anulavel_expr(g, e->filhos[i]);
    switch (e->tipo) {
    case E_TERM:
        v = 0;
        break;
    case E_NAOTERM:
        v = g->nts[e->simbolo].anulavel;
        break;
    case E_SEQ:
        v = 1;
        for (i = 0; i < e->nfilhos; i++)
            v = v && e->filhos[i]->anulavel;
        break;
    case E_ALT:
        v = 0;
        for (i = 0; i < e->nfilhos; i++)
            v = v || e->filhos[i]->anulavel;
        break;
    default:            /* E_OPC, E_REP, E_VAZIO */
        v = 1;
        break;
    }
    if (v != e->anulavel) {
        e->anulavel = v;
        mudou = 1;
    }
    return mudou;
}

void conj_anulavel(Gramatica *g)
{
    int i, mudou = 1;

    while (mudou) {
        mudou = 0;
        for (i = 0; i < g->nnts; i++) {
            mudou |= anulavel_expr(g, g->nts[i].regra);
            if (g->nts[i].anulavel != g->nts[i].regra->anulavel) {
                g->nts[i].anulavel = g->nts[i].regra->anulavel;
                mudou = 1;
            }
        }
    }
}

static int first_expr(Gramatica *g, Expr *e)
{
    int i, mudou = 0;

    for (i = 0; i < e->nfilhos; i++)
        mudou |= first_expr(g, e->filhos[i]);
    switch (e->tipo) {
    case E_TERM:
        if (!bitset_tem(&e->first, e->simbolo)) {
            bitset_add(&e->first, e->simbolo);
            mudou = 1;
        }
        break;
    case E_NAOTERM:
        mudou |= bitset_unir(&e->first, &g->nts[e->simbolo].first);
        break;
    case E_SEQ:
        for (i = 0; i < e->nfilhos; i++) {
            mudou |= bitset_unir(&e->first, &e->filhos[i]->first);
            if (!e->filhos[i]->anulavel)
                break;
        }
        break;
    default:            /* E_ALT, E_OPC, E_REP; E_VAZIO nao tem filhos */
        for (i = 0; i < e->nfilhos; i++)
            mudou |= bitset_unir(&e->first, &e->filhos[i]->first);
        break;
    }
    return mudou;
}

void conj_first(Gramatica *g)
{
    int i, mudou = 1;

    while (mudou) {
        mudou = 0;
        for (i = 0; i < g->nnts; i++) {
            mudou |= first_expr(g, g->nts[i].regra);
            mudou |= bitset_unir(&g->nts[i].first, &g->nts[i].regra->first);
        }
    }
}

/* Passa o follow de e para os filhos e desce. */
static int follow_expr(Gramatica *g, Expr *e)
{
    int i, mudou = 0;
    Bitset resto;

    switch (e->tipo) {
    case E_NAOTERM:
        mudou |= bitset_unir(&g->nts[e->simbolo].follow, &e->follow);
        break;
    case E_SEQ:
        /* De tras para frente: resto e o que pode vir depois do filho i
         * (FIRST do restante, mais follow(seq) enquanto ele for anulavel). */
        bitset_iniciar(&resto, g->nterms);
        bitset_copiar(&resto, &e->follow);
        for (i = e->nfilhos - 1; i >= 0; i--) {
            mudou |= bitset_unir(&e->filhos[i]->follow, &resto);
            if (e->filhos[i]->anulavel)
                bitset_unir(&resto, &e->filhos[i]->first);
            else
                bitset_copiar(&resto, &e->filhos[i]->first);
        }
        bitset_liberar(&resto);
        break;
    case E_REP:
        /* depois de uma volta pode vir outra */
        mudou |= bitset_unir(&e->filhos[0]->follow, &e->filhos[0]->first);
        mudou |= bitset_unir(&e->filhos[0]->follow, &e->follow);
        break;
    default:            /* E_ALT, E_OPC; E_TERM e E_VAZIO nao tem filhos */
        for (i = 0; i < e->nfilhos; i++)
            mudou |= bitset_unir(&e->filhos[i]->follow, &e->follow);
        break;
    }
    for (i = 0; i < e->nfilhos; i++)
        mudou |= follow_expr(g, e->filhos[i]);
    return mudou;
}

void conj_follow(Gramatica *g)
{
    int i, mudou = 1;

    bitset_add(&g->nts[g->inicial].follow, TERM_FIM);
    while (mudou) {
        mudou = 0;
        for (i = 0; i < g->nnts; i++) {
            mudou |= bitset_unir(&g->nts[i].regra->follow, &g->nts[i].follow);
            mudou |= follow_expr(g, g->nts[i].regra);
        }
    }
}
