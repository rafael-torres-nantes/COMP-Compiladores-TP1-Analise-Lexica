/*
 * ll1.c - Verificacao da condicao LL(1) (etapa 4).
 *
 * Condicoes (secao 3 do enunciado):
 *   1. Em a1 | a2 | ... | an, os FIRST das alternativas sao disjuntos dois
 *      a dois e no maximo uma alternativa e anulavel.
 *   2. Se uma alternativa ai e anulavel, FOLLOW(alternativa) e disjunto
 *      dos FIRST das demais.
 *   3. Em [ a ] e { a }, a nao e anulavel e FIRST(a) e disjunto do
 *      FOLLOW da construcao.
 *
 * Violacoes de 1, 2 e da primeira parte de 3 sao ERROS. A violacao de
 * FIRST(a) inter FOLLOW nao vazio em [ a ] ou { a } e reportada como
 * AVISO e resolvida de forma gulosa: o analisador gerado entra na
 * construcao sempre que o token estiver em FIRST(a). E o caso classico
 * do "else pendente":  if = "if" "(" expr ")" cmd [ "else" cmd ] ;
 * em que o else fica com o if mais proximo, como em C.
 *
 * Fornecido: reportar (formata as mensagens).
 * Implementado pelo grupo: verificar_expr e ll1_verificar.
 */
#include <stdarg.h>
#include <stdio.h>
#include "ll1.h"
#include "util.h"

/* Reporta um conflito na regra do nao terminal nt, na posicao de e.
 * comum (pode ser NULL) e o conjunto de tokens em conflito. */
static void reportar(const Gramatica *g, int nt, const Expr *e, int eh_aviso,
                     const Bitset *comum, const char *fmt, ...)
{
    va_list ap;
    fprintf(stderr, "%s:%d:%d: %s: conflito LL(1) em %s: ", g->arquivo, e->linha,
            e->coluna, eh_aviso ? "aviso" : "erro", g->nts[nt].nome);
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    if (comum != NULL) {
        fprintf(stderr, " ");
        gram_imprimir_conjunto(stderr, g, comum);
    }
    fputc('\n', stderr);
}

/* Verifica e e seus descendentes; devolve o numero de erros. */
static int verificar_expr(const Gramatica *g, int nt, const Expr *e, Bitset *comum)
{
    int i, j, erros = 0;
    const char *nome = e->tipo == E_OPC ? "[ ]" : "{ }";

    if (e->tipo == E_ALT) {
        for (i = 0; i < e->nfilhos; i++) {
            for (j = i + 1; j < e->nfilhos; j++) {
                const Expr *a = e->filhos[i], *b = e->filhos[j];
                bitset_intersecao(comum, &a->first, &b->first);
                if (!bitset_vazio(comum)) {
                    reportar(g, nt, e, 0, comum, "alternativas %d e %d comecam com", i + 1, j + 1);
                    erros++;
                }
                if (a->anulavel && b->anulavel) {
                    reportar(g, nt, e, 0, NULL, "alternativas %d e %d sao anulaveis", i + 1, j + 1);
                    erros++;
                }
            }
            /* condicao 2: alternativa anulavel x FIRST das demais */
            if (e->filhos[i]->anulavel)
                for (j = 0; j < e->nfilhos; j++) {
                    bitset_intersecao(comum, &e->follow, &e->filhos[j]->first);
                    if (j != i && !bitset_vazio(comum)) {
                        reportar(g, nt, e, 0, comum, "alternativa %d e anulavel e o FOLLOW "
                                 "intersecta o FIRST da alternativa %d em", i + 1, j + 1);
                        erros++;
                    }
                }
        }
    } else if (e->tipo == E_OPC || e->tipo == E_REP) {
        if (e->filhos[0]->anulavel) {
            reportar(g, nt, e, 0, NULL, "o conteudo de %s e anulavel", nome);
            erros++;
        } else {
            bitset_intersecao(comum, &e->filhos[0]->first, &e->follow);
            if (!bitset_vazio(comum))
                reportar(g, nt, e, 1, comum, "FIRST e FOLLOW de %s se intersectam "
                         "(resolvido de forma gulosa) em", nome);
        }
    }
    for (i = 0; i < e->nfilhos; i++)
        erros += verificar_expr(g, nt, e->filhos[i], comum);
    return erros;
}

int ll1_verificar(const Gramatica *g)
{
    int i, erros = 0;
    Bitset comum;

    bitset_iniciar(&comum, g->nterms);
    for (i = 0; i < g->nnts; i++)
        erros += verificar_expr(g, i, g->nts[i].regra, &comum);
    bitset_liberar(&comum);
    return erros;
}
