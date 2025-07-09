# Compilador de Linguagem Personalizada

Um compilador completo implementado em C seguindo as especificações ISO/IEC 9899-1990, com análise léxica, sintática, semântica e interpretação de código.

## 🎯 Funcionalidades Implementadas

### ✅ Análise Léxica
- Reconhecimento completo de tokens
- Suporte a comentários
- Identificação de palavras-chave
- Validação de identificadores e literais
- Tratamento de erros léxicos

### ✅ Análise Sintática
- Parser completo para a gramática da linguagem
- Construção de AST (Árvore Sintática Abstrata)
- Tratamento de erros sintáticos com recuperação
- Suporte a múltiplas funções

### ✅ Análise Semântica
- Verificação rigorosa de tipos
- Validação de escopo
- Verificação de declarações de variáveis e funções
- Avisos para conversões implícitas de tipos
- Validação de nomes conforme especificações

### ✅ Gerenciamento de Memória (100% Coverage)
- Alocação e liberação automática
- Detecção de vazamentos de memória
- Guardas de memória para detecção de corrupção
- Testes de estresse automáticos
- Relatórios detalhados de uso
- Limite configurável (padrão: 2048 KB)

### ✅ Interpretador
- Execução de código fonte
- Suporte a todos os tipos de dados
- Operações aritméticas, relacionais e lógicas
- Comandos de entrada e saída
- Gerenciamento de variáveis em runtime

## 🔧 Compilação

```bash
# Compilar o projeto
make

# Limpar arquivos compilados
make clean

# Executar testes
make test
```

## 🚀 Uso

```bash
# Executar arquivo de código
./bin/compiler examples/hello_world.txt

# Executar teste básico (sem argumentos)
./bin/compiler
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
│   ├── parser.c      # Analisador sintático
│   ├── semantic.c    # Analisador semântico
│   ├── interpreter.c # Interpretador
│   ├── memory.c      # Gerenciador de memória
│   ├── symbol_table.c # Tabela de símbolos
│   ├── ast.c         # Árvore sintática abstrata
│   ├── utils.c       # Utilitários
│   └── main.c        # Programa principal
├── include/          # Headers
│   └── compiler.h    # Definições principais
├── examples/         # Exemplos de código
├── tests/           # Testes automatizados
├── Makefile         # Sistema de build
└── README.md        # Este arquivo
```

## 🧪 Exemplos Disponíveis

- `examples/hello_world.txt` - Exemplo básico
- `examples/comprehensive.txt` - Exemplo completo com funções
- `examples/calculator.txt` - Calculadora simples
- `examples/functions.txt` - Demonstração de funções
- `examples/loops.txt` - Estruturas de repetição

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
- Testes de estresse de memória automáticos
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

### Erros Sintáticos
- Estruturas malformadas
- Pontuação ausente
- Duplo balanceamento

### Erros Semânticos
- Tipos incompatíveis
- Variáveis não declaradas
- Função principal ausente

### Erros de Execução
- Divisão por zero
- Acesso a variáveis não inicializadas
- Estouro de memória

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
- **Parser**: ✅ Completo  
- **Semantic**: ✅ Completo
- **Memory**: ✅ Completo (100% coverage)
- **Interpreter**: ✅ Funcional
- **Testing**: ✅ Implementado

## 🤝 Contribuição

Este projeto foi desenvolvido como trabalho acadêmico seguindo especificações rigorosas. Todas as funcionalidades principais foram implementadas e testadas.

## 📄 Licença

Projeto acadêmico - Ver especificações no arquivo `compiler.txt` para detalhes completos das regras implementadas.
