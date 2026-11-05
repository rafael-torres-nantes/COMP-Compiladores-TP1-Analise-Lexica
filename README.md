aluno: 202019050169
aluno: 202419050093
uso_ia: sim

# Trabalhos de Compiladores I - Partes 1 e 2

Repositório dos Trabalhos Práticos da disciplina **Compiladores I (FACOM/UFMS)**:

| Trabalho | Tema | Onde |
|----------|------|------|
| **TP1** | Analisador léxico (scanner) para Micro C com **Flex** | raiz do repositório |
| **TP2** | Gerador de analisadores sintáticos descendentes recursivos a partir de EBNF, em **ANSI C** | [`tp2-gerador-sintatico/`](./tp2-gerador-sintatico/) |

[<img src="https://img.shields.io/badge/C-A8B9CC?logo=c&logoColor=black">](https://en.wikipedia.org/wiki/ANSI_C)
[<img src="https://img.shields.io/badge/Flex-3C3C3C?logo=gnu&logoColor=white">](https://github.com/westes/flex)
[<img src="https://img.shields.io/badge/GNU_Make-A42E2B?logo=gnu&logoColor=white">](https://www.gnu.org/software/make/)

## Índice

* [Trabalho Prático 1: Análise Léxica para Micro C](#trabalho-prático-1-análise-léxica-para-micro-c)
* [Trabalho Prático 2: Gerador de Analisadores Sintáticos a partir de EBNF](#trabalho-prático-2-gerador-de-analisadores-sintáticos-a-partir-de-ebnf)
* [📁 Estrutura do repositório](#-estrutura-do-repositório)
* [👤 Autor(es)](#-autores)
* [🎓 Disciplina](#-disciplina)

# Trabalho Prático 1: Análise Léxica para Micro C

Implementação de um **analisador léxico (scanner)** para a linguagem **Micro C**, utilizando a ferramenta **Flex**.

## 📋 Sobre o TP1

O objetivo é completar a especificação léxica de Micro C a partir de um esqueleto fornecido (`microc.flex`), reconhecendo corretamente todos os tokens da linguagem — palavras reservadas, identificadores, constantes, operadores e delimitadores — além de identificar e reportar erros léxicos.

Para cada token reconhecido na entrada, o scanner deve imprimir seu tipo, o lexema correspondente e a linha em que foi encontrado:

```
Token: tipo = INT lexema = 'int' linha = 1
Token: tipo = ID lexema = 'x' linha = 1
Token: tipo = ASSIGN lexema = '=' linha = 1
Token: tipo = INTEGERCONST lexema = '10' linha = 1
Token: tipo = SEMICOLON lexema = ';' linha = 1
```

Erros léxicos (token `UNDEF`) são impressos na saída de erro padrão (`stderr`) no formato:

```
ERRO LEXICO (linha N): <mensagem de erro>
```

## ✅ Itens implementados no TP1

- [x] Reconhecimento das palavras reservadas: `main`, `if`, `else`, `for`, `return`, `int`, `char`, `print` (antes tratadas como `ID`)
- [x] Constantes de caractere (`CHARCONST`) e de string (`STRINGCONST`), com conversão de sequências de escape (`\n`, `\t`, `\\`, `\"`, `\0`) e tratamento dos erros correspondentes
- [x] Operadores relacionais e lógicos com prefixo compartilhado: `!=`, `!`, `<=`, `<`, `>=`, `>`, `&&`, `||` (seguindo o exemplo já implementado para `==` e `=`)
- [x] Tratamento de constantes inteiras negativas, diferenciando-as do operador de subtração
- [x] Ampliação dos arquivos de teste, cobrindo o maior número possível de tokens e de erros léxicos
- [x] Especificação léxica completa (toda entrada deve corresponder a alguma regra, inclusive a regra de erro no final do arquivo)
- [x] Remoção de instruções de depuração (`printf` de teste etc.) antes da entrega

## 🔧 Como compilar e executar o TP1

No diretório raiz do repositório:

```bash
flex microc.flex
gcc lex.yy.c -o lexer
./lexer tests/test.mc
```

O primeiro comando gera o arquivo `lex.yy.c` a partir das regras definidas em `microc.flex`, o segundo compila esse código gerado e o terceiro lê o arquivo Micro C informado, imprime os tokens reconhecidos e reporta eventuais erros léxicos.

# Trabalho Prático 2: Gerador de Analisadores Sintáticos a partir de EBNF

Ferramenta em **ANSI C (C89/C90)** que lê uma gramática em **EBNF** e gera o código-fonte de um **analisador sintático descendente recursivo**, integrado ao scanner de Micro C do TP1. Construída sobre o código-base `tp2-gerador-ebnf.zip` do professor. Detalhes completos em [`tp2-gerador-sintatico/README.md`](./tp2-gerador-sintatico/README.md).

> Status: todos os `TODO(aluno)` estão implementados, `make` compila sem avisos, `make test` passa 35 de 35 e `sh testes/prova_de_fogo.sh` passa 51 de 51.

## 🔀 Pipeline do gerador

1. Leitura da EBNF (`src/ebnf_lexer.c`, `src/ebnf_parser.c`)
2. Verificação semântica (`src/verificacao.c`)
3. Anulável, FIRST e FOLLOW (`src/conjuntos.c`), e detecção de recursão à esquerda
4. Verificação LL(1) (`src/ll1.c`)
5. Geração de código (`src/codegen.c`), com o léxico de cada regex vindo de `src/automato.c`

O scanner do TP1 adaptado para o TP2 fica em [`tp2-gerador-sintatico/microc/`](./tp2-gerador-sintatico/microc/).

## 🔧 Como compilar e testar o TP2

Requer `gcc`, `make`, `flex` e `diff`. Dentro de `tp2-gerador-sintatico/`:

```bash
make                          # compila ./gerador e build/microc/parser_microc
make test                     # testes de testes/ (35)
sh testes/prova_de_fogo.sh    # bordas, regex diferencial, mutação e entradas hostis (51)
make clean
```

Gerando e usando um parser a partir de uma gramática:

```bash
./gerador testes/gramaticas/expr.ebnf -o build/expr --tree
gcc -ansi -pedantic -Wall -Wextra -o build/expr_bin build/expr.c
./build/expr_bin --tree programa.txt     # ou: ./build/expr_bin --tokens tokens.txt
```

Opções do gerador: `-o prefixo` grava `prefixo.c` e `prefixo.h`, `--sets` imprime anulável, FIRST e FOLLOW, `--debug` imprime a gramática lida, `--tree` inclui a impressão da árvore de derivação e `--scanner microc` usa o scanner do TP1 (`TOKENS EXTERN "tokens.h" ;`).

## 📁 Estrutura do repositório

```
.
├── microc.flex                 # TP1: especificação léxica de Micro C
├── tests/                      # TP1: programas Micro C usados como entrada de teste
├── leiame.txt                  # TP1: instruções originais do professor
├── USO_IA.md                   # TP1: registro de uso de IA
├── tp2-gerador-sintatico/      # TP2: gerador de analisadores sintáticos
│   ├── src/                    # código do gerador (lexer/parser EBNF, conjuntos, LL(1), codegen)
│   ├── microc/                 # scanner do TP1 adaptado para o TP2
│   ├── testes/                 # gramáticas, entradas, saídas esperadas e prova_de_fogo.sh
│   ├── relatorio/              # fonte LaTeX do relatório
│   ├── Makefile
│   ├── USO_IA.md               # TP2: registro de uso de IA
│   └── README.md
└── README.md                   # Este arquivo
```

## 👤 Autor(es)

| Nome | Matrícula |
|------|-----------|
| Rafael Torres Nantes | 202019050169 |
| Victor Hugo de Oliveira Arevalos | 202419050093 |

## 🎓 Disciplina

- **Curso:** Compiladores I
- **Instituição:** FACOM/UFMS
- **Trabalhos:** TP1 — Análise Léxica para Micro C · TP2 — Gerador de Analisadores Sintáticos a partir de EBNF

A submissão deve seguir as instruções específicas informadas pelo professor no AVA (Moodle), dentro do prazo estabelecido no cronograma da disciplina.
