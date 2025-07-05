# Makefile do Compilador de Linguagem Personalizada

# Configurações do compilador
CC = gcc
CFLAGS = -Wall -Wextra -std=c90 -pedantic -g
LDFLAGS = -lm

# Diretórios
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
TEST_DIR = tests

# Executável de destino
TARGET = compilador

# Arquivos fonte
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
HEADERS = $(wildcard $(INCLUDE_DIR)/*.h)

# Alvo padrão
all: $(TARGET)

# Criar diretório de build se não existir
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compilar arquivos objeto
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Linkar executável
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

# Limpar artefatos de build
clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET)

# Executar testes
test: $(TARGET)
	@echo "Executando testes do compilador..."
	@if [ -d "$(TEST_DIR)" ]; then \
		for test_file in $(TEST_DIR)/*.txt; do \
			echo "Testando $$test_file"; \
			./$(TARGET) "$$test_file" || true; \
		done; \
	else \
		echo "Diretório de testes não encontrado. Crie $(TEST_DIR) com arquivos de teste."; \
	fi

# Instalar (copiar para /usr/local/bin)
install: $(TARGET)
	sudo cp $(TARGET) /usr/local/bin/

# Desinstalar
uninstall:
	sudo rm -f /usr/local/bin/$(TARGET)

# Build de debug
debug: CFLAGS += -DDEBUG -O0
debug: $(TARGET)

# Build de release
release: CFLAGS += -O2 -DNDEBUG
release: clean $(TARGET)

# Verificação de memória com valgrind
memcheck: $(TARGET)
	@if command -v valgrind >/dev/null 2>&1; then \
		echo "Executando verificação de memória..."; \
		valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET) tests/exemplo.txt; \
	else \
		echo "Valgrind não encontrado. Instale valgrind para verificação de memória."; \
	fi

# Formatar código (requer clang-format)
format:
	@if command -v clang-format >/dev/null 2>&1; then \
		find $(SRC_DIR) $(INCLUDE_DIR) -name "*.c" -o -name "*.h" | xargs clang-format -i; \
		echo "Código formatado com sucesso."; \
	else \
		echo "clang-format não encontrado. Instale clang-format para formatação de código."; \
	fi

# Análise estática (requer cppcheck)
analyze:
	@if command -v cppcheck >/dev/null 2>&1; then \
		cppcheck --enable=all --std=c90 --platform=native $(SRC_DIR); \
	else \
		echo "cppcheck não encontrado. Instale cppcheck para análise estática."; \
	fi

# Ajuda
help:
	@echo "Alvos disponíveis:"
	@echo "  all       - Compilar o compilador (padrão)"
	@echo "  clean     - Remover artefatos de build"
	@echo "  test      - Executar casos de teste"
	@echo "  debug     - Compilar com símbolos de debug"
	@echo "  release   - Compilar versão otimizada de release"
	@echo "  install   - Instalar em /usr/local/bin"
	@echo "  uninstall - Remover de /usr/local/bin"
	@echo "  memcheck  - Executar detecção de vazamentos de memória"
	@echo "  format    - Formatar código fonte"
	@echo "  analyze   - Executar análise estática de código"
	@echo "  help      - Mostrar esta mensagem de ajuda"

# Alvos phony
.PHONY: all clean test debug release install uninstall memcheck format analyze help

# Dependências
-include $(OBJECTS:.o=.d)

# Gerar arquivos de dependência
$(BUILD_DIR)/%.d: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -MM $< > $@
	@sed -i 's|^[^:]*:|$(BUILD_DIR)/&|' $@ 