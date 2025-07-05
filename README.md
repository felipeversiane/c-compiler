# Compilador de Linguagem Personalizada

Este é um compilador para uma linguagem de programação personalizada, desenvolvido como projeto acadêmico.

## Estado Atual do Projeto

### Componentes Implementados ✅
- Analisador Léxico (lexer.c)
- Gerenciador de Memória (memory.c)
- Tabela de Símbolos (symbol_table.c)
- Árvore Sintática Abstrata (ast.c)
- Analisador Sintático (parser.c)

### Em Desenvolvimento 🚧
- Analisador Semântico
- Interpretador
- Otimizador

## Características da Linguagem

- Tipos de dados: `inteiro`, `texto`, `decimal`
- Variáveis começam com `!` (ex: `!idade`)
- Funções começam com `__` (ex: `__calcular`)
- Suporte a arrays e decimais com precisão definida
- Estruturas de controle: `se`, `senao`, `para`
- Entrada/saída: `leia`, `escreva`

## Exemplo de Código

```
principal() {
    inteiro !idade = 25;
    texto !nome[50];
    decimal !altura[3.2];
    
    escreva("Digite seu nome: ");
    leia(!nome);
    
    se(!idade >= 18) {
        escreva("Maior de idade");
    } senao {
        escreva("Menor de idade");
    }
}
```

## Requisitos

- GCC (GNU Compiler Collection)
- GNU Make (opcional)
- Sistema operacional: Windows, Linux ou macOS

## Compilação

### Usando GCC diretamente:
```bash
gcc -o compiler.exe src/*.c -I include
```

### Usando Make (se disponível):
```bash
make
```

## Uso

Para compilar um arquivo fonte:

```bash
compiler.exe arquivo_fonte.txt
```

## Exemplos Disponíveis

O projeto inclui vários exemplos na pasta `examples/`:

- `hello_world.txt` - Exemplo básico
- `calculator.txt` - Calculadora com operações básicas
- `loops.txt` - Demonstração de estruturas de repetição
- `comprehensive.txt` - Exemplo completo com várias funcionalidades
- `error_test.txt` - Testes de detecção de erros

## Estrutura do Projeto

```
c-compiler/
├── src/
│   ├── lexer.c         # Analisador léxico
│   ├── parser.c        # Analisador sintático
│   ├── ast.c          # Árvore sintática abstrata
│   ├── symbol_table.c  # Tabela de símbolos
│   ├── memory.c       # Gerenciador de memória
│   ├── utils.c        # Funções utilitárias
│   └── main.c         # Programa principal
├── include/
│   └── compiler.h     # Definições e estruturas
├── examples/          # Exemplos de código
└── docs/             # Documentação (em breve)
```

## Funcionalidades Implementadas

### Analisador Léxico
- Reconhecimento de tokens
- Identificação de palavras-chave
- Suporte a identificadores especiais
- Tratamento de strings e números
- Detecção de erros léxicos

### Gerenciador de Memória
- Alocação dinâmica
- Detecção de vazamentos
- Relatórios de uso
- Limites de memória

### Analisador Sintático
- Parsing de declarações
- Parsing de expressões
- Construção da AST
- Tratamento de erros sintáticos

### Tabela de Símbolos
- Gerenciamento de escopo
- Rastreamento de símbolos
- Verificação de duplicatas

## Próximos Passos

1. Implementação do analisador semântico
2. Desenvolvimento do interpretador
3. Adição de otimizações
4. Melhorias na detecção de erros
5. Documentação completa

## Contribuição

Para contribuir com o projeto:

1. Faça um fork do repositório
2. Crie uma branch para sua feature
3. Faça commit das suas alterações
4. Envie um pull request

## Licença

Este projeto é licenciado sob a licença MIT - veja o arquivo LICENSE para detalhes.
