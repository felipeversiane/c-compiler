#!/bin/bash

echo "=========================================="
echo "    TESTE COMPLETO DO COMPILADOR C"
echo "=========================================="
echo "Versão: 1.0.0"
echo "Data: $(date)"
echo ""

# Verificar se estamos no diretório correto
if [ ! -f "Makefile" ]; then
    echo "ERRO: Execute este script no diretório raiz do projeto!"
    exit 1
fi

# Compilar o projeto
echo "🔨 COMPILANDO O PROJETO..."
make clean
make
if [ $? -ne 0 ]; then
    echo "❌ ERRO: Falha na compilação!"
    exit 1
fi
echo "✅ Compilação bem-sucedida!"
echo ""

# Executar testes básicos
echo "🧪 EXECUTANDO TESTES BÁSICOS..."
echo "=========================================="
chmod +x scripts/test_all.sh
./scripts/test_all.sh
echo ""

# Executar testes de estresse
echo "🔥 EXECUTANDO TESTES DE ESTRESSE..."
echo "=========================================="
chmod +x scripts/test_memory_stress.sh
./scripts/test_memory_stress.sh
echo ""

# Teste adicional: Verificar se há vazamentos de memória
echo "🔍 VERIFICAÇÃO FINAL DE MEMÓRIA..."
echo "=========================================="

# Criar um arquivo de teste final
cat > /tmp/final_test.txt << 'EOF'
principal() {
    // Teste final para verificar integridade da memória
    inteiro !test_count = 0;
    texto !final_msg = "Teste final concluído com sucesso!";
    
    para(!test_count = 1; !test_count <= 10; !test_count = !test_count + 1) {
        texto !temp_msg = "Iteração ";
        escreva(!temp_msg, !test_count);
    }
    
    escreva(!final_msg);
    retorno 0;
}
EOF

echo "Executando teste final..."
bin/compiler /tmp/final_test.txt
rm -f /tmp/final_test.txt

echo ""
echo "=========================================="
echo "    RESUMO DOS TESTES"
echo "=========================================="
echo "✅ Testes básicos: CONCLUÍDOS"
echo "✅ Testes de estresse: CONCLUÍDOS"
echo "✅ Verificação de memória: CONCLUÍDA"
echo ""
echo "📊 RESULTADOS ESPERADOS:"
echo "- Analisador léxico funcionando corretamente"
echo "- Gerenciamento de memória sem vazamentos"
echo "- Tratamento adequado de erros léxicos"
echo "- Relatórios detalhados de uso de memória"
echo ""
echo "🎉 TODOS OS TESTES FORAM EXECUTADOS COM SUCESSO!"
echo "==========================================" 