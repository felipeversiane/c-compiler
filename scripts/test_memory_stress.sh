#!/bin/bash

echo "=== TESTE DE ESTRESSE DE MEMÓRIA ==="
echo ""

# Compilar o projeto
echo "1. Compilando o projeto..."
make clean
make
if [ $? -ne 0 ]; then
    echo "ERRO: Falha na compilação!"
    exit 1
fi
echo "✅ Compilação bem-sucedida!"
echo ""

# Teste 1: Arquivo com muitas variáveis
echo "2. TESTE 1: Muitas variáveis (teste de fragmentação)"
echo "----------------------------------------"
cat > /tmp/many_vars.txt << 'EOF'
principal() {
    // Declarar muitas variáveis para testar fragmentação
    inteiro !a1 = 1; inteiro !a2 = 2; inteiro !a3 = 3; inteiro !a4 = 4; inteiro !a5 = 5;
    inteiro !b1 = 6; inteiro !b2 = 7; inteiro !b3 = 8; inteiro !b4 = 9; inteiro !b5 = 10;
    inteiro !c1 = 11; inteiro !c2 = 12; inteiro !c3 = 13; inteiro !c4 = 14; inteiro !c5 = 15;
    inteiro !d1 = 16; inteiro !d2 = 17; inteiro !d3 = 18; inteiro !d4 = 19; inteiro !d5 = 20;
    inteiro !e1 = 21; inteiro !e2 = 22; inteiro !e3 = 23; inteiro !e4 = 24; inteiro !e5 = 25;
    
    texto !msg1 = "Mensagem de teste 1";
    texto !msg2 = "Mensagem de teste 2";
    texto !msg3 = "Mensagem de teste 3";
    texto !msg4 = "Mensagem de teste 4";
    texto !msg5 = "Mensagem de teste 5";
    
    retorno 0;
}
EOF
bin/compiler /tmp/many_vars.txt
echo ""

# Teste 2: Arquivo com strings muito longas
echo "3. TESTE 2: Strings muito longas"
echo "----------------------------------------"
cat > /tmp/long_strings.txt << 'EOF'
principal() {
    texto !str1 = "Esta é uma string extremamente longa que contém muitos caracteres para testar o gerenciamento de memória do compilador. A string deve ser alocada corretamente e liberada após o uso. Este teste verifica se o sistema consegue lidar com strings de tamanho considerável sem vazamentos de memória.";
    
    texto !str2 = "Segunda string muito longa para continuar testando o sistema de gerenciamento de memória. Esta string também deve ser processada corretamente pelo analisador léxico e liberada adequadamente pelo gerenciador de memória.";
    
    texto !str3 = "Terceira string longa para garantir que o sistema está funcionando corretamente mesmo com múltiplas strings grandes sendo processadas simultaneamente.";
    
    escreva(!str1);
    escreva(!str2);
    escreva(!str3);
    
    retorno 0;
}
EOF
bin/compiler /tmp/long_strings.txt
echo ""

# Teste 3: Arquivo com muitos números
echo "4. TESTE 3: Muitos números (inteiros e decimais)"
echo "----------------------------------------"
cat > /tmp/many_numbers.txt << 'EOF'
principal() {
    // Muitos números inteiros
    inteiro !n1 = 12345; inteiro !n2 = 67890; inteiro !n3 = 11111; inteiro !n4 = 22222;
    inteiro !n5 = 33333; inteiro !n6 = 44444; inteiro !n7 = 55555; inteiro !n8 = 66666;
    inteiro !n9 = 77777; inteiro !n10 = 88888; inteiro !n11 = 99999; inteiro !n12 = 00000;
    
    // Muitos números decimais
    decimal !d1 = 123.456; decimal !d2 = 789.012; decimal !d3 = 345.678; decimal !d4 = 901.234;
    decimal !d5 = 567.890; decimal !d6 = 123.789; decimal !d7 = 456.123; decimal !d8 = 789.456;
    decimal !d9 = 012.345; decimal !d10 = 678.901; decimal !d11 = 234.567; decimal !d12 = 890.123;
    
    retorno 0;
}
EOF
bin/compiler /tmp/many_numbers.txt
echo ""

