#!/bin/sh
# Prova de fogo do gerador. Execute a partir da raiz do projeto, depois de
# make:   sh testes/prova_de_fogo.sh
#
# Criterio comum: o gerador e os analisadores gerados sempre terminam com
# 0 ou 1 (2 para arquivo inexistente) -- nunca travam nem caem -- e todo
# codigo gerado compila sem avisos com -ansi -pedantic -Wall -Wextra.

GERADOR=./gerador
CC=${CC:-gcc}
CFLAGS="-ansi -pedantic -Wall -Wextra -Werror"
F=build/fogo
passou=0
total=0
rm -rf "$F"
mkdir -p "$F"
export LC_ALL=C

ok() {
    passou=$((passou + 1))
    total=$((total + 1))
    echo "ok     $1"
}

falha() {
    total=$((total + 1))
    echo "FALHA  $1"
    [ -n "$2" ] && echo "$2" | sed 's/^/       /'
}

# gera DIR TEXTO: grava a gramatica, roda o gerador e, se ele aceitar,
# compila o analisador. $rc e o codigo do gerador; $compila e sim, nao ou -.
gera() {
    mkdir -p "$1"
    printf '%s' "$2" > "$1/g.ebnf"
    timeout 10 $GERADOR "$1/g.ebnf" -o "$1/p" >/dev/null 2>"$1/err"
    rc=$?
    compila=-
    if [ $rc -eq 0 ]; then
        $CC $CFLAGS -o "$1/a" "$1/p.c" 2>"$1/cc" && compila=sim || compila=nao
    fi
}

