aluno: 202019050169
aluno: 202419050093
uso_ia: sim

# 🧮 Trabalhos de Compiladores I - Partes 1 e 2

## 👨‍💻 Projeto desenvolvido por:
[Rafael Torres Nantes](https://github.com/rafael-torres-nantes) e Victor Hugo de Oliveira Arevalos

## Índice

* [📚 Contextualização do projeto](#-contextualização-do-projeto)
* [🛠️ Tecnologias/Ferramentas utilizadas](#%EF%B8%8F-tecnologiasferramentas-utilizadas)
* [🖥️ Funcionamento do sistema](#%EF%B8%8F-funcionamento-do-sistema)
   * [🔤 Parte 1 - TP1: analisador léxico](#-parte-1---tp1-analisador-léxico)
   * [🧠 Parte 2 - TP2: gerador de analisadores sintáticos](#-parte-2---tp2-gerador-de-analisadores-sintáticos)
* [🔀 Arquitetura da aplicação](#-arquitetura-da-aplicação)
* [📁 Estrutura do projeto](#-estrutura-do-projeto)
* [📌 Como executar o projeto](#-como-executar-o-projeto)
* [🔎 Como usar: entrada e saída](#-como-usar-entrada-e-saída)
* [🕵️ Dificuldades Encontradas](#%EF%B8%8F-dificuldades-encontradas)
* [🎓 Disciplina](#-disciplina)

## 📚 Contextualização do projeto

Repositório de entrega dos Trabalhos Práticos da disciplina **Compiladores I (FACOM/UFMS, 2026/2)**. O **TP1** é um analisador léxico de Micro C em **Flex**; o **TP2** é um gerador que lê uma gramática em **EBNF** e escreve, em **ANSI C**, um analisador sintático descendente recursivo para ela. O layout é o do TP2: a raiz do repositório é a raiz do projeto do gerador, e o TP1 é o módulo `microc/`, que o analisador gerado usa como fonte de tokens.

> Status: todos os `TODO(aluno)` estão implementados, `make` compila sem avisos, `make test` passa 35 de 35.

## 🛠️ Tecnologias/Ferramentas utilizadas

[<img src="https://img.shields.io/badge/C-A8B9CC?logo=c&logoColor=black">](https://en.wikipedia.org/wiki/ANSI_C)
[<img src="https://img.shields.io/badge/Flex-3C3C3C?logo=gnu&logoColor=white">](https://github.com/westes/flex)
[<img src="https://img.shields.io/badge/GNU_Make-A42E2B?logo=gnu&logoColor=white">](https://www.gnu.org/software/make/)
[<img src="https://img.shields.io/badge/LaTeX-008080?logo=latex&logoColor=white">](https://www.latex-project.org/)

## 🖥️ Funcionamento do sistema

| Pasta | Trabalho | O que é |
|-------|----------|---------|
| [`microc/`](microc/) | **TP1** | Analisador léxico de Micro C em **Flex** (`microc.flex`, `tokens.h`) mais o adaptador `scanner.c`/`scanner.h` que o expõe como função para o parser gerado |
| [`src/`](src/) | **TP2** | Gerador de analisadores sintáticos descendentes recursivos a partir de EBNF, em **ANSI C (C89/C90)** |
| [`testes/`](testes/) | TP2 | Gramáticas, programas de entrada, saídas esperadas, `executar.sh` |
| [`relatorio/`](relatorio/) | TP2 | Relatório em LaTeX (`relatorio.tex`) e o PDF |

### 🔤 Parte 1 - TP1: analisador léxico

O scanner do TP1 roda como parte do parser de Micro C (`make microc`). A entrega original, autônoma (`microc.flex` com `main`, `tests/test.mc` e `leiame.txt`), foi removida da árvore e continua no histórico do repositório (`git log --diff-filter=D --name-only` acha o commit da remoção). O `main` de teste do léxico também existe em `microc/microc.flex`, sob `-DMICROC_TESTE_LEXICO`:

```bash
mkdir -p build
flex -o build/lexer.c microc/microc.flex
gcc -w -DMICROC_TESTE_LEXICO -Imicroc -o build/lexer build/lexer.c
./build/lexer testes/programas/microc/soma.mc
```

Cada token reconhecido é impresso com tipo, lexema e linha (`Token: tipo = INT lexema = 'int' linha = 1`); erros léxicos vão para `stderr` como `ERRO LEXICO (linha N): <mensagem>`. Itens do TP1: palavras reservadas, constantes de caractere e de string com escapes, operadores com prefixo compartilhado (`!=`, `<=`, `>=`, `&&`, `||`), inteiros negativos e regra final de erro para qualquer entrada.

#### Correções feitas desde o TP1

Diferenças entre o `microc.flex` da entrega original do TP1 (no histórico do repositório) e `microc/microc.flex` (as regras léxicas são as mesmas; a regra `<<EOF>>` já devolvia `END_OF_FILE` no TP1):

- `%option prefix="microc_yy" noyywrap noinput nounput`: o scanner passa a ser `microc_yylex()` e não colide com outro `yylex`; `noinput`/`nounput` removem funções não usadas.
- `enum TokenType` e `YYSTYPE` saíram do `.flex` para `microc/tokens.h`, compartilhado com o parser gerado.
- `main()` de teste do léxico só compila com `-DMICROC_TESTE_LEXICO`, para não colidir com o `main` do parser.
- `yywrap()` removida (substituída por `noyywrap`) e `microc_libera_tabela()` nova, que libera a tabela de strings e é chamada por `scanner_finalizar`.
- Comentário de cabeçalho atualizado para registrar a adaptação.

### 🧠 Parte 2 - TP2: gerador de analisadores sintáticos

```bash
./gerador testes/gramaticas/expr.ebnf -o build/expr --tree
gcc -ansi -pedantic -Wall -Wextra -o build/expr_bin build/expr.c
./build/expr_bin --tree programa.txt     # ou: ./build/expr_bin --tokens tokens.txt
```

Opções: `-o prefixo` grava `prefixo.c` e `prefixo.h`, `--sets` imprime anulável, FIRST e FOLLOW, `--debug` imprime a gramática lida, `--tree` inclui a árvore de derivação e `--scanner microc` usa o scanner do TP1 (`TOKENS EXTERN "tokens.h" ;`).

## 🔀 Arquitetura da aplicação

O gerador roda em cinco etapas, na ordem de `src/main.c`. Um erro em qualquer etapa interrompe a geração e nenhum arquivo é escrito:

1. **Leitura da EBNF** (`ebnf_lexer.c`, `ebnf_parser.c`): monta a árvore `Expr` descrita em `gramatica.h`.
2. **Verificação semântica** (`verificacao.c`): não definidos, inalcançáveis e improdutivos.
3. **Conjuntos** (`conjuntos.c`): anulável, recursão à esquerda, FIRST e FOLLOW.
4. **Verificação LL(1)** (`ll1.c`): conflitos FIRST∩FIRST e FIRST∩FOLLOW.
5. **Geração de código** (`codegen.c`): uma função C por regra, com o léxico de cada regex vindo de `automato.c`.

No modo Micro C, o analisador gerado não tem léxico próprio: ele pede cada token a `proximo_token()`, em `microc/scanner.c`, que chama o `microc_yylex()` gerado pelo Flex a partir do TP1.

## 📁 Estrutura do projeto

```
.
├── Makefile
├── microc/                 # TP1: scanner de Micro C (microc.flex, tokens.h, scanner.c/.h)
├── src/                    # TP2: código do gerador
├── testes/                 # TP2: gramáticas, entradas, esperados, executar.sh
├── relatorio/              # TP2: relatório LaTeX e PDF
├── USO_IA.md               # registro obrigatório de uso de IA
└── README.md
```

## 📌 Como executar o projeto

Requer `gcc`, `make`, `flex` e `diff`. Tudo roda da raiz:

```bash
git clone https://github.com/rafael-torres-nantes/COMP-Compiladores-TP1-Analise-Lexica.git
cd COMP-Compiladores-TP1-Analise-Lexica
make                          # compila ./gerador e build/microc/parser_microc
make microc                   # só o parser de Micro C (scanner do TP1 + parser gerado)
make test                     # testes de testes/ (35)
make clean
```

## 🔎 Como usar: entrada e saída

Todos os exemplos abaixo foram executados na raiz do projeto (saídas coladas sem edição). Para gerar um analisador, o gerador lê uma gramática EBNF; o analisador gerado lê o programa a ser reconhecido.

### Gerar, compilar e rodar (gramática de expressões)

Entrada, `testes/gramaticas/expr.ebnf`:

```
(* Expressoes aritmeticas: exemplo da secao 5 do enunciado *)
TOKENS
  NUM  = /[0-9]+/ ;
  ID   = /[a-zA-Z_][a-zA-Z0-9_]*/ ;
  SKIP = /[ \t\r\n]+/ ;

RULES
  expr   = term { ( "+" | "-" ) term } ;
  term   = factor { ( "*" | "/" ) factor } ;
  factor = NUM | ID | "(" expr ")" ;
```

```bash
make
mkdir -p build
./gerador testes/gramaticas/expr.ebnf -o build/expr --tree
gcc -ansi -pedantic -Wall -Wextra -o build/expr_bin build/expr.c
echo '2 * (x + 1)' > build/ok.txt
echo '2 * (x + )'  > build/ruim.txt
./build/expr_bin build/ok.txt;   echo "saida=$?"
./build/expr_bin build/ruim.txt; echo "saida=$?"
```

```
entrada aceita
saida=0
linha 1, coluna 10: encontrado ')', esperado um de {NUM, ID, '('}
saida=1
```

Código de saída 0 é aceito, 1 é erro léxico ou sintático (o analisador para no primeiro erro e diz o que esperava).

### Conjuntos anulável, FIRST e FOLLOW (`--sets`)

```bash
./gerador testes/gramaticas/expr.ebnf --sets
```

```
NAO TERMINAL  ANULAVEL  FIRST / FOLLOW
expr          nao       FIRST  = {NUM, ID, '('}
                        FOLLOW = {$, ')'}
term          nao       FIRST  = {NUM, ID, '('}
                        FOLLOW = {$, '+', '-', ')'}
factor        nao       FIRST  = {NUM, ID, '('}
                        FOLLOW = {$, '+', '-', '*', '/', ')'}
```

### Árvore de derivação (`--tree`)

```bash
echo '2 * x' > build/arv.txt
./build/expr_bin --tree build/arv.txt
```

```
expr
  term
    factor
      NUM '2'
    '*'
    factor
      ID 'x'
entrada aceita
```

Cada nível de recuo é uma chamada de procedimento do analisador (um não terminal da gramática); as folhas, com o lexema entre aspas, são os tokens lidos da entrada.

### Modo Micro C (scanner do TP1)

Entrada, `testes/programas/microc/soma.mc`:

```c
int main()
{
int a;
int b;
a = 10;
b = 20;
print(a + b);
return 0;
}
```

```bash
make microc
./build/microc/parser_microc --tree testes/programas/microc/soma.mc
```

A árvore tem 74 linhas; as 25 primeiras:

```
programa
  'int'
  'main'
  '('
  ')'
  bloco
    '{'
    declaracao
      tipo
        'int'
      ID 'a'
      ';'
    declaracao
      tipo
        'int'
      ID 'b'
      ';'
    comando
      atribuicao
        atribuicao_interna
          ID 'a'
          '='
          expressao
            expressao_logica
              expressao_relacional
```

Programa rejeitado, `testes/programas/microc/rejeitado_sintaxe.mc`:

```c
int main() {
    int x;
    x = (3 + ;
}
```

```bash
./build/microc/parser_microc testes/programas/microc/rejeitado_sintaxe.mc; echo "saida=$?"
```

```
linha 3: encontrado ';', esperado um de {'(', ID, INTEGERCONST, CHARCONST, '!'}
saida=1
```

### Suíte de testes

```bash
make test
```

Última linha da saída:

```
35 de 35 testes passaram
```

## 🕵️ Dificuldades Encontradas

- **`else` pendente na gramática de Micro C:** `[ "else" comando ]` gera um aviso de conflito LL(1). O conflito é resolvido de forma gulosa, como o manual pede: o `else` fica com o `if` mais próximo.
- **Recursão à esquerda antes do FIRST:** o FIRST de uma regra recursiva à esquerda nunca chega a ponto fixo, então a recursão é checada depois do anulável e antes do FIRST.
- **Memória em entrada rejeitada:** o `exit(1)` de erro saía sem fechar os arquivos nem liberar o buffer ou o scanner. O analisador gerado passou a registrar `atexit(limpar)`, e o `valgrind --leak-check=full` fica limpo também em entrada rejeitada.
- **Fim de linha no Windows:** num checkout Windows, `testes/esperado/` vem com CRLF e o gerador escreve LF, então o `make test` falha inteiro. Com um `diff --strip-trailing-cr` no PATH os 35 passam. No Linux não acontece, porque o git guarda LF.

## 🎓 Disciplina

| Nome | Matrícula |
|------|-----------|
| Rafael Torres Nantes | 202019050169 |
| Victor Hugo de Oliveira Arevalos | 202419050093 |

- **Curso:** Compiladores I
- **Instituição:** FACOM/UFMS
- **Trabalhos:** TP1 — Análise Léxica para Micro C · TP2 — Gerador de Analisadores Sintáticos a partir de EBNF
- **Uso de IA:** registrado em [`USO_IA.md`](USO_IA.md), obrigatório pelo enunciado.

A submissão deve seguir as instruções específicas informadas pelo professor no AVA (Moodle), dentro do prazo estabelecido no cronograma da disciplina.