# Teste 4: Arquivo com muitas operações
echo "5. TESTE 4: Muitas operações matemáticas"
echo "----------------------------------------"
cat > /tmp/many_operations.txt << 'EOF'
principal() {
    inteiro !a = 10;
    inteiro !b = 20;
    inteiro !c = 30;
    inteiro !d = 40;
    inteiro !e = 50;
    
    // Muitas operações para testar tokens de operadores
    inteiro !result1 = !a + !b;
    inteiro !result2 = !b - !a;
    inteiro !result3 = !c * !d;
    inteiro !result4 = !d / !e;
    inteiro !result5 = !a ^ 2;
    
    inteiro !result6 = !a + !b + !c;
    inteiro !result7 = !b - !a - !c;
    inteiro !result8 = !c * !d * !e;
    inteiro !result9 = !d / !e / !a;
    
    retorno 0;
}
EOF
bin/compiler /tmp/many_operations.txt
echo ""

# Teste 5: Arquivo com muitas comparações
echo "6. TESTE 5: Muitas comparações lógicas"
echo "----------------------------------------"
cat > /tmp/many_comparisons.txt << 'EOF'
principal() {
    inteiro !x = 10;
    inteiro !y = 20;
    inteiro !z = 30;
    
    // Muitas comparações para testar operadores relacionais
    se(!x < !y) {
        escreva("x é menor que y");
    }
    
    se(!y > !x) {
        escreva("y é maior que x");
    }
    
    se(!x <= !y) {
        escreva("x é menor ou igual a y");
    }
    
    se(!y >= !x) {
        escreva("y é maior ou igual a x");
    }
    
    se(!x == !x) {
        escreva("x é igual a x");
    }
    
    se(!x <> !y) {
        escreva("x é diferente de y");
    }
    
    se(!x < !y && !y < !z) {
        escreva("x < y < z");
    }
    
    se(!x > !y || !y > !z) {
        escreva("x > y ou y > z");
    }
    
    retorno 0;
}
EOF
bin/compiler /tmp/many_comparisons.txt
echo ""

# Teste 6: Arquivo com muitas funções
echo "7. TESTE 6: Muitas funções"
echo "----------------------------------------"
cat > /tmp/many_functions.txt << 'EOF'
funcao __func1() {
    retorno 1;
}

funcao __func2() {
    retorno 2;
}

funcao __func3() {
    retorno 3;
}

funcao __func4() {
    retorno 4;
}

funcao __func5() {
    retorno 5;
}

funcao __func6() {
    retorno 6;
}

funcao __func7() {
    retorno 7;
}

funcao __func8() {
    retorno 8;
}

funcao __func9() {
    retorno 9;
}

funcao __func10() {
    retorno 10;
}

principal() {
    inteiro !result = __func1() + __func2() + __func3() + __func4() + __func5();
    retorno !result;
}
EOF
bin/compiler /tmp/many_functions.txt
echo ""

# Teste 7: Arquivo com muitos comentários
echo "8. TESTE 7: Muitos comentários"
echo "----------------------------------------"
cat > /tmp/many_comments.txt << 'EOF'
// Comentário 1
// Comentário 2
// Comentário 3
// Comentário 4
// Comentário 5
// Comentário 6
// Comentário 7
// Comentário 8
// Comentário 9
// Comentário 10

principal() {
    // Comentário dentro da função 1
    inteiro !x = 10;
    // Comentário dentro da função 2
    texto !msg = "teste";
    // Comentário dentro da função 3
    retorno 0;
    // Comentário dentro da função 4
}
EOF
bin/compiler /tmp/many_comments.txt
echo ""

# Limpeza
echo "9. Limpeza de arquivos temporários..."
rm -f /tmp/many_vars.txt /tmp/long_strings.txt /tmp/many_numbers.txt
rm -f /tmp/many_operations.txt /tmp/many_comparisons.txt /tmp/many_functions.txt
rm -f /tmp/many_comments.txt

echo ""
echo "=== TESTE DE ESTRESSE FINALIZADO ==="
echo "Todos os testes de estresse foram executados!"
echo "Verifique os relatórios de memória acima para analisar:"
echo "- Fragmentação de memória"
echo "- Uso de memória com strings longas"
echo "- Processamento de muitos tokens"
echo "- Eficiência do gerenciador de memória" 