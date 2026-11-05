/*
 * microc_mapa.c - Correspondencia com os tokens de Micro C (ver microc_mapa.h).
 *
 * A tabela de literais segue o tokens.h do livro-texto (secao 2.3.1):
 *
 *   UNDEF, ID, END_OF_FILE, INTEGERCONST, CHARCONST, STRINGCONST,
 *   PLUS, MINUS, MUL, DIV, MOD, EQ, NEQ, LT, GT, LEQ, GEQ, AND, OR, NOT,
 *   ASSIGN, SEMICOLON, COMMA, LPAREN, RPAREN, LBRACE, RBRACE,
 *   LBRACKET, RBRACKET, MAIN, IF, ELSE, FOR, RETURN, INT, CHAR, PRINT
 *
 * Se o seu tokens.h do TP1 usar outros nomes, ajuste a tabela.
 *
 * Implementado pelo grupo: microc_carregar_tokens_h.
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "microc_mapa.h"
#include "util.h"

static const char *const literais[][2] = {
    { "+",  "PLUS" },      { "-",  "MINUS" },     { "*",  "MUL" },
    { "/",  "DIV" },       { "%",  "MOD" },
    { "==", "EQ" },        { "!=", "NEQ" },       { "<",  "LT" },
    { ">",  "GT" },        { "<=", "LEQ" },       { ">=", "GEQ" },
    { "&&", "AND" },       { "||", "OR" },        { "!",  "NOT" },
    { "=",  "ASSIGN" },    { ";",  "SEMICOLON" }, { ",",  "COMMA" },
    { "(",  "LPAREN" },    { ")",  "RPAREN" },    { "{",  "LBRACE" },
    { "}",  "RBRACE" },    { "[",  "LBRACKET" },  { "]",  "RBRACKET" },
    { "main", "MAIN" },    { "if", "IF" },        { "else", "ELSE" },
    { "for", "FOR" },      { "return", "RETURN" },{ "int", "INT" },
    { "char", "CHAR" },    { "print", "PRINT" }
};

static char **nomes = NULL;
static int nnomes = 0, capnomes = 0;

const char *microc_nome_literal(const char *literal)
{
    size_t i;
    for (i = 0; i < sizeof(literais) / sizeof(literais[0]); i++)
        if (strcmp(literais[i][0], literal) == 0)
            return literais[i][1];
    return NULL;
}

static void adicionar_nome(const char *s, size_t n)
{
    if (nnomes == capnomes) {
        capnomes = capnomes ? 2 * capnomes : 64;
        nomes = (char **) xrealloc(nomes, (size_t) capnomes * sizeof(char *));
    }
    nomes[nnomes++] = xstrndup(s, n);
}

/* Le os nomes do corpo de um enum: o primeiro identificador depois de
 * '{' ou ',' e um nome; o que vem depois de '=' e comentario e ignorado. */
static void ler_nomes(const char *s, const char *fim)
{
    int esperando = 1;
    const char *inicio;

    while (s < fim) {
        if (s[0] == '/' && s[1] == '*') {
            s = strstr(s + 2, "*/");
            s = s != NULL ? s + 2 : fim;
        } else if (s[0] == '/' && s[1] == '/') {
            while (s < fim && *s != '\n')
                s++;
        } else if (isalpha((unsigned char) *s) || *s == '_') {
            for (inicio = s; s < fim && (isalnum((unsigned char) *s) || *s == '_'); s++)
                ;
            if (esperando)
                adicionar_nome(inicio, (size_t) (s - inicio));
            esperando = 0;
        } else {
            esperando = esperando || *s == ',';
            s++;
        }
    }
}

int microc_carregar_tokens_h(const char *caminho)
{
    char *texto = caminho != NULL ? ler_arquivo(caminho) : NULL;
    const char *p, *abre, *fecha;

    if (texto == NULL) {
        diag_aviso(0, 0, "nao foi possivel ler %s: nomes de TokenType nao verificados",
                   caminho != NULL ? caminho : "tokens.h");
        return -1;
    }
    for (p = texto; (p = strstr(p, "typedef enum")) != NULL; p = fecha + 1) {
        abre = strchr(p, '{');
        fecha = abre != NULL ? strchr(abre, '}') : NULL;
        if (fecha == NULL)
            break;
        for (p = fecha + 1; isspace((unsigned char) *p); p++)
            ;
        if (strncmp(p, "TokenType", 9) == 0 && !isalnum((unsigned char) p[9]) && p[9] != '_') {
            ler_nomes(abre + 1, fecha);
            free(texto);
            return 0;
        }
    }
    diag_aviso(0, 0, "enumeracao TokenType nao encontrada em %s: nomes nao verificados", caminho);
    free(texto);
    return -1;
}

int microc_token_existe(const char *nome)
{
    int i;
    for (i = 0; i < nnomes; i++)
        if (strcmp(nomes[i], nome) == 0)
            return 1;
    return 0;
}

void microc_liberar(void)
{
    int i;
    for (i = 0; i < nnomes; i++)
        free(nomes[i]);
    free(nomes);
    nomes = NULL;
    nnomes = capnomes = 0;
}
