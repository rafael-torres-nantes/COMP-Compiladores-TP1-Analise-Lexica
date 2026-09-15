aluno: 202019050169

# Trabalho Prático 1 — Análise Léxica para Micro C

Trabalho Prático 1 da disciplina **Compiladores I (FACOM)**: implementação de um analisador
léxico (scanner) para a linguagem **Micro C**, usando **Flex**.

## Sobre o trabalho

O objetivo é completar a especificação léxica de Micro C a partir do esqueleto fornecido
(`microc.flex`), reconhecendo todos os tokens da linguagem — palavras reservadas, identificadores,
constantes, operadores e delimitadores — além de identificar e reportar erros léxicos.

O enunciado completo está em [`TP1_Analise_Lexica_MicroC.pdf`](./TP1_Analise_Lexica_MicroC.pdf) e
as instruções originais de compilação em [`leiame.txt`](./leiame.txt).

## Como compilar

```bash
flex microc.flex
gcc lex.yy.c -o lexer
```

## Como executar

```bash
./lexer tests/test.mc
```

## Estrutura

```
.
├── microc.flex                    # especificação léxica (arquivo principal)
├── tests/                         # programas Micro C usados como entrada de teste
├── TP1_Analise_Lexica_MicroC.pdf  # enunciado do trabalho
└── leiame.txt                     # instruções originais do professor
```

## Disciplina

Compiladores I — FACOM. Repositório espelhado a partir do pacote fornecido pelo professor
[Amaury Antônio de Castro Junior](https://github.com/amaury-junior/comp1-2026).
