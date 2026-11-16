/*
 * automato.c
 *
 * Transforma a expressao regular de um token em automato finito
 * deterministico minimo, pelas tres construcoes classicas (slide 14):
 *
 *   expressao regular -> AFN (Thompson) -> AFD (subconjuntos) -> AFD minimo
 *
 * Sintaxe aceita: caractere, \x (escape), . (qualquer um menos \n),
 * [abc], [a-z], [^...], ( ), |, *, + e ?.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "automato.h"
#include "util.h"

#define ALFABETO 256

/* ---------------------------------------------------------------------
 * AFN de Thompson
 * ------------------------------------------------------------------- */

typedef struct {
    int eps[2];                     /* transicoes vazias, -1 se ausentes */
    int destino;                    /* destino da transicao por classe   */
    unsigned char classe[ALFABETO]; /* classe[c] = 1 se c leva a destino */
} EstadoAfn;

typedef struct {
    EstadoAfn *e;
    int n, cap;
} Afn;

typedef struct {
    int inicio, fim;                /* fim ainda nao tem saidas */
} Fragmento;

typedef struct {
    const char *s;
    int pos;
    Afn *afn;
    char *mensagem;
    int falhou;
} Compilador;

static int novo_estado(Afn *a)
{
    EstadoAfn *e;
    if (a->n == a->cap) {
        a->cap = a->cap ? a->cap * 2 : 32;
        a->e = (EstadoAfn *) xrealloc(a->e, (size_t) a->cap * sizeof(EstadoAfn));
    }
    e = &a->e[a->n];
    memset(e, 0, sizeof(*e));
    e->eps[0] = e->eps[1] = -1;
    e->destino = -1;
    return a->n++;
}

static void liga(Afn *a, int de, int para)
{
    if (a->e[de].eps[0] < 0)
        a->e[de].eps[0] = para;
    else
        a->e[de].eps[1] = para;
}

static void falha(Compilador *c, const char *mensagem)
{
    if (!c->falhou) {
        sprintf(c->mensagem, "%s (posicao %d)", mensagem, c->pos + 1);
        c->falhou = 1;
    }
}

static Fragmento fragmento_classe(Compilador *c, const unsigned char *classe)
{
    Fragmento f;
    f.inicio = novo_estado(c->afn);
    f.fim = novo_estado(c->afn);
    memcpy(c->afn->e[f.inicio].classe, classe, ALFABETO);
    c->afn->e[f.inicio].destino = f.fim;
    return f;
}

static Fragmento fragmento_vazio(Compilador *c)
{
    Fragmento f;
    f.inicio = f.fim = novo_estado(c->afn);
    return f;
}

static int escape(int ch)
{
    switch (ch) {
    case 'n': return '\n';
    case 't': return '\t';
    case 'r': return '\r';
    case 'f': return '\f';
    case 'v': return '\v';
    default:  return ch;
    }
}

static int atual(const Compilador *c)
{
    return (unsigned char) c->s[c->pos];
}

/* Le um caractere de uma classe [ ], tratando escape. */
static int le_caractere_classe(Compilador *c)
{
    int ch = atual(c);
    if (ch == '\\' && c->s[c->pos + 1] != '\0') {
        c->pos++;
        ch = escape(atual(c));
    }
    c->pos++;
    return ch;
}

static Fragmento classe(Compilador *c)
{
    unsigned char conjunto[ALFABETO];
    int negada = 0, primeiro = 1, i;

    memset(conjunto, 0, sizeof(conjunto));
    c->pos++;                                   /* '[' */
    if (atual(c) == '^') {
        negada = 1;
        c->pos++;
    }
    while (atual(c) != ']' || primeiro) {
        int de, ate;
        if (atual(c) == '\0') {
            falha(c, "classe [ ] nao fechada");
            return fragmento_vazio(c);
        }
        de = le_caractere_classe(c);
        ate = de;
        if (atual(c) == '-' && c->s[c->pos + 1] != ']' && c->s[c->pos + 1] != '\0') {
            c->pos++;
            ate = le_caractere_classe(c);
            if (ate < de) {
                falha(c, "intervalo invertido em [ ]");
                return fragmento_vazio(c);
            }
        }
        for (i = de; i <= ate; i++)
            conjunto[i] = 1;
        primeiro = 0;
    }
    c->pos++;                                   /* ']' */
    if (negada) {
        for (i = 0; i < ALFABETO; i++)
            conjunto[i] = (unsigned char) !conjunto[i];
    }
    conjunto[0] = 0;                            /* '\0' marca o fim da entrada */
    return fragmento_classe(c, conjunto);
}

static Fragmento alternativa(Compilador *c);