# borda NOME RC_ESPERADO TEXTO_DA_GRAMATICA [ENTRADA RC_ESPERADO]...
borda() {
    nome=$1; esperado=$2; gera "$F/$nome" "$3"; shift 3
    if [ $rc -ne "$esperado" ] || [ $compila = nao ]; then
        falha "borda $nome: gerador saiu com $rc (esperado $esperado), compila=$compila" \
            "$(cat "$F/$nome/err" "$F/$nome/cc" 2>/dev/null | head -5)"
        return
    fi
    ok "borda $nome: gerador saiu com $rc$( [ $rc -eq 0 ] && echo ', analisador compila')"
    while [ $# -ge 2 ]; do
        printf '%s' "$1" > "$F/$nome/in"
        timeout 10 "$F/$nome/a" "$F/$nome/in" >/dev/null 2>&1
        r=$?
        if [ $r -eq "$2" ]; then ok "borda $nome: entrada [$1] saiu com $r"
        else falha "borda $nome: entrada [$1] saiu com $r, esperado $2"; fi
        shift 2
    done
}

echo "== bordas da EBNF"
grande=$(printf 'a%.0s' $(seq 1 600))
borda arquivo_vazio 1 ''
borda comentario_aberto 1 '(* nunca fecha'
borda literal_aberto 1 'RULES
s = "abc ;'
borda regex_aberta 1 'TOKENS
X = /abc ;
RULES
s = X ;'
borda regex_aceita_vazio 1 'TOKENS
X = /a*/ ;
RULES
s = X ;'
borda auto_referencia 1 'RULES
s = s ;'
borda nome_com_600_caracteres 0 "RULES
$grande = \"x\" ;" 'x' 0
borda sem_alternativas 0 'RULES
s = "x" ;' 'x' 0 'xx' 1 '' 1
borda nomes_do_c 0 'RULES
while = int ;
int = main ;
main = "x" ;' 'x' 0
borda token_com_nome_de_mapeamento 0 'TOKENS
MAIS = /m/ ;
RULES
s = MAIS "+" ;' 'm+' 0 '+m' 1
borda literais_que_quebram_c 0 'RULES
s = "*/" "\"" "\\" "/*" ;' '*/"\/*' 0 '*/"/*' 1
borda alternativa_vazia 0 'RULES
s = "a" | ;' '' 0 'a' 0 'aa' 1
borda fim_de_linha_crlf 0 "$(printf 'RULES\r\ns = "x" ;\r\n')" 'x' 0
borda maior_prefixo_contra_literal 0 'TOKENS
ID = /[a-z]+/ ;
SKIP = /[ ]+/ ;
RULES
s = { "if" | ID } ;' 'if ifx iff if' 0

# s = X aceita a entrada se e somente se ela inteira casa com X (o maior
# prefixo cobre tudo); grep -Ex e o juiz independente.
echo "== expressoes regulares contra grep -Ex"
palavras=$(for a in a b c; do echo $a; for b in a b c; do echo $a$b; for c in a b c; do echo $a$b$c; done; done; done)
for re in 'ab|c' '(a|b)*c' 'a+b?' '[ab]+' '[^a]+' 'a(b|c)+a?' '(ab)+|c+' '[a-c]b*' '(a|ab)(c|bcd)*' 'a*b' '((a|b)(b|c))+' 'c?(ab)*a'; do
    d=$F/regex_$(echo "$re" | tr -c 'a-z0-9\n' '_')
    gera "$d" "TOKENS
X = /$re/ ;
RULES
s = X ;"
    if [ $compila != sim ]; then
        falha "regex /$re/: gerador saiu com $rc, compila=$compila" "$(cat "$d/err")"
        continue
    fi
    divergencias=""
    for w in $palavras; do
        printf '%s' "$w" > "$d/in"
        "$d/a" "$d/in" >/dev/null 2>&1 && nosso=0 || nosso=1
        echo "$w" | grep -Eqx "$re" && juiz=0 || juiz=1
        [ $nosso -ne $juiz ] && divergencias="$divergencias $w"
    done
    if [ -z "$divergencias" ]; then ok "regex /$re/: 39 cadeias iguais ao grep -Ex"
    else falha "regex /$re/: diverge do grep -Ex em:$divergencias"; fi
done

# Mutacao: apaga, insere ou troca um caractere das gramaticas validas. O
# gerador pode aceitar ou recusar, mas nunca cair, e o que ele aceitar
# tem de compilar.
echo "== gramaticas mutadas"
for g in expr comandos json; do
    quedas=""
    for semente in $(seq 1 25); do
        texto=$(awk -v semente=$semente '
            BEGIN { srand(semente) }
            { s = s $0 "\n" }
            END {
                p = int(rand() * length(s)) + 1
                op = int(rand() * 3)
                c = substr("=;|[]{}()\"/*aZ_ \n", int(rand() * 17) + 1, 1)
                if (op == 0) s = substr(s, 1, p - 1) substr(s, p + 1)
                else if (op == 1) s = substr(s, 1, p - 1) c substr(s, p)
                else s = substr(s, 1, p - 1) c substr(s, p + 1)
                printf "%s", s
            }' "testes/gramaticas/$g.ebnf")
        gera "$F/mut_${g}_$semente" "$texto"
        if [ $rc -gt 1 ] || [ $compila = nao ]; then quedas="$quedas $semente(rc=$rc,compila=$compila)"; fi
    done
    if [ -z "$quedas" ]; then ok "$g: 25 mutacoes sem queda nem codigo que nao compila"
    else falha "$g: mutacoes com problema:$quedas"; fi
done

echo "== entradas hostis"
for g in expr comandos json; do
    gera "$F/$g" "$(cat "testes/gramaticas/$g.ebnf")"
    quedas=""
    for semente in $(seq 1 20); do
        awk -v semente=$semente 'BEGIN { srand(semente); n = int(rand() * 300)
            for (i = 0; i < n; i++) printf "%c", int(rand() * 255) + 1 }' > "$F/lixo_$g"
        timeout 10 "$F/$g/a" "$F/lixo_$g" >/dev/null 2>&1
        r=$?
        [ $r -gt 1 ] && quedas="$quedas $semente(rc=$r)"
    done
    if [ -z "$quedas" ]; then ok "$g: 20 arquivos de bytes aleatorios sem queda"
    else falha "$g: bytes aleatorios derrubaram o analisador:$quedas"; fi
done

profundo() {
    awk -v n=$1 -v fecha=$2 'BEGIN { for (i = 0; i < n; i++) printf "("; printf "1"
        if (fecha) for (i = 0; i < n; i++) printf ")" }'
}
confere_entrada() {
    timeout 20 "$F/$1/a" "$2" >/dev/null 2>&1
    r=$?
    if [ $r -eq "$3" ]; then ok "$1: $4 saiu com $r"
    else falha "$1: $4 saiu com $r, esperado $3"; fi
}
profundo 3000 1 > "$F/aninhado"
confere_entrada expr "$F/aninhado" 0 "3000 parenteses aninhados"
profundo 3000 0 > "$F/aninhado_aberto"
confere_entrada expr "$F/aninhado_aberto" 1 "3000 parenteses sem fechar"
awk 'BEGIN { printf "["; for (i = 0; i < 50000; i++) printf "%s%d", i ? "," : "", i; printf "]" }' > "$F/lista_grande"
confere_entrada json "$F/lista_grande" 0 "lista com 50000 numeros"
printf '' > "$F/vazio"
confere_entrada expr "$F/vazio" 1 "arquivo vazio"
confere_entrada json "$F/inexistente" 2 "arquivo inexistente"

echo
echo "$passou de $total testes passaram"
[ $passou -eq $total ]
