// test.mc - Programa de teste para o scanner de Micro C
//
// Este arquivo exercita boa parte dos tokens da linguagem: palavras
// reservadas, identificadores, constantes inteiras, de caractere e de
// string, operadores aritmeticos, relacionais e logicos, simbolos de
// pontuacao, alem de comentarios de linha e de bloco.
//
// LEMBRETE: este teste NAO cobre todos os casos (em especial, poucos
// casos de erro lexico). Parte do trabalho e ampliar este arquivo com
// testes proprios que exercitem seu scanner de forma mais completa,
// incluindo entradas invalidas (veja a Secao 4.1 do enunciado).

/* Calcula o fatorial de um numero inteiro utilizando um laco for. */

/* Comentario de bloco
   em mais de uma linha. */
int fatorial(int n) {
    int resultado;
    int i;

    resultado = 1;
    for (i = 1; i <= n; i = i + 1) {
        resultado = resultado * i;
    }
    return resultado;
}

int main() {
    int x;
    int y;
    int linhas, colunas;
    char c;
    char letras[10];

    x = 5;
    y = fatorial(x);

    if (y > 100) {
        print("Resultado grande");
    } else {
        print("Resultado pequeno");
    }

    c = 'A';
    letras[0] = 'H';
    letras[1] = 'i';

    // Testando operadores relacionais e logicos
    if (x >= 0 && y != 0) {
        print("x e nao-negativo e y e diferente de zero");
    }

    if (x == 5 || y == 0) {
        print("condicao ou satisfeita");
    }

    if (!(x < 0)) {
        print("x nao e negativo");
    }

    // Constante negativa x subtracao
    x = -5;
    x = (-42);
    y = x - 5;
    y = fatorial(x) - 1;
    y = letras[0] - 1;

    y = x + 1 * 2 / 3 % 4;

    print("linha\ntabulacao\taspas \" barra \\ fim");
    c = '\n';

    return 0;
}

// --- Erros lexicos recuperaveis (Secao 4.1 do enunciado) -------------
@
*/
c = 'ab';
print("sem fim
print("nulo\0aqui");
