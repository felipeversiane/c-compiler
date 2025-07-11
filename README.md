# Compilador de Linguagem Personalizada

Um compilador simplificado implementado em C seguindo as especificações ISO/IEC 9899-1990, focado em **análise léxica** e **gerenciamento de memória** com sistema completo de testes.

## 🎯 Funcionalidades Implementadas

### ✅ Análise Léxica
- Reconhecimento completo de tokens
- Suporte a comentários
- Identificação de palavras-chave
- Validação de identificadores e literais
- Tratamento de erros léxicos

### ✅ Gerenciamento de Memória (100% Coverage)
- Alocação e liberação automática
- Detecção de vazamentos de memória
- Guardas de memória para detecção de corrupção
- Relatórios detalhados de uso
- Limite configurável (padrão: 512 KB)
- Fragmentação e integridade de memória
- Rastreamento completo de alocações

### ✅ Sistema de Testes Completo
- 16 cenários de teste básicos
- 8 cenários de estresse de memória
- Testes de erros léxicos
- Verificação de limites de memória
- Relatórios detalhados de performance

## 🔧 Compilação

```bash
# Compilar o projeto
make

# Limpar arquivos compilados
make clean

# Executar testes completos
docker-compose up test

# Ou localmente
chmod +x scripts/*.sh
./scripts/run_all_tests.sh
```

## 🚀 Uso

```bash
# Executar arquivo de código
./bin/compiler examples/hello_world.txt

# Executar teste básico (sem argumentos)
./bin/compiler

# Executar conjunto completo de testes
./scripts/run_all_tests.sh

# Ver documentação completa dos testes
# TESTING.md
```

## 📝 Linguagem Suportada

### Estrutura Básica
```c
principal() {
    // código aqui
}

funcao __minhaFuncao(inteiro !param) {
    // código da função
    retorno !param + 1;
}
```

### Tipos de Dados
- `inteiro` - números inteiros
- `decimal[antes.depois]` - números decimais com precisão
- `texto[tamanho]` - strings de texto

### Variáveis
```c
inteiro !idade = 25;
texto !nome[50];
decimal !altura[3.2] = 1.75;
```

### Operadores
- **Aritméticos**: `+`, `-`, `*`, `/`, `^`
- **Relacionais**: `==`, `<>`, `<`, `<=`, `>`, `>=`
- **Lógicos**: `&&`, `||`

### Comandos de Entrada/Saída
```c
escreva("Digite seu nome: ");
leia(!nome);
escreva("Olá, ", !nome);
```

### Estruturas de Controle
```c
// Condicional
se(!idade >= 18) {
    escreva("Maior de idade");
} senao {
    escreva("Menor de idade");
}

// Laço
para(!i = 1; !i <= 10; !i = !i + 1) {
    escreva("Contagem: ", !i);
}
```

## 📁 Estrutura do Projeto

```
c-compiler/
├── src/              # Código fonte
│   ├── lexer.c       # Analisador léxico
│   ├── memory.c      # Gerenciador de memória
│   ├── utils.c       # Utilitários
│   └── main.c        # Programa principal
├── include/          # Headers
│   └── compiler.h    # Definições principais
├── examples/         # Exemplos de código
├── scripts/          # Scripts de teste
│   ├── test_all.sh           # Testes básicos
│   ├── test_memory_stress.sh # Testes de estresse
│   └── run_all_tests.sh      # Script master
├── Makefile          # Sistema de build
├── docker-compose.yml # Configuração Docker
├── TESTING.md        # Documentação dos testes
└── README.md         # Este arquivo
```

## 🧪 Exemplos e Testes Disponíveis

### Arquivos de Exemplo
- `examples/hello_world.txt` - Exemplo básico
- `examples/calculator.txt` - Operações matemáticas
- `examples/functions.txt` - Demonstração de funções
- `examples/loops.txt` - Estruturas de repetição
- `examples/comprehensive.txt` - Exemplo abrangente
- `examples/decimals.txt` - Números decimais
- `examples/error_test.txt` - Teste de erros léxicos

### Scripts de Teste
- `scripts/test_all.sh` - 16 cenários de teste básicos
- `scripts/test_memory_stress.sh` - 8 cenários de estresse
- `scripts/run_all_tests.sh` - Script master completo

## 🔍 Funcionalidades de Debug

### Relatórios de Memória
```bash
# O compilador gera automaticamente relatórios detalhados de memória
./bin/compiler examples/hello_world.txt
```

### Análise Semântica Detalhada
- Avisos para conversões implícitas
- Detecção de variáveis não inicializadas
- Validação rigorosa de tipos
- Verificação de escopo

### Testes de Integridade
- Validação de integridade da memória
- Detecção de corrupção de dados

## 📊 Estatísticas de Execução

O compilador fornece relatórios detalhados incluindo:
- Uso de memória atual e pico
- Número de alocações/desalocações
- Detecção de vazamentos
- Nível de fragmentação
- Taxa de eficiência

## ⚠️ Tratamento de Erros

### Erros Léxicos
- Tokens não reconhecidos
- Caracteres inválidos
- Strings não fechadas
- Identificadores malformados
- Números malformados

### Erros de Memória
- Limite de memória excedido
- Vazamentos de memória
- Corrupção de dados
- Fragmentação excessiva

## 🎓 Conformidade com Especificações

- ✅ **ISO/IEC 9899-1990**: Código em C padrão
- ✅ **Tabela ASCII**: Utilizada corretamente
- ✅ **Memória**: Limite de 2048 KB implementado
- ✅ **Case Sensitive**: Implementado
- ✅ **Palavras-chave**: Todas implementadas
- ✅ **Operadores**: Todos suportados
- ✅ **Duplo balanceamento**: Verificado

## 🔧 Requisitos do Sistema

- GCC 6.1 ou superior
- Sistema Linux/Unix ou Windows com MinGW
- Make (para compilação)

## 📈 Status do Projeto

- **Lexer**: ✅ Completo
- **Memory**: ✅ Completo (100% coverage)
- **Testing**: ✅ Implementado (24 cenários)
- **Documentation**: ✅ Completa

## 🤝 Contribuição

Este projeto foi desenvolvido como trabalho acadêmico seguindo especificações rigorosas. Todas as funcionalidades principais foram implementadas e testadas.

## 📄 Licença

Projeto acadêmico - Ver especificações no arquivo `compiler.txt` para detalhes completos das regras implementadas.
