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

### 4. Casos de teste em `testes/`

- Ferramenta: Claude (Anthropic)
- Trecho: gramáticas em `testes/gramaticas/`, programas em `testes/programas/` e o gabarito de FIRST/FOLLOW em `testes/README.md`
- Finalidade: sugerir testes, uso permitido pelo enunciado, e conferir o cálculo de anulável, FIRST e FOLLOW feito à mão.
- O que fiz: revisei cada caso e o diagnóstico esperado. Nenhuma parte do gerador (leitura, verificação, conjuntos, LL(1), geração) foi produzida por IA.
