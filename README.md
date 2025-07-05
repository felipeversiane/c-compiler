# Compilador de Linguagem Personalizada

Uma implementação de compilador para uma linguagem de programação personalizada escrita em C, seguindo os padrões ISO/IEC 9899-1990.

## Visão Geral

Este projeto implementa um compilador completo para uma linguagem de programação personalizada que suporta:
- Declarações de variáveis (inteiro, texto, decimal)
- Definições e chamadas de funções
- Estruturas de controle (se/senão, laços para)
- Operações de entrada/saída
- Expressões matemáticas e lógicas
- Gerenciamento de memória com alocação dinâmica

## Características da Linguagem

### Tipos de Dados
- **inteiro** - Números inteiros
- **texto** - Strings com tamanho especificado `[tamanho]`
- **decimal** - Números de ponto flutuante com precisão especificada `[antes.depois]`

### Palavras-Chave
- `principal()` - Função principal (obrigatória)
- `funcao` - Definição de função
- `leia()` - Operação de entrada
- `escreva()` - Operação de saída
- `se()` - Comando condicional
- `senao` - Comando senão
- `para()` - Laço para
- `retorno` - Comando de retorno

### Nomenclatura de Variáveis
- Variáveis devem começar com `!` seguido de uma letra minúscula
- Podem conter letras (a-z, A-Z) e números (0-9) após o primeiro caractere
- Exemplos: `!a`, `!contador`, `!valor1`

### Nomenclatura de Funções
- Funções devem começar com `__` seguido de uma letra ou número
- Podem conter letras e números após o prefixo
- Exemplos: `__soma`, `__calcula1`, `__verificar`

### Operadores
- **Aritméticos**: `+`, `-`, `*`, `/`, `^` (exponenciação)
- **Relacionais**: `==`, `<>`, `<`, `<=`, `>`, `>=`
- **Lógicos**: `&&` (e), `||` (ou)

## Requisitos de Compilação

### Compiladores Suportados
- **Linux**: GCC versão 6.1 ou inferior
- **Windows**: 
  - Dev-C++ 4.9.9.2
  - Code::Blocks 20.03

### Restrições de Memória
- Uso máximo de memória: 2048 KB
- Alocação dinâmica de memória obrigatória
- Rastreamento e relatório de uso de memória

## Uso

### Estrutura Básica do Programa
```c
principal(){
    inteiro !a, !b = 10;
    texto !nome[20];
    decimal !preco[5.2];
    
    escreva("Digite um número: ");
    leia(!a);
    
    se(!a > !b)
        escreva("A é maior que B");
    senao
        escreva("B é maior ou igual a A");
}
```

### Definição de Função
```c
funcao __soma(inteiro !x, inteiro !y){
    inteiro !resultado;
    !resultado = !x + !y;
    retorno !resultado;
}

principal(){
    inteiro !a = 5, !b = 3, !c;
    !c = __soma(!a, !b);
    escreva("Resultado: ", !c);
}
```

### Estruturas de Controle
```c
principal(){
    inteiro !i;
    
    // Laço para
    para(!i = 1; !i <= 10; !i++){
        escreva("Número: ", !i);
    }
    
    // Condicional
    se(!i > 5 && !i < 15)
        escreva("i está entre 5 e 15");
}
```

## Tratamento de Erros

### Erros Léxicos e Sintáticos
- Programa termina com mensagem de erro e número da linha
- Exemplos: Tokens inválidos, sintaxe incorreta

### Avisos Semânticos
- Programa continua execução mas mostra avisos
- Exemplos: Incompatibilidade de tipos, variáveis não declaradas

### Erros de Memória
- **Memória Insuficiente**: Termina quando limite de memória é excedido
- **Aviso de Memória**: Alerta quando uso está entre 90-99% do limite

## Tabela de Símbolos

O compilador mantém uma tabela de símbolos contendo:
- Tipo da variável
- Nome da variável
- Valor atual
- Escopo da função/módulo

## Detalhes da Implementação

### Fases do Compilador
1. **Análise Léxica** - Tokenização
2. **Análise Sintática** - Parsing e construção da AST
3. **Análise Semântica** - Verificação de tipos e validação de escopo
4. **Geração/Interpretação de Código** - Execução

### Requisitos Principais
- Linguagem sensível a maiúsculas/minúsculas
- Delimitadores balanceados: `{}`, `()`, `[]`, `""`
- Gerenciamento adequado de memória
- Relatório completo de erros

## Compilação e Execução

1. Compilar o compilador:
```bash
gcc -o compilador main.c lexer.c parser.c semantic.c interpreter.c memory.c
```

2. Executar com arquivo fonte:
```bash
./compilador programa.txt
```

## Testes

O compilador inclui casos de teste abrangentes cobrindo:
- Todas as características da linguagem
- Condições de erro
- Casos extremos
- Cenários de gerenciamento de memória

## Estrutura do Projeto

```
c-compiler/
├── src/
│   ├── main.c          # Ponto de entrada do programa
│   ├── lexer.c         # Analisador léxico
│   ├── parser.c        # Analisador sintático
│   ├── semantic.c      # Analisador semântico
│   ├── interpreter.c   # Interpretador de código
│   ├── memory.c        # Gerenciador de memória
│   └── symbol_table.c  # Implementação da tabela de símbolos
├── include/
│   └── compiler.h      # Definições de cabeçalho
├── tests/
│   └── test_cases/     # Programas de teste
├── examples/
│   └── sample_programs/ # Programas de exemplo
└── README.md
```

## Licença

Este projeto é desenvolvido para fins acadêmicos seguindo as especificações fornecidas nos requisitos do curso.
