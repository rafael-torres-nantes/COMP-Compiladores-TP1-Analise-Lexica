/*
 * automato.h - Expressao regular de TOKENS para automato finito
 * deterministico minimo: AFN de Thompson, construcao de subconjuntos e
 * minimizacao de Moore. Usado por gen_casamento_regex (codegen.c).
 */
#ifndef AUTOMATO_H
#define AUTOMATO_H

typedef struct {
    int n_estados;
    int *transicao;     /* transicao[estado * 256 + c] = destino, ou -1 */
    char *final;        /* final[estado] = 1 se o estado aceita          */
} Afd;

/* Devolve o automato minimo, com o estado 0 inicial, ou NULL com a causa
 * em mensagem_erro (ao menos 128 bytes). Recusa expressao que aceita a
 * cadeia vazia, porque um token nunca pode ter comprimento zero. */
Afd *compila_regex(const char *regex, char *mensagem_erro);
void afd_libera(Afd *a);

#endif
