aluno: 202019050169
aluno: 202419050093
uso_ia: sim

# Trabalho de Compiladores I - Parte 2

# Trabalho Prático 2: Gerador de Analisadores Sintáticos Descendentes Recursivos a partir de EBNF

Repositório do Trabalho Prático 2 da disciplina **Compiladores I (FACOM/UFMS, 2026/2)**: uma
ferramenta em **ANSI C (C89/C90)** que lê uma gramática em EBNF e gera o código-fonte de um
analisador sintático descendente recursivo, integrado ao scanner de Micro C do TP1. Construída
sobre o código-base `tp2-gerador-ebnf.zip` do professor.

> Status: todos os `TODO(aluno)` estão implementados, `make` compila sem avisos,
> `make test` passa 35 de 35 e `sh testes/prova_de_fogo.sh` passa 51 de 51.

## Como compilar e usar

Requer `gcc`, `make`, `flex` e `diff` (nesta máquina, no MSYS2).

```bash
make                          # compila ./gerador e build/microc/parser_microc
make test                     # testes de testes/ (35)
sh testes/prova_de_fogo.sh    # bordas, regex diferencial, mutação e entradas hostis (51)
make clean
```

```bash
./gerador testes/gramaticas/expr.ebnf -o build/expr --tree
gcc -ansi -pedantic -Wall -Wextra -o build/expr_bin build/expr.c
./build/expr_bin --tree programa.txt     # ou: ./build/expr_bin --tokens tokens.txt
```

Opções do gerador: `-o prefixo` grava `prefixo.c` e `prefixo.h`, `--sets` imprime anulável,
FIRST e FOLLOW, `--debug` imprime a gramática lida, `--tree` inclui a impressão da árvore de
derivação e `--scanner microc` usa o scanner do TP1 (`TOKENS EXTERN "tokens.h" ;`).

## Pipeline

1. Leitura da EBNF (`ebnf_lexer.c`, `ebnf_parser.c`)
2. Verificação semântica (`verificacao.c`)
3. Anulável, FIRST e FOLLOW (`conjuntos.c`), e recursão à esquerda
4. Verificação LL(1) (`ll1.c`)
5. Geração de código (`codegen.c`), com o léxico de cada regex vindo de `automato.c`

O scanner do TP1 adaptado mora em [`microc/`](microc/).

## Documentação

- [`USO_IA.md`](USO_IA.md): registro obrigatório de uso de IA.
