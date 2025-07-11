#!/bin/bash

echo "=== TESTE COMPLETO DO COMPILADOR ==="
echo "Versão: 1.0.0"
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

# Teste 1: Arquivo válido básico
echo "2. TESTE 1: hello_world.txt (arquivo válido básico)"
echo "----------------------------------------"
bin/compiler examples/hello_world.txt
echo ""

# Teste 2: Arquivo com cálculos
echo "3. TESTE 2: calculator.txt (cálculos e operações)"
echo "----------------------------------------"
bin/compiler examples/calculator.txt
echo ""

# Teste 3: Arquivo com funções
echo "4. TESTE 3: functions.txt (declaração de funções)"
echo "----------------------------------------"
bin/compiler examples/functions.txt
echo ""

# Teste 4: Arquivo com loops
echo "5. TESTE 4: loops.txt (estruturas de repetição)"
echo "----------------------------------------"
bin/compiler examples/loops.txt
echo ""

# Teste 5: Arquivo abrangente
echo "6. TESTE 5: comprehensive.txt (teste abrangente)"
echo "----------------------------------------"
bin/compiler examples/comprehensive.txt
echo ""

# Teste 6: Arquivo com decimais
echo "7. TESTE 6: decimals.txt (números decimais)"
echo "----------------------------------------"
bin/compiler examples/decimals.txt
echo ""

# Teste 7: Arquivo com erros (deve mostrar erros léxicos)
echo "8. TESTE 7: error_test.txt (teste de erros léxicos)"
echo "----------------------------------------"
bin/compiler examples/error_test.txt
echo ""

# Teste 8: Arquivo inexistente (teste de erro de arquivo)
echo "9. TESTE 8: arquivo_inexistente.txt (erro de arquivo)"
echo "----------------------------------------"
bin/compiler examples/arquivo_inexistente.txt
echo ""

# Teste 9: Arquivo vazio
echo "10. TESTE 9: arquivo vazio"
echo "----------------------------------------"
echo "" > /tmp/empty.txt
bin/compiler /tmp/empty.txt
echo ""

# Teste 10: Arquivo com caracteres inválidos
echo "11. TESTE 10: caracteres inválidos"
echo "----------------------------------------"
cat > /tmp/invalid_chars.txt << 'EOF'
principal() {
    inteiro !x = @#$%;  // Caracteres inválidos
    texto !y = "teste";
    retorno 0;
}
EOF
bin/compiler /tmp/invalid_chars.txt
echo ""

# Teste 11: Arquivo com strings não fechadas
echo "12. TESTE 11: strings não fechadas"
echo "----------------------------------------"
cat > /tmp/unclosed_string.txt << 'EOF'
principal() {
    escreva("String não fechada
    retorno 0;
}
EOF
bin/compiler /tmp/unclosed_string.txt
echo ""

# Teste 12: Arquivo com identificadores malformados
echo "13. TESTE 12: identificadores malformados"
echo "----------------------------------------"
cat > /tmp/bad_identifiers.txt << 'EOF'
principal() {
    inteiro idade = 25;  // Falta !
    texto nome = "teste";  // Falta !
    funcao __teste() {  // OK
        retorno 0;
    }
    retorno 0;
}
EOF
bin/compiler /tmp/bad_identifiers.txt
echo ""

# Teste 13: Arquivo com números malformados
echo "14. TESTE 13: números malformados"
echo "----------------------------------------"
cat > /tmp/bad_numbers.txt << 'EOF'
principal() {
    inteiro !x = 123.456.789;  // Número malformado
    decimal !y = 123..456;     // Número malformado
    retorno 0;
}
EOF
bin/compiler /tmp/bad_numbers.txt
echo ""

# Teste 14: Arquivo com comentários aninhados (não suportado)
echo "15. TESTE 14: comentários aninhados"
echo "----------------------------------------"
cat > /tmp/nested_comments.txt << 'EOF'
principal() {
    // Comentário normal
    /* Comentário não suportado */
    inteiro !x = 10;
    retorno 0;
}
EOF
bin/compiler /tmp/nested_comments.txt
echo ""

# Teste 15: Arquivo com uso intensivo de memória
echo "16. TESTE 15: uso intensivo de memória"
echo "----------------------------------------"
cat > /tmp/memory_test.txt << 'EOF'
principal() {
    // Gerar muitos tokens para testar gerenciamento de memória
    inteiro !a = 1;
    inteiro !b = 2;
    inteiro !c = 3;
    inteiro !d = 4;
    inteiro !e = 5;
    inteiro !f = 6;
    inteiro !g = 7;
    inteiro !h = 8;
    inteiro !i = 9;
    inteiro !j = 10;
    
    texto !msg1 = "Mensagem muito longa para testar alocação de memória em strings com muitos caracteres";
    texto !msg2 = "Outra mensagem longa para continuar testando o gerenciamento de memória do compilador";
    texto !msg3 = "Terceira mensagem para garantir que o sistema de memória está funcionando corretamente";
    
    escreva(!msg1);
    escreva(!msg2);
    escreva(!msg3);
    
    retorno 0;
}
EOF
bin/compiler /tmp/memory_test.txt
echo ""

# Limpeza
echo "17. Limpeza de arquivos temporários..."
rm -f /tmp/empty.txt /tmp/invalid_chars.txt /tmp/unclosed_string.txt
rm -f /tmp/bad_identifiers.txt /tmp/bad_numbers.txt /tmp/nested_comments.txt
rm -f /tmp/memory_test.txt

echo ""
echo "=== TESTE COMPLETO FINALIZADO ==="
echo "Todos os testes foram executados com sucesso!"
echo "Verifique os resultados acima para analisar:"
echo "- Reconhecimento correto de tokens"
echo "- Tratamento de erros léxicos"
echo "- Gerenciamento de memória"
echo "- Relatórios de uso de memória" 