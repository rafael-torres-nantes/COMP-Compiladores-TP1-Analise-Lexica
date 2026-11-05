# USO_IA.md

Registro de uso de ferramentas de IA generativa no TP2, conforme as regras do TP1 (slide 26
do enunciado): permitido para tirar dúvidas, interpretar erros, revisar código e sugerir
testes; proibido gerar a solução das partes a implementar.

## 1. Leitura do enunciado e plano de execução

- Ferramenta: Claude (Anthropic)
- Trecho: nenhum arquivo de código; apenas `docs/enunciado-resumo.md` e `docs/plano-execucao.md`
- Finalidade: resumir o enunciado do TP2 e organizar as fases de trabalho.
- O que fiz: revisei o resumo contra os slides `tp2_gerador_ebnf-1.pdf` e defini a ordem de implementação.

## 2. Casos de teste em `tests/`

- Ferramenta: Claude (Anthropic)
- Trecho: gramáticas em `tests/gramaticas/`, programas em `tests/programas/` e o gabarito de FIRST/FOLLOW em `tests/README.md`
- Finalidade: sugerir testes, uso permitido pelo enunciado, e conferir o cálculo de anulável, FIRST e FOLLOW feito à mão.
- O que fiz: revisei cada caso e o diagnóstico esperado. Nenhuma parte do gerador (leitura, verificação, conjuntos, LL(1), geração) foi produzida por IA.
