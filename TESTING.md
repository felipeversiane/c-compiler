# Guia de Testes do Compilador C

Este documento descreve como executar os testes completos do compilador para verificar o funcionamento do analisador léxico e gerenciamento de memória.

## 🚀 Execução Rápida

### Usando Docker (Recomendado)
```bash
# Executar todos os testes
docker-compose up test

# Executar em modo desenvolvimento
docker-compose up dev
```

### Execução Local
```bash
# Dar permissão de execução aos scripts
chmod +x scripts/run_all_tests.sh scripts/test_all.sh scripts/test_memory_stress.sh

# Executar todos os testes
./scripts/run_all_tests.sh

# Executar apenas testes básicos
./scripts/test_all.sh

# Executar apenas testes de estresse
./scripts/test_memory_stress.sh
```

## 📋 Tipos de Teste

### 1. Testes Básicos (`scripts/test_all.sh`)
Executa 16 cenários de teste diferentes:

- **Testes 1-7**: Arquivos de exemplo válidos
  - `hello_world.txt` - Programa básico
  - `calculator.txt` - Operações matemáticas
  - `functions.txt` - Declaração de funções
  - `loops.txt` - Estruturas de repetição
  - `comprehensive.txt` - Teste abrangente
  - `decimals.txt` - Números decimais
  - `error_test.txt` - Erros propositais

- **Testes 8-16**: Cenários de erro e edge cases
  - Arquivo inexistente
  - Arquivo vazio
  - Caracteres inválidos
  - Strings não fechadas
  - Identificadores malformados
  - Números malformados
  - Comentários não suportados
  - Uso intensivo de memória

### 2. Testes de Estresse (`scripts/test_memory_stress.sh`)
Executa 8 cenários de estresse de memória:

- **Teste 1**: Muitas variáveis (teste de fragmentação)
- **Teste 2**: Strings muito longas
- **Teste 3**: Muitos números (inteiros e decimais)
- **Teste 4**: Muitas operações matemáticas
- **Teste 5**: Muitas comparações lógicas
- **Teste 6**: Muitas funções
- **Teste 7**: Muitos comentários

### 3. Teste Master (`scripts/run_all_tests.sh`)
Combina todos os testes acima mais uma verificação final de memória.

## 🔍 O que Verificar nos Resultados

### ✅ Indicadores de Sucesso
- **Compilação**: Sem erros de compilação
- **Tokens**: Reconhecimento correto de todos os tipos de token
- **Memória**: Taxa de liberação de 100%
- **Erros**: Tratamento adequado de erros léxicos
- **Relatórios**: Relatórios detalhados de uso de memória

### 📊 Relatórios de Memória
Cada teste gera relatórios detalhados incluindo:
- Limite total de memória (512 KB)
- Uso atual e pico de uso
- Total de alocações e desalocações
- Nível de fragmentação
- Blocos ativos (deve ser 0 no final)
- Taxa de liberação (deve ser 100%)

### 🚨 Cenários de Erro Esperados
- **Arquivo inexistente**: Erro de arquivo não encontrado
- **Caracteres inválidos**: Tokens de erro para `@#$%`
- **Strings não fechadas**: Erro de string malformada
- **Identificadores malformados**: Erro para variáveis sem `!`
- **Números malformados**: Erro para `123.456.789`

## 🛠️ Personalização de Testes

### Adicionar Novo Arquivo de Teste
1. Crie o arquivo em `examples/`
2. Adicione o teste ao script `scripts/test_all.sh`:

```bash
echo "X. TESTE X: descrição do teste"
echo "----------------------------------------"
bin/compiler examples/novo_arquivo.txt
echo ""
```

### Modificar Limite de Memória
Edite `include/compiler.h`:
```c
#define MAX_MEMORY_KB 512  // Altere para o valor desejado
```

### Ajustar Verbosidade
Edite `include/compiler.h`:
```c
#define DEBUG_MEMORY 1  // 0 para desabilitar, 1 para habilitar
```

## 📈 Interpretação dos Resultados

### Relatório de Tokens
```
=== TOKENS GERADOS ===
Token: principal | Tipo: PRINCIPAL | Linha: 1 | Coluna: 1
Token: ( | Tipo: ABRE_PAREN | Linha: 1 | Coluna: 10
...
=== ESTATÍSTICAS ===
Total de tokens: 90
Erros encontrados: 0
```

### Relatório de Memória
```
=== RELATÓRIO DETALHADO DE MEMÓRIA ===
--- USO ATUAL ---
Memória alocada: 0 bytes (0.00 KB)
Pico de uso de memória: 1756 bytes (1.71 KB)
--- EFICIÊNCIA ---
Taxa de liberação: 100.0% (3/3)
```

## 🐛 Solução de Problemas

### Erro de Compilação
```bash
# Limpar e recompilar
make clean
make
```

### Erro de Permissão
```bash
# Dar permissão aos scripts
chmod +x scripts/*.sh
```

### Erro de Docker
```bash
# Reconstruir container
docker-compose down
docker-compose build --no-cache
docker-compose up test
```

## 📝 Logs e Debug

### Habilitar Debug Detalhado
O sistema já inclui logs detalhados por padrão. Para mais informações, verifique:
- `DEBUG_MEMORY` em `include/compiler.h`
- Logs de alocação/desalocação
- Relatórios de integridade de memória

### Arquivos de Log
Os testes não geram arquivos de log separados, mas toda a saída é exibida no terminal com formatação clara para análise. 