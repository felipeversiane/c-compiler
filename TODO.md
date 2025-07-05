# Compilador de Linguagem Personalizada - TODO de Desenvolvimento

## Fase 1: Infraestrutura Principal

### 1. Analisador Léxico (Tokenizador)
- [ ] Definir enum de tipos de tokens (palavras-chave, identificadores, operadores, literais, delimitadores)
- [ ] Implementar estrutura de token com tipo, valor, número da linha e coluna
- [ ] Criar funções de varredura de caracteres
- [ ] Implementar reconhecimento de palavras-chave (principal, funcao, leia, escreva, se, senao, para, retorno)
- [ ] Implementar reconhecimento de identificadores (variáveis com prefixo `!`, funções com prefixo `__`)
- [ ] Implementar reconhecimento de operadores (`+`, `-`, `*`, `/`, `^`, `==`, `<>`, `<`, `<=`, `>`, `>=`, `&&`, `||`)
- [ ] Implementar reconhecimento de literais numéricos (inteiros e decimais)
- [ ] Implementar reconhecimento de literais de string (texto entre aspas duplas)
- [ ] Implementar reconhecimento de delimitadores (`{}`, `()`, `[]`, `;`, `,`)
- [ ] Tratar espaços em branco e comentários
- [ ] Implementar relatório de erros para caracteres inválidos
- [ ] Adicionar rastreamento de linha e coluna para mensagens de erro

### 2. Gerenciamento da Tabela de Símbolos
- [ ] Projetar estrutura da tabela de símbolos (hash table ou árvore recomendada)
- [ ] Implementar estrutura de entrada de símbolo (nome, tipo, valor, escopo, linha_declarada)
- [ ] Criar funções de inserção de símbolos
- [ ] Criar funções de busca de símbolos
- [ ] Implementar gerenciamento de escopo (escopos globais e de função)
- [ ] Adicionar suporte para tipos de variáveis (inteiro, texto, decimal)
- [ ] Adicionar suporte para símbolos de função com parâmetros
- [ ] Implementar função de exibição da tabela de símbolos
- [ ] Adicionar rastreamento de uso de memória por símbolo
- [ ] Implementar funções de validação de símbolos

### 3. Gerenciador de Memória
- [ ] Implementar wrapper de alocação dinâmica de memória
- [ ] Criar rastreamento de uso de memória (atual, pico, limite = 2048KB)
- [ ] Implementar funções de alocação de memória com verificação de tamanho
- [ ] Implementar funções de desalocação de memória
- [ ] Adicionar suporte para realocação de memória
- [ ] Criar relatório de uso de memória
- [ ] Implementar sistema de aviso de memória (uso 90-99%)
- [ ] Adicionar tratamento de erro de memória insuficiente (uso 100%)
- [ ] Implementar limpeza de memória na terminação do programa

## Fase 2: Análise Sintática

### 4. Definição da Gramática
- [ ] Definir gramática BNF/EBNF para a linguagem personalizada
- [ ] Criar regras de produção para estrutura do programa
- [ ] Definir regras para declarações de função
- [ ] Definir regras para declarações de variável
- [ ] Definir regras para comandos (atribuição, fluxo de controle)
- [ ] Definir regras para expressões (aritméticas, relacionais, lógicas)
- [ ] Definir regras para chamadas de função
- [ ] Definir precedência e associatividade de operadores
- [ ] Documentar regras gramaticais em notação formal

### 5. Analisador Sintático (Parser)
- [ ] Escolher método de parsing (descida recursiva recomendada)
- [ ] Implementar estruturas de nós AST para diferentes tipos de comandos
- [ ] Criar funções de parsing para cada regra gramatical
- [ ] Implementar parsing do programa (deve conter função principal())
- [ ] Implementar parsing de declaração de função
- [ ] Implementar parsing de declaração de variável com verificação de tipo
- [ ] Implementar parsing de comandos (atribuições, fluxo de controle)
- [ ] Implementar parsing de expressões com tratamento de precedência
- [ ] Implementar verificação de delimitadores balanceados
- [ ] Adicionar relatório de erros sintáticos com números de linha
- [ ] Implementar construção da AST durante o parsing
- [ ] Adicionar mecanismos de recuperação para tratamento de erros

## Fase 3: Análise Semântica