static Fragmento atomo(Compilador *c)
{
    unsigned char conjunto[ALFABETO];
    int ch = atual(c);
    Fragmento f;

    memset(conjunto, 0, sizeof(conjunto));
    switch (ch) {
    case '(':
        c->pos++;
        f = alternativa(c);
        if (atual(c) != ')') {
            falha(c, "parentese nao fechado");
            return f;
        }
        c->pos++;
        return f;
    case '[':
        return classe(c);
    case '.':
        c->pos++;
        memset(conjunto, 1, sizeof(conjunto));
        conjunto[0] = 0;
        conjunto['\n'] = 0;
        return fragmento_classe(c, conjunto);
    case '*':
    case '+':
    case '?':
        falha(c, "operador de repeticao sem operando");
        return fragmento_vazio(c);
    case '\\':
        if (c->s[c->pos + 1] == '\0') {
            falha(c, "barra invertida no fim da expressao");
            return fragmento_vazio(c);
        }
        c->pos++;
        ch = escape(atual(c));
        break;
    default:
        break;
    }
    c->pos++;
    conjunto[ch] = 1;
    return fragmento_classe(c, conjunto);
}

static Fragmento repeticao(Compilador *c)
{
    Fragmento f = atomo(c), g;
    Afn *a = c->afn;

    while (!c->falhou && (atual(c) == '*' || atual(c) == '+' || atual(c) == '?')) {
        int op = atual(c);
        c->pos++;
        g.inicio = novo_estado(a);
        g.fim = novo_estado(a);
        liga(a, g.inicio, f.inicio);
        liga(a, f.fim, g.fim);
        if (op == '*' || op == '?')
            liga(a, g.inicio, g.fim);
        if (op == '*' || op == '+')
            liga(a, f.fim, f.inicio);
        f = g;
    }
    return f;
}

static Fragmento concatenacao(Compilador *c)
{
    Fragmento f, g;
    int vazio = 1;

    f.inicio = f.fim = -1;
    while (!c->falhou && atual(c) != '\0' && atual(c) != '|' && atual(c) != ')') {
        g = repeticao(c);
        if (vazio) {
            f = g;
            vazio = 0;
        } else {
            liga(c->afn, f.fim, g.inicio);
            f.fim = g.fim;
        }
    }
    return vazio ? fragmento_vazio(c) : f;
}

static Fragmento alternativa(Compilador *c)
{
    Fragmento f = concatenacao(c), g, h;
    Afn *a = c->afn;

    while (!c->falhou && atual(c) == '|') {
        c->pos++;
        g = concatenacao(c);
        h.inicio = novo_estado(a);
        h.fim = novo_estado(a);
        liga(a, h.inicio, f.inicio);
        liga(a, h.inicio, g.inicio);
        liga(a, f.fim, h.fim);
        liga(a, g.fim, h.fim);
        f = h;
    }
    return f;
}

/* ---------------------------------------------------------------------
 * AFD pela construcao de subconjuntos
 * ------------------------------------------------------------------- */

static void fecho(const Afn *a, unsigned char *conjunto, int *pilha)
{
    int topo = 0, i;
    for (i = 0; i < a->n; i++)
        if (conjunto[i])
            pilha[topo++] = i;
    while (topo > 0) {
        int s = pilha[--topo], k;
        for (k = 0; k < 2; k++) {
            int t = a->e[s].eps[k];
            if (t >= 0 && !conjunto[t]) {
                conjunto[t] = 1;
                pilha[topo++] = t;
            }
        }
    }
}

static Afd *subconjuntos(const Afn *a, int inicio, int final)
{
    int n = a->n, cap = 16, n_estados = 0, i, c;
    unsigned char **conjuntos = (unsigned char **) xcalloc(1, (size_t) cap * sizeof(unsigned char *));
    int *transicao = (int *) xcalloc(1, (size_t) cap * ALFABETO * sizeof(int));
    int *pilha = (int *) xcalloc(1, (size_t) n * 2 * sizeof(int) + sizeof(int));
    unsigned char *novo = (unsigned char *) xcalloc(1, (size_t) n);
    Afd *afd;

    conjuntos[0] = (unsigned char *) xcalloc(1, (size_t) n);
    conjuntos[0][inicio] = 1;
    fecho(a, conjuntos[0], pilha);
    n_estados = 1;

    for (i = 0; i < n_estados; i++) {
        for (c = 1; c < ALFABETO; c++) {
            int s, algum = 0, j;
            memset(novo, 0, (size_t) n);
            for (s = 0; s < n; s++) {
                if (conjuntos[i][s] && a->e[s].destino >= 0 && a->e[s].classe[c]) {
                    novo[a->e[s].destino] = 1;
                    algum = 1;
                }
            }
            if (!algum) {
                transicao[i * ALFABETO + c] = -1;
                continue;
            }
            fecho(a, novo, pilha);
            for (j = 0; j < n_estados; j++)
                if (memcmp(conjuntos[j], novo, (size_t) n) == 0)
                    break;
            if (j == n_estados) {
                if (n_estados == cap) {
                    cap *= 2;
                    conjuntos = (unsigned char **) xrealloc(conjuntos, (size_t) cap * sizeof(unsigned char *));
                    transicao = (int *) xrealloc(transicao, (size_t) cap * ALFABETO * sizeof(int));
                }
                conjuntos[n_estados] = (unsigned char *) xcalloc(1, (size_t) n);
                memcpy(conjuntos[n_estados], novo, (size_t) n);
                n_estados++;
            }
            transicao[i * ALFABETO + c] = j;
        }
        transicao[i * ALFABETO + 0] = -1;
    }

    afd = (Afd *) xcalloc(1, sizeof(Afd));
    afd->n_estados = n_estados;
    afd->transicao = transicao;
    afd->final = (char *) xcalloc(1, (size_t) n_estados);
    for (i = 0; i < n_estados; i++) {
        afd->final[i] = (char) conjuntos[i][final];
        free(conjuntos[i]);
    }
    free(conjuntos);
    free(pilha);
    free(novo);
    return afd;
}

