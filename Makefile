CC = gcc
CFLAGS = -Wall -std=c99 -Wextra -I include
SRCDIR = src
OBJDIR = obj
BINDIR = bin

# Arquivos fonte necessários apenas para léxico e memória
SOURCES = lexer.c memory.c main.c utils.c
OBJECTS = $(SOURCES:%.c=$(OBJDIR)/%.o)
TARGET = $(BINDIR)/compiler

# Regra padrão
all: $(TARGET)

# Criar diretórios
$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# Compilar objetos
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Linkar executável
$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CC) $(OBJECTS) -o $(TARGET)

# Testar com exemplo
test: $(TARGET)
	$(TARGET) examples/hello_world.txt

# Limpar
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Instalar
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

# Desinstalar
uninstall:
	rm -f /usr/local/bin/compiler

.PHONY: all clean test install uninstall 