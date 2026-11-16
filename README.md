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

> Status (16/11/2026, cópia limpa do repositório, Ubuntu 24.04 e Windows/MSYS2): todos os `TODO(aluno)` estão implementados, `make` compila sem avisos, `make test` passa 35 de 35, o código gerado compila sem avisos com `gcc -ansi -pedantic -Wall -Wextra` e o `valgrind --leak-check=full` não acusa nenhum erro nem vazamento em 60 execuções.

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
| [`relatorio/`](relatorio/) | TP1 e TP2 | Relatório em LaTeX (`relatorio.tex`) e o PDF, com o léxico, as mudanças do TP1 e o gerador |

### 🔤 Parte 1 - TP1: analisador léxico

O scanner do TP1 roda como parte do parser de Micro C (`make microc`). A entrega original, autônoma (`microc.flex` com `main`, `tests/test.mc` e `leiame.txt`), foi removida da árvore e continua no histórico do repositório (`git log --diff-filter=D --name-only` acha o commit da remoção). O `main` de teste do léxico também existe em `microc/microc.flex`, sob `-DMICROC_TESTE_LEXICO`:

```bash
mkdir -p build
flex -o build/lexer.c microc/microc.flex
gcc -w -DMICROC_TESTE_LEXICO -Imicroc -o build/lexer build/lexer.c
./build/lexer testes/programas/microc/soma.mc
```

Cada token reconhecido é impresso com tipo, lexema e linha (`Token: tipo = INT lexema = 'int' linha = 1`); erros léxicos vão para `stderr` como `ERRO LEXICO (linha N): <mensagem>`.

O que o scanner reconhece (`microc/microc.flex`, tokens em `microc/tokens.h`, seção 2.3.1 do livro-texto):

| Classe | Tokens |
|--------|--------|
| Controle | `UNDEF` (erro léxico), `ID`, `END_OF_FILE` |
| Constantes | `INTEGERCONST` (inclusive negativas, `-{DIGIT}+`), `CHARCONST`, `STRINGCONST` (com escapes `\.`) |
| Operadores | `PLUS` `MINUS` `MUL` `DIV` `MOD` · `EQ` `NEQ` `LT` `GT` `LEQ` `GEQ` `AND` `OR` `NOT` |
| Pontuação | `ASSIGN` `SEMICOLON` `COMMA` · parênteses, chaves e colchetes |
| Reservadas | `MAIN` `IF` `ELSE` `FOR` `RETURN` `INT` `CHAR` `PRINT` |

- **Maior casamento:** os operadores de dois caracteres (`==`, `!=`, `<=`, `>=`, `&&`, `||`) vêm antes dos de um caractere com o mesmo prefixo.
- **Reservadas:** um identificador casa `{LETRA}{ALFANUM}*` e só depois é comparado com as palavras reservadas, sem uma regra por palavra.
- **Comentários:** `//` vai até o fim da linha; `/* ... */` usa a condição de início `COMMENT` e continua contando linhas.
- **Lexemas:** identificadores e constantes vão para uma tabela de strings; o token carrega tipo, lexema e linha.

Erros léxicos (todos devolvem `UNDEF` com a mensagem em `error_msg`; a regra final `.` garante que toda entrada casa alguma regra):

| Situação | Mensagem |
|----------|----------|
| Fim do arquivo dentro de `/* ... */` | `EOF em comentario` |
| `*/` fora de um comentário | `Comentario nao iniciado` |
| Caractere vazio, sem fechar ou com mais de um símbolo | `Constante de caractere invalida` |
| String com caractere nulo | `String contem caractere nulo` |
| Quebra de linha antes de fechar a string | `String nao terminada` |
| Fim do arquivo antes de fechar a string | `EOF em string` |
| Caractere fora do alfabeto | o próprio caractere |

#### Correções feitas desde o TP1

Diferenças entre o `microc.flex` da entrega original do TP1 (no histórico do repositório) e `microc/microc.flex` (as regras léxicas são as mesmas; a regra `<<EOF>>` já devolvia `END_OF_FILE` no TP1):

- `%option prefix="microc_yy" noyywrap noinput nounput`: o scanner passa a ser `microc_yylex()` e não colide com outro `yylex`; `noinput`/`nounput` removem funções não usadas.
- `enum TokenType` e `YYSTYPE` saíram do `.flex` para `microc/tokens.h`, compartilhado com o parser gerado.
- `main()` de teste do léxico só compila com `-DMICROC_TESTE_LEXICO`, para não colidir com o `main` do parser.
- `yywrap()` removida (substituída por `noyywrap`) e `microc_libera_tabela()` nova, que libera a tabela de strings e é chamada por `scanner_finalizar`.
- Comentário de cabeçalho atualizado para registrar a adaptação.
- Adaptador novo `microc/scanner.c`/`scanner.h`: `scanner_iniciar`, `proximo_token` e `scanner_finalizar`. `proximo_token` monta o `Token` com o lexema da tabela de strings e, para `UNDEF`, com a mensagem de erro do TP1.

### 🧠 Parte 2 - TP2: gerador de analisadores sintáticos

```bash
./gerador testes/gramaticas/expr.ebnf -o build/expr --tree
gcc -ansi -pedantic -Wall -Wextra -o build/expr_bin build/expr.c
./build/expr_bin --tree programa.txt     # ou: ./build/expr_bin --tokens tokens.txt
```

