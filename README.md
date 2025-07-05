# Compilador de Linguagem Personalizada

Este é um compilador para uma linguagem de programação personalizada, desenvolvido como projeto acadêmico.

## Estado Atual do Projeto

### Componentes Implementados ✅
- Analisador Léxico (lexer.c)
- Gerenciador de Memória (memory.c)
- Tabela de Símbolos (symbol_table.c)
- Árvore Sintática Abstrata (ast.c)
- Analisador Sintático (parser.c)
- Infraestrutura de Testes (Docker + CI)

### Em Desenvolvimento 🚧
- Melhorias no Parser (leitura completa do arquivo)
- Analisador Semântico (em breve)

## Características da Linguagem

- Tipos de dados: `inteiro`, `texto`, `decimal`
- Variáveis começam com `!` (ex: `!idade`)
- Funções começam com `__` (ex: `__calcular`)
- Suporte a arrays e decimais com precisão definida
- Estruturas de controle: `se`, `senao`, `para`
- Entrada/saída: `leia`, `escreva`

## Exemplo de Código

```
funcao __soma(inteiro !a, inteiro !b) {
    retorno !a + !b;
}

principal() {
    inteiro !x = 10;
    inteiro !y = 20;
    inteiro !resultado;
    
    !resultado = __soma(!x, !y);
    escreva("Soma: ", !resultado);
}
```

## Requisitos

### Para Desenvolvimento Local
- GCC (GNU Compiler Collection)
- GNU Make (opcional)
- Sistema operacional: Windows, Linux ou macOS

### Para Desenvolvimento com Docker
- Docker
- Docker Compose

## Compilação e Execução

### Usando Docker (Recomendado)

1. Build e execução:
   ```bash
   # Construir a imagem
   docker compose build

   # Executar testes
   docker compose run --rm test

   # Ambiente de desenvolvimento
   docker compose run --rm dev
   ```

2. Compilar um arquivo:
   ```bash
   docker compose run --rm dev compiler arquivo_fonte.txt
   ```

### Compilação Local

1. Usando GCC diretamente:
   ```bash
   gcc -o compiler.exe src/*.c -I include
   ```

2. Usando Make (se disponível):
   ```bash
   make
   ```

## Estrutura do Projeto

```
c-compiler/
├── src/             # Código fonte
│   ├── lexer.c     # Analisador léxico
│   ├── parser.c    # Analisador sintático
│   ├── ast.c       # Árvore sintática
│   ├── memory.c    # Gerenciador de memória
│   └── utils.c     # Utilitários
├── include/        # Headers
├── examples/       # Exemplos
└── tests/         # Testes
```

## Funcionalidades Implementadas

### Analisador Léxico
- Reconhecimento de tokens
- Identificação de palavras-chave
- Suporte a identificadores especiais
- Tratamento de strings e números
- Detecção de erros léxicos

### Analisador Sintático
- Parsing de declarações
- Parsing de expressões
- Construção da AST
- Tratamento de erros sintáticos

### Gerenciador de Memória
- Alocação dinâmica
- Detecção de vazamentos
- Relatórios de uso
- Limites de memória

## Próximos Passos

1. Melhorar o parser para:
   - Ler o arquivo completo
   - Remover restrição de função principal
   - Suportar múltiplas funções
   - Melhorar tratamento de erros

2. Implementar análise semântica:
   - Verificação de tipos
   - Verificação de escopo
   - Validação de declarações

## Contribuição

1. Fork o repositório
2. Crie uma branch para sua feature
3. Execute os testes
4. Envie um pull request

## Licença

Este projeto é licenciado sob a licença MIT - veja o arquivo LICENSE para detalhes.