### 6. Sistema de Verificação de Tipos
- [ ] Implementar verificação de compatibilidade de tipos
- [ ] Criar funções de conversão de tipos quando aplicável
- [ ] Validar declarações de variáveis contra uso
- [ ] Verificar tipos e quantidades de parâmetros de função
- [ ] Validar tipos de retorno para funções
- [ ] Implementar verificação de tipos em atribuições
- [ ] Adicionar validação de tipos em expressões
- [ ] Verificar limites de array para tipos texto e decimal
- [ ] Implementar sistema de avisos semânticos
- [ ] Adicionar validação de escopo para acesso a variáveis

### 7. Analisador Semântico
- [ ] Implementar população da tabela de símbolos durante o parsing
- [ ] Adicionar validação de uso de variáveis
- [ ] Implementar validação de chamadas de função
- [ ] Verificar variáveis não declaradas
- [ ] Validar assinaturas de função
- [ ] Implementar verificação de escopo para funções aninhadas
- [ ] Adicionar detecção de declarações duplicadas
- [ ] Validar estrutura de fluxo de controle
- [ ] Implementar relatório de erros e avisos semânticos
- [ ] Adicionar verificação de referências cruzadas

## Fase 4: Execução de Código

### 8. Avaliador de Expressões
- [ ] Implementar avaliação de expressões aritméticas (`+`, `-`, `*`, `/`, `^`)
- [ ] Implementar avaliação de expressões relacionais (`==`, `<>`, `<`, `<=`, `>`, `>=`)
- [ ] Implementar avaliação de expressões lógicas (`&&`, `||`)
- [ ] Adicionar tratamento de precedência de operadores
- [ ] Implementar suporte a parênteses para override de precedência
- [ ] Adicionar coerção de tipos para operações de tipos mistos
- [ ] Implementar resolução de variáveis em expressões
- [ ] Adicionar avaliação de chamadas de função em expressões
- [ ] Tratar operações de indexação de array/string
- [ ] Implementar otimização de expressões quando possível

### 9. Funções Built-in
- [ ] Implementar função `leia()` para entrada
  - [ ] Suportar múltiplas variáveis em uma única chamada
  - [ ] Tratar diferentes tipos de dados (inteiro, texto, decimal)
  - [ ] Adicionar validação de entrada e tratamento de erros
- [ ] Implementar função `escreva()` para saída
  - [ ] Suportar mistura de texto e variáveis
  - [ ] Tratar formatação adequada para diferentes tipos
  - [ ] Suportar múltiplas saídas separadas por vírgula
- [ ] Adicionar validação adequada de parâmetros
- [ ] Implementar formatação de entrada/saída específica por tipo

### 10. Estruturas de Controle
- [ ] Implementar execução do comando `se` (if)
  - [ ] Avaliação de condição
  - [ ] Seleção de ramificação
  - [ ] Suporte a comando único e bloco
- [ ] Implementar execução do comando `senao` (else)
- [ ] Implementar execução do laço `para` (for)
  - [ ] Comando de inicialização
  - [ ] Verificação de condição
  - [ ] Expressão de atualização
  - [ ] Execução do corpo do laço
- [ ] Adicionar suporte para estruturas de controle aninhadas
- [ ] Implementar gerenciamento adequado de escopo para blocos

### 11. Sistema de Funções
- [ ] Implementar chamadas de funções definidas pelo usuário
- [ ] Criar mecanismo de passagem de parâmetros de função
- [ ] Implementar gerenciamento de escopo de variáveis locais
- [ ] Adicionar tratamento de valores de retorno de função
- [ ] Implementar suporte para funções recursivas
- [ ] Adicionar busca e validação de funções
- [ ] Implementar gerenciamento adequado de pilha para chamadas de função
- [ ] Adicionar verificação de tipos de parâmetros de função em tempo de execução

### 12. Motor de Interpretação
- [ ] Criar loop principal de execução
- [ ] Implementar dispatcher de execução de comandos
- [ ] Adicionar gerenciamento de contador de programa
- [ ] Implementar execução de atribuições de variáveis
- [ ] Adicionar execução de chamadas de função
- [ ] Implementar execução de fluxo de controle
- [ ] Adicionar tratamento de erros em tempo de execução
- [ ] Implementar condições de terminação do programa

## Fase 5: Tratamento de Erros e Relatórios