Opções: `-o prefixo` grava `prefixo.c` e `prefixo.h`, `--sets` imprime anulável, FIRST e FOLLOW, `--debug` imprime a gramática lida, `--tree` inclui a árvore de derivação e `--scanner microc` usa o scanner do TP1 (`TOKENS EXTERN "tokens.h" ;`).

O que foi implementado em cada `TODO(aluno)` do código-base:

| Arquivo | O que faz |
|---------|-----------|
| `src/ebnf_parser.c` | `parse_expr`, `parse_seq` e `parse_fator`: descendente recursivo da própria notação EBNF, que monta a árvore `Expr` (`E_TERM`, `E_NAOTERM`, `E_SEQ`, `E_ALT`, `E_OPC`, `E_REP`, `E_VAZIO`) de `gramatica.h` |
| `src/verificacao.c` | Inalcançáveis (aviso, busca a partir do símbolo inicial), improdutivos (erro, ponto fixo) e recursão à esquerda direta, indireta e por prefixo anulável (erro, fecho transitivo de Warshall) |
| `src/conjuntos.c` | Anulável, FIRST e FOLLOW por ponto fixo, para cada não terminal e cada subexpressão, em vetores de bits (`bitset.c`); no corpo de `{ a }`, `FOLLOW(a)` recebe `FIRST(a)` |
| `src/ll1.c` | Erros: FIRST de alternativas não disjuntos, mais de uma alternativa anulável, FOLLOW de alternativa anulável cruzando o FIRST de outra, corpo anulável em `[ ]` ou `{ }`. Aviso: FIRST ∩ FOLLOW em `[ ]` ou `{ }`, resolvido de forma gulosa |
| `src/codegen.c` | Uma função `parse_X()` por regra: sequência em ordem, alternativa em `if / else if`, `[ ]` em `if`, `{ }` em `while`; conjuntos de decisão em `static const int conj_N[]`; suporte a `--tree` e `--tokens`; `limpar()` registrada com `atexit` |
| `src/automato.c` | Léxico próprio: regex → AFN (Thompson) → AFD (subconjuntos) → AFD mínimo (Moore), emitido como `switch` por estado; vence o maior prefixo e, em empate, o literal |
| `src/microc_mapa.c` | Lê o `enum TokenType` do `tokens.h` do TP1 para mapear os terminais da gramática de Micro C |
| `microc/scanner.c` | Adaptador do scanner do TP1 (ver [Correções feitas desde o TP1](#correções-feitas-desde-o-tp1)) |

Exemplo do código gerado para `term = factor { ( "*" | "/" ) factor } ;`:

```c
/* term = factor { ( '*' | '/' ) factor } ; */
void parse_term(void)
{
    arvore_entra("term");
    parse_factor();
    while (tok.tipo == T_VEZES || tok.tipo == T_BARRA) {
        if (tok.tipo == T_VEZES) {
            match(T_VEZES);
        } else if (tok.tipo == T_BARRA) {
            match(T_BARRA);
        } else {
            erro_sintatico(conj_1);
        }
        parse_factor();
    }
    arvore_sai();
}
```

Diagnósticos da gramática seguem o formato `arquivo:linha:coluna: erro:` ou `aviso:`. Um erro encerra com código 1 antes de qualquer arquivo ser escrito; um aviso não impede a geração; arquivo inexistente devolve 2.

#### Revisão final do TP2

Depois de os 35 testes passarem, a revisão do código função por função e os testes de robustez (gramáticas vazias, regex sem fechar, CRLF, aninhamento excessivo, bytes aleatórios) trouxeram três mudanças:

- **Memória do analisador gerado:** em entrada rejeitada, o `exit(1)` deixava arquivos abertos e o buffer ou o scanner alocados. Agora `limpar()` roda via `atexit` em qualquer saída.
- **Código redundante:** trechos repetidos em `automato.c`, `conjuntos.c` e `verificacao.c` foram removidos (41 linhas a menos, 16 a mais), sem mudar nenhuma saída.
- **Literais com `*/`:** um literal com `*/` ou `/*` quebrava o comentário C que documenta a regra. O gerador passou a emitir esses pares com um espaço no meio.

## 🔀 Arquitetura da aplicação

O gerador roda em cinco etapas, na ordem de `src/main.c`. Um erro em qualquer etapa interrompe a geração e nenhum arquivo é escrito:

1. **Leitura da EBNF** (`ebnf_lexer.c`, `ebnf_parser.c`): monta a árvore `Expr` descrita em `gramatica.h`.
2. **Verificação semântica** (`verificacao.c`): não definidos, inalcançáveis e improdutivos.
3. **Conjuntos** (`conjuntos.c`): anulável, depois a recursão à esquerda (`verif_recursao_esquerda`, em `verificacao.c`, que depende do anulável), depois FIRST e FOLLOW.
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

O `make test` roda o gerador com `--sets` em cada uma das 10 gramáticas e cada analisador em cada um dos 25 programas, e compara a saída, inclusive a linha final `status 0` ou `status 1`, com o esperado. Fora da suíte, também foram conferidos:

- **Memória:** `valgrind --leak-check=full` no gerador e nos analisadores de `expr`, `json`, `comandos` e Micro C, com e sem `--tree`. Foram 60 execuções, sem nenhum erro nem vazamento.
- **Modo `--tokens`:** nos 6 programas de Micro C, a saída lendo o arquivo de tokens é igual à leitura direta, inclusive o código de saída.

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
