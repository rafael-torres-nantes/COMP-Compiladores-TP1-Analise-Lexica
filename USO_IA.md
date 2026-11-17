# USO_IA.md

Registro de uso de ferramentas de IA generativa nos TP1 e TP2, conforme as regras do TP1 (slide 26
do enunciado): permitido para tirar dúvidas, interpretar erros, revisar código e sugerir
testes; proibido gerar a solução das partes a implementar.

## TP1: analisador léxico

### 1. Regras da especificação léxica em `microc/microc.flex`

- Ferramenta: Claude (Anthropic)
- Trecho: regras da especificação léxica em `microc/microc.flex`
- Finalidade: tirar dúvidas conceituais sobre flex (condições de início, regra do casamento mais longo, uso de `yyless`) e validar o raciocínio por trás das regras antes de escrevê-las.
- O que fiz: escrevi as regras do trabalho. As respostas serviram para confirmar o raciocínio e esclarecer dúvidas pontuais; nenhuma regra foi gerada nem copiada pronta.

### 2. Casos de teste do TP1 (`tests/test.mc`, hoje só no histórico do git)

- Ferramenta: Claude (Anthropic)
- Trecho: casos de teste em `tests/test.mc` do TP1 (removido do repositório; está no histórico do git)
- Finalidade: revisar os casos de teste que escrevi e verificar se cobriam os erros léxicos previstos na Seção 4.1 do enunciado.
- O que fiz: usei a revisão para conferir a cobertura dos testes e completar os casos que faltavam.

## TP2: gerador de analisadores sintáticos

### 3. Leitura do enunciado e plano de execução

- Ferramenta: Claude (Anthropic)
- Trecho: nenhum arquivo de código; apenas os resumos internos do enunciado e do plano de execução (material local, fora do repositório)
- Finalidade: resumir o enunciado do TP2 e organizar as fases de trabalho.
- O que fiz: revisei o resumo contra os slides `tp2_gerador_ebnf-1.pdf` e defini a ordem de implementação.

### 4. Criação de testes em `testes/`

- Ferramenta: Claude (Anthropic)
- Trecho: gramáticas em `testes/gramaticas/`, programas em `testes/programas/` e as saídas esperadas em `testes/esperado/`
- Finalidade: criar casos de teste (gramáticas válidas e com erro, programas aceitos e rejeitados) e conferir o cálculo de anulável, FIRST e FOLLOW feito à mão.
- O que fiz: revisei cada caso, conferi o diagnóstico esperado contra a saída do gerador e mantive só os casos que exercitam requisitos do enunciado.

### 5. Validação de lógicas antes de implementar

- Ferramenta: Claude (Anthropic)
- Trecho: raciocínio das etapas em `src/` (verificação semântica, conjuntos, condição LL(1) e geração de código)
- Finalidade: validar a lógica de algumas partes antes de implementá-las, por exemplo a ordem do ponto fixo de FIRST e FOLLOW e a detecção de recursão à esquerda por prefixo anulável.
- O que fiz: escrevi o código das etapas. As respostas serviram para conferir o raciocínio e apontar casos de borda.

### 6. Execução dos testes simulando Ubuntu

- Ferramenta: Claude (Anthropic)
- Trecho: execução dos testes em `testes/`
- Finalidade: como minha máquina é Windows, usei a IA para me ajudar a rodar os testes num ambiente Ubuntu simulado com Docker.
- O que fiz: conferi os resultados dos testes e corrigi no código o que eles apontaram.

### 7. Ajustes no relatório e no README

- Ferramenta: Claude (Anthropic)
- Trecho: `relatorio/relatorio.tex` e `README.md`
- Finalidade: revisar a redação, organizar as seções e conferir se o texto descreve o que o código faz.
- O que fiz: revisei o texto final e conferi cada afirmação contra o código e os testes.