### 13. Sistema de Gerenciamento de Erros
- [ ] Projetar hierarquia de tipos de erro (léxico, sintático, semântico, tempo de execução)
- [ ] Implementar formatação de mensagens de erro
- [ ] Adicionar rastreamento de número de linha e coluna
- [ ] Criar informações de contexto de erro
- [ ] Implementar mecanismos de recuperação de erros
- [ ] Adicionar capacidades de log de erros
- [ ] Implementar terminação graciosa do programa em erros

### 14. Sistema de Avisos
- [ ] Implementar detecção de avisos semânticos
- [ ] Adicionar formatação de mensagens de aviso
- [ ] Criar mecanismos de supressão de avisos
- [ ] Implementar configuração de nível de avisos
- [ ] Adicionar rastreamento de estatísticas de avisos

## Fase 6: Programa Principal e Integração

### 15. Ponto de Entrada do Programa Principal
- [ ] Implementar parsing de argumentos de linha de comando
- [ ] Adicionar leitura e validação de arquivo fonte
- [ ] Criar integração do pipeline do compilador
- [ ] Implementar execução das fases de compilação
- [ ] Adicionar controle de execução do programa
- [ ] Implementar limpeza de memória na saída
- [ ] Adicionar informações de versão e ajuda

### 16. Gerenciamento de E/S de Arquivo
- [ ] Implementar leitura de arquivo fonte
- [ ] Adicionar validação de formato de arquivo
- [ ] Implementar tratamento de codificação de caracteres
- [ ] Adicionar tratamento de erros de arquivo
- [ ] Implementar leitura bufferizada para arquivos grandes

## Fase 7: Testes e Validação

### 17. Testes Unitários
- [ ] Criar casos de teste para analisador léxico
- [ ] Criar casos de teste para operações da tabela de símbolos
- [ ] Criar casos de teste para gerenciador de memória
- [ ] Criar casos de teste para parser
- [ ] Criar casos de teste para analisador semântico
- [ ] Criar casos de teste para avaliador de expressões
- [ ] Criar casos de teste para estruturas de controle
- [ ] Criar casos de teste para sistema de funções

### 18. Testes de Integração
- [ ] Criar casos de teste de programas completos
- [ ] Testar cenários de tratamento de erros
- [ ] Testar gerenciamento de memória sob carga
- [ ] Testar chamadas de função aninhadas
- [ ] Testar expressões complexas
- [ ] Testar casos extremos e condições de fronteira

### 19. Testes de Performance
- [ ] Testar padrões de uso de memória
- [ ] Testar benchmarks de velocidade de execução
- [ ] Testar tratamento de programas grandes
- [ ] Testar cenários de recursão profunda
- [ ] Testar eficiência de alocação/desalocação de memória

## Fase 8: Documentação e Finalização

### 20. Documentação do Código
- [ ] Adicionar comentários abrangentes no código
- [ ] Documentar funções da API
- [ ] Criar documentação de interface de módulos
- [ ] Documentar estruturas de dados
- [ ] Adicionar notas de complexidade algorítmica

### 21. Documentação do Usuário
- [ ] Completar README com exemplos
- [ ] Criar manual do usuário
- [ ] Documentar mensagens de erro e soluções
- [ ] Adicionar guia de solução de problemas
- [ ] Criar guia de referência da linguagem

### 22. Integração Final e Testes
- [ ] Realizar testes abrangentes do sistema
- [ ] Validar contra todos os requisitos da especificação
- [ ] Testar compilação em ambientes alvo
- [ ] Realizar detecção de vazamentos de memória
- [ ] Revisão final do código e limpeza

## Ordem de Prioridade de Implementação:
1. Analisador Léxico + Tabela de Símbolos + Gerenciador de Memória (Fundação)
2. Definição da Gramática + Analisador Sintático (Parsing)
3. Analisador Semântico + Verificação de Tipos (Validação)
4. Avaliador de Expressões + Funções Built-in (Funcionalidade principal)
5. Estruturas de Controle + Sistema de Funções (Características avançadas)
6. Motor de Interpretação + Tratamento de Erros (Execução)
7. Programa Principal + Integração (Sistema completo)
8. Testes + Documentação (Garantia de qualidade)

Cada fase deve ser completada e testada antes de prosseguir para a próxima fase.
