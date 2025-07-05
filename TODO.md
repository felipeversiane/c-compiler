# Lista de Tarefas do Compilador

## Concluído ✅

### Fase 1: Análise Léxica
- [x] Implementação do analisador léxico (lexer.c)
- [x] Reconhecimento de tokens
- [x] Tratamento de erros léxicos
- [x] Suporte a comentários
- [x] Identificação de palavras-chave
- [x] Identificação de identificadores (variáveis e funções)
- [x] Identificação de literais (números e strings)
- [x] Identificação de operadores e delimitadores

### Fase 2: Gerenciamento de Memória
- [x] Implementação do gerenciador de memória (memory.c)
- [x] Alocação e liberação de memória
- [x] Rastreamento de uso de memória
- [x] Detecção de vazamentos
- [x] Relatórios de uso de memória
- [x] Verificação de limites

### Fase 3: Estruturas de Dados
- [x] Implementação da tabela de símbolos (symbol_table.c)
- [x] Gerenciamento de escopo
- [x] Busca e inserção de símbolos
- [x] Implementação da AST (ast.c)
- [x] Criação e destruição de nós
- [x] Manipulação da árvore

### Fase 4: Análise Sintática
- [x] Implementação do analisador sintático (parser.c)
- [x] Parsing de declarações
- [x] Parsing de expressões
- [x] Parsing de comandos
- [x] Construção da AST
- [x] Tratamento de erros sintáticos

### Fase 5: Infraestrutura
- [x] Configuração do Docker
- [x] Configuração do CI/CD
- [x] Testes automatizados
- [x] Exemplos de código
- [x] Documentação básica

## Em Andamento 🚧

### Fase 6: Melhorias no Parser
- [ ] Leitura completa do arquivo
- [ ] Remoção da restrição de função principal
- [ ] Suporte a múltiplas funções
- [ ] Melhor tratamento de erros
- [ ] Recuperação de erros

### Fase 7: Análise Semântica
- [ ] Implementação do analisador semântico (semantic.c)
- [ ] Verificação de tipos
- [ ] Verificação de escopo
- [ ] Verificação de declarações
- [ ] Tratamento de erros semânticos

## Pendente ⏳

### Fase 8: Interpretador
- [ ] Implementação do interpretador (interpreter.c)
- [ ] Execução de expressões
- [ ] Execução de comandos
- [ ] Execução de funções
- [ ] Entrada/saída
- [ ] Tratamento de erros de execução

### Fase 9: Otimização
- [ ] Implementação de otimizações básicas
- [ ] Eliminação de código morto
- [ ] Propagação de constantes
- [ ] Simplificação de expressões
- [ ] Otimização de loops
