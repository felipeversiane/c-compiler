# Instruções de Compilação para Compilador de Linguagem Personalizada

## Requisitos de Ambiente

Este compilador foi projetado para funcionar com ambientes de desenvolvimento específicos conforme os requisitos do projeto:

### Ambiente Linux
- **Obrigatório**: GCC versão 6.1 ou inferior
- **Padrão**: ISO/IEC 9899-1990 (C90)

### Ambiente Windows
- **Opção 1**: Dev-C++ 4.9.9.2 (versão instalável)
- **Opção 2**: Code::Blocks 20.03

## Compilação no Linux (GCC)

### Pré-requisitos
```bash
# Verificar versão do GCC (deve ser 6.1 ou inferior)
gcc --version

# Se você tem uma versão mais nova, pode precisar instalar uma mais antiga
# Exemplo Ubuntu/Debian:
sudo apt-get install gcc-6

# Definir GCC 6 como padrão (se necessário)
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-6 60
```

### Comandos de Compilação

#### Usando Makefile (Recomendado)
```bash
# Compilar o compilador
make

# Compilar com símbolos de debug
make debug

# Compilar versão otimizada de release
make release

# Limpar artefatos de build
make clean

# Executar testes
make test

# Instalar no sistema
make install
```

#### Compilação Manual
```bash
# Criar diretório de build
mkdir -p build

# Compilar módulos individuais
gcc -Wall -Wextra -std=c90 -pedantic -g -Iinclude -c src/main.c -o build/main.o
gcc -Wall -Wextra -std=c90 -pedantic -g -Iinclude -c src/lexer.c -o build/lexer.o
gcc -Wall -Wextra -std=c90 -pedantic -g -Iinclude -c src/parser.c -o build/parser.o
gcc -Wall -Wextra -std=c90 -pedantic -g -Iinclude -c src/semantic.c -o build/semantic.o
gcc -Wall -Wextra -std=c90 -pedantic -g -Iinclude -c src/interpreter.c -o build/interpreter.o
gcc -Wall -Wextra -std=c90 -pedantic -g -Iinclude -c src/memory.c -o build/memory.o
gcc -Wall -Wextra -std=c90 -pedantic -g -Iinclude -c src/symbol_table.c -o build/symbol_table.o

# Linkar executável
gcc build/*.o -lm -o compilador
```

## Compilação no Windows

### Opção 1: Dev-C++ 4.9.9.2

1. **Instalar Dev-C++**
   - Baixar Dev-C++ 4.9.9.2 do site oficial
   - Instalar com configurações padrão

2. **Criar Novo Projeto**
   - Arquivo → Novo → Projeto
   - Selecionar "Console Application"
   - Escolher "C Project"
   - Nomear o projeto como "compilador"

3. **Adicionar Arquivos Fonte**
   - Projeto → Adicionar ao Projeto
   - Adicionar todos os arquivos .c do diretório `src/`
   - Adicionar todos os arquivos .h do diretório `include/`

4. **Configurar Definições do Compilador**
   - Ferramentas → Opções do Compilador
   - Definir padrão C para C90/C89
   - Adicionar diretório de include: `include/`
   - Habilitar avisos: `-Wall -Wextra -pedantic`

5. **Compilar Projeto**
   - Executar → Compilar (Ctrl+F9)
   - Executar → Executar (Ctrl+F10)

### Opção 2: Code::Blocks 20.03

1. **Instalar Code::Blocks**
   - Baixar Code::Blocks 20.03 do site oficial
   - Instalar com compilador MinGW

2. **Criar Novo Projeto**
   - Arquivo → Novo → Projeto
   - Selecionar "Console Application"
   - Escolher linguagem "C"
   - Definir nome do projeto: "compilador"

3. **Configurar Definições do Projeto**
   - Projeto → Propriedades
   - Alvos de build → Release/Debug
   - Configurações do compilador → Outras opções: `-std=c90 -Wall -Wextra -pedantic`
   - Diretórios de busca → Compilador → Adicionar: `include/`

4. **Adicionar Arquivos Fonte**
   - Clique direito no projeto → Adicionar arquivos
   - Adicionar todos os arquivos .c do diretório `src/`
   - Adicionar todos os arquivos .h do diretório `include/`

5. **Compilar Projeto**
   - Build → Build (Ctrl+F9)
   - Build → Executar (Ctrl+F10)

## Configuração da Estrutura do Projeto

Antes de compilar, certifique-se de que seu projeto tem a seguinte estrutura:

```
c-compiler/
├── src/
│   ├── main.c
│   ├── lexer.c
│   ├── parser.c
│   ├── semantic.c
│   ├── interpreter.c
│   ├── memory.c
│   └── symbol_table.c
├── include/
│   └── compiler.h
├── examples/
│   ├── hello_world.txt
│   ├── calculator.txt
│   ├── loops.txt
│   ├── decimals.txt
│   ├── comprehensive.txt
│   └── error_test.txt
├── tests/
│   └── (arquivos de teste)
├── build/
│   └── (artefatos de build)
├── Makefile
├── README.md
└── TODO.md
```

## Configuração de Memória

O compilador está configurado com um limite máximo de memória de 2048 KB conforme especificado nos requisitos. Isso é configurável no código fonte:

```c
#define MAX_MEMORY_KB 2048
```

## Executando o Compilador

Após compilação bem-sucedida:

```bash
# Executar com um arquivo fonte
./compilador examples/hello_world.txt

# Windows (Dev-C++/Code::Blocks)
compilador.exe examples/hello_world.txt
```

## Testes

Execute os casos de teste fornecidos:

```bash
# Testar funcionalidade básica
./compilador examples/hello_world.txt

# Testar características da calculadora
./compilador examples/calculator.txt

# Testar laços
./compilador examples/loops.txt

# Testar operações decimais
./compilador examples/decimals.txt

# Testar características abrangentes
./compilador examples/comprehensive.txt

# Testar tratamento de erros
./compilador examples/error_test.txt
```

## Problemas Comuns e Soluções

### Problemas no Linux

1. **Versão do GCC muito nova**
   - Instalar GCC 6.1 ou usar flags de compatibilidade
   - Usar flag `-std=c90` explicitamente

2. **Biblioteca math faltando**
   - Linkar com flag `-lm`

3. **Problemas de permissão**
   - Usar `sudo` para instalação no sistema

### Problemas no Windows

1. **Erros de compilação no Dev-C++**
   - Certificar-se de que todos os arquivos fonte estão adicionados ao projeto
   - Verificar se os diretórios include estão configurados corretamente

2. **Erros de linkagem no Code::Blocks**
   - Verificar se MinGW está instalado adequadamente
   - Verificar configurações de build do projeto

3. **Erros de execução**
   - Certificar-se de que arquivos de entrada usam separadores de caminho corretos
   - Verificar permissões de arquivo

## Depuração

Para fins de depuração:

```bash
# Linux com GDB
gcc -g -std=c90 -Wall -Wextra -pedantic -Iinclude src/*.c -lm -o compilador
gdb ./compilador

# Detecção de vazamentos de memória (Linux)
valgrind --leak-check=full ./compilador examples/hello_world.txt
```

## Otimização de Performance

Para builds de release:

```bash
# Compilação otimizada
gcc -O2 -DNDEBUG -std=c90 -Wall -Wextra -pedantic -Iinclude src/*.c -lm -o compilador
```

O compilador deve executar rapidamente e usar memória mínima de acordo com os requisitos do projeto. 