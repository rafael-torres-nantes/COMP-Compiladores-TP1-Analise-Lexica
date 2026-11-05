/*
 * tokens.h
 *
 * Vocabulario de tokens de Micro C, compartilhado entre o scanner do TP1
 * (microc.flex) e o analisador sintatico gerado no TP2. Extraido do
 * microc.flex do TP1 sem mudar a ordem nem os valores.
 */

#ifndef TOKENS_H
#define TOKENS_H

typedef enum {
    /* Tokens fundamentais */
    UNDEF,          /* token indefinido (usado para reportar erros) */
    ID,             /* identificador                                */
    END_OF_FILE,    /* fim de arquivo                                */

    /* Constantes literais */
    INTEGERCONST,
    CHARCONST,
    STRINGCONST,

    /* Operadores aritmeticos */
    PLUS, MINUS, MUL, DIV, MOD,

    /* Operadores relacionais e logicos */
    EQ, NEQ, LT, GT, LEQ, GEQ, AND, OR, NOT,

    /* Simbolos de atribuicao e pontuacao */
    ASSIGN, SEMICOLON, COMMA, LPAREN, RPAREN,
    LBRACE, RBRACE, LBRACKET, RBRACKET,

    /* Palavras reservadas */
    MAIN, IF, ELSE, FOR, RETURN, INT, CHAR, PRINT
} TokenType;

/* Token entregue ao analisador sintatico (livro-texto, secao 2.3.1). */
typedef struct {
    TokenType tipo;
    char lexema[100];
    int linha;
} Token;

/* Valor semantico do token corrente. */
typedef struct {
    char *symbol;      /* lexema para ID, INTEGERCONST, CHARCONST, STRINGCONST */
    char *error_msg;   /* mensagem de erro, usada apenas quando tipo == UNDEF   */
} YYSTYPE;

#endif