/* ---------------------------------------------------------------------
 * Minimizacao por refinamento de particoes (Moore)
 *
 * Dois estados ficam no mesmo bloco enquanto forem ambos finais ou ambos
 * nao finais e, para todo caractere, levarem ao mesmo bloco (ou ambos a
 * lugar nenhum). Repete-se o refinamento ate nada mudar.
 * ------------------------------------------------------------------- */

static int equivalentes(const Afd *a, const int *bloco, int p, int q)
{
    int c;
    if (bloco[p] != bloco[q])
        return 0;
    for (c = 1; c < ALFABETO; c++) {
        int tp = a->transicao[p * ALFABETO + c];
        int tq = a->transicao[q * ALFABETO + c];
        if ((tp < 0 ? -1 : bloco[tp]) != (tq < 0 ? -1 : bloco[tq]))
            return 0;
    }
    return 1;
}

static Afd *minimiza(Afd *a)
{
    int n = a->n_estados, i, j, n_blocos = 0, anterior = -1;
    int *bloco = (int *) xcalloc(1, (size_t) n * sizeof(int));
    int *novo = (int *) xcalloc(1, (size_t) n * sizeof(int));
    int c;
    Afd *m;

    for (i = 0; i < n; i++)
        bloco[i] = a->final[i] ? 1 : 0;

    /* Cada rodada so divide blocos; quando o numero deles nao cresce, a
     * particao estabilizou. */
    while (n_blocos != anterior) {
        anterior = n_blocos;
        n_blocos = 0;
        for (i = 0; i < n; i++) {
            novo[i] = -1;
            for (j = 0; j < i; j++) {
                if (equivalentes(a, bloco, i, j)) {
                    novo[i] = novo[j];
                    break;
                }
            }
            if (novo[i] < 0)
                novo[i] = n_blocos++;
        }
        memcpy(bloco, novo, (size_t) n * sizeof(int));
    }

    /* O estado 0 e o primeiro a ser numerado no refinamento, entao cai no
     * bloco 0: o estado 0 do automato minimo continua sendo o inicial. Os
     * blocos sao contiguos (0..n_blocos-1) e todo estado do AFD e alcancavel. */
    m = (Afd *) xcalloc(1, sizeof(Afd));
    m->n_estados = n_blocos;
    m->transicao = (int *) xcalloc(1, (size_t) n_blocos * ALFABETO * sizeof(int));
    m->final = (char *) xcalloc(1, (size_t) n_blocos);
    for (i = 0; i < n; i++) {
        int b = bloco[i];
        m->final[b] = a->final[i];
        for (c = 0; c < ALFABETO; c++) {
            int t = a->transicao[i * ALFABETO + c];
            m->transicao[b * ALFABETO + c] = t < 0 ? -1 : bloco[t];
        }
    }

    free(bloco);
    free(novo);
    afd_libera(a);
    return m;
}

/* ---------------------------------------------------------------------
 * Interface
 * ------------------------------------------------------------------- */

Afd *compila_regex(const char *regex, char *mensagem_erro)
{
    Afn afn;
    Compilador c;
    Fragmento f;
    Afd *afd;

    memset(&afn, 0, sizeof(afn));
    c.s = regex;
    c.pos = 0;
    c.afn = &afn;
    c.mensagem = mensagem_erro;
    c.falhou = 0;
    mensagem_erro[0] = '\0';

    if (regex[0] == '\0') {
        strcpy(mensagem_erro, "expressao regular vazia");
        return NULL;
    }
    f = alternativa(&c);
    if (!c.falhou && atual(&c) == ')')
        falha(&c, "parentese ')' sem '(' correspondente");
    if (c.falhou) {
        free(afn.e);
        return NULL;
    }

    afd = minimiza(subconjuntos(&afn, f.inicio, f.fim));
    free(afn.e);
    if (afd->final[0]) {
        strcpy(mensagem_erro, "a expressao aceita a cadeia vazia");
        afd_libera(afd);
        return NULL;
    }
    return afd;
}

void afd_libera(Afd *a)
{
    if (a == NULL)
        return;
    free(a->transicao);
    free(a->final);
    free(a);
}
