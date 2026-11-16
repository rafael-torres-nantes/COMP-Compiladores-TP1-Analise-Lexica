# Pasta microc/

Aqui fica o scanner de Micro C do TP1, convertido em função para o analisador gerado com `--scanner microc` (seção 3.1 do enunciado).

Arquivos desta pasta:

- `scanner.h`: interface usada pelo analisador gerado (`scanner_iniciar`, `proximo_token`, `scanner_finalizar`).
- `scanner.c`: adaptador entre `microc_yylex()` e `Token`. O lexema vem da tabela de strings e o token `UNDEF` traz a mensagem de erro do TP1.
- `microc.flex`: o scanner do TP1, adaptado com `%option prefix="microc_yy" noyywrap` e `return END_OF_FILE;` na regra `<<EOF>>`. As diferenças em relação à entrega original (que está no histórico do git) estão na seção "Correções feitas desde o TP1" do `README.md` da raiz.
- `tokens.h`: `TokenType` e `Token` como no livro-texto (seção 2.3.1), extraídos do `microc.flex` do TP1.

O Makefile compila todos os `.c` desta pasta. O `lex.yy.c` gerado é compilado sem a exigência de ausência de avisos.

Cuidado com tokens devolvidos pelo código ASCII (TP1, seção 5). Valores até o último item de `TokenType` (36 no `tokens.h` do livro) são sempre interpretados como `TokenType`. Por isso `!` (33) deve ser devolvido como `NOT`, e não como `'!'`.
