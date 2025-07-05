#include "../include/compiler.h"

/* Criar tabela de símbolos */
SymbolTable* symbol_table_create(void) {
    SymbolTable* st = (SymbolTable*)malloc(sizeof(SymbolTable));
    if (!st) {
        error_report(ERROR_MEMORY, 0, 0, "Falha ao alocar tabela de símbolos");
        return NULL;
    }
    
    /* Inicializar tabela */
    for (int i = 0; i < MAX_SYMBOL_TABLE_SIZE; i++) {
        st->table[i] = NULL;
    }
    
    st->scope_level = 0;
    st->symbol_count = 0;
    
    return st;
}

/* Destruir tabela de símbolos */
void symbol_table_destroy(SymbolTable* st) {
    if (!st) return;
    
    /* Liberar todos os símbolos */
    for (int i = 0; i < MAX_SYMBOL_TABLE_SIZE; i++) {
        Symbol* symbol = st->table[i];
        while (symbol) {
            Symbol* next = symbol->next;
            free(symbol);
            symbol = next;
        }
    }
    
    free(st);
}

/* Função de hash para strings */
unsigned int symbol_hash(const char* str) {
    unsigned int hash = 5381;
    int c;
    
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    
    return hash % MAX_SYMBOL_TABLE_SIZE;
}

/* Inserir símbolo na tabela */
Symbol* symbol_table_insert(SymbolTable* st, const char* name, DataType type) {
    if (!st || !name) return NULL;
    
    /* Verificar se já existe no escopo atual */
    Symbol* existing = symbol_table_lookup(st, name);
    if (existing && existing->scope_level == st->scope_level) {
        return NULL; /* Já declarado neste escopo */
    }
    
    /* Criar novo símbolo */
    Symbol* symbol = (Symbol*)malloc(sizeof(Symbol));
    if (!symbol) {
        error_report(ERROR_MEMORY, 0, 0, "Falha ao alocar símbolo");
        return NULL;
    }
    
    /* Inicializar símbolo */
    strncpy(symbol->name, name, MAX_IDENTIFIER_LENGTH - 1);
    symbol->name[MAX_IDENTIFIER_LENGTH - 1] = '\0';
    symbol->type = type;
    symbol->is_function = 0;
    symbol->is_parameter = 0;
    symbol->scope_level = st->scope_level;
    symbol->is_initialized = 0;
    symbol->param_count = 0;
    
    /* Zerar valores */
    memset(&symbol->value, 0, sizeof(symbol->value));
    memset(&symbol->type_info, 0, sizeof(symbol->type_info));
    
    /* Inserir na tabela */
    unsigned int index = symbol_hash(name);
    symbol->next = st->table[index];
    st->table[index] = symbol;
    st->symbol_count++;
    
    return symbol;
}

/* Buscar símbolo na tabela */
Symbol* symbol_table_lookup(SymbolTable* st, const char* name) {
    if (!st || !name) return NULL;
    
    unsigned int index = symbol_hash(name);
    Symbol* symbol = st->table[index];
    
    /* Procurar símbolo mais recente com o nome */
    Symbol* found = NULL;
    while (symbol) {
        if (strcmp(symbol->name, name) == 0) {
            if (!found || symbol->scope_level > found->scope_level) {
                found = symbol;
            }
        }
        symbol = symbol->next;
    }
    
    return found;
}

/* Entrar em novo escopo */
void symbol_table_enter_scope(SymbolTable* st) {
    if (!st) return;
    st->scope_level++;
}

/* Sair do escopo atual */
void symbol_table_exit_scope(SymbolTable* st) {
    if (!st || st->scope_level == 0) return;
    
    /* Remover símbolos do escopo atual */
    for (int i = 0; i < MAX_SYMBOL_TABLE_SIZE; i++) {
        Symbol** symbol_ptr = &st->table[i];
        while (*symbol_ptr) {
            if ((*symbol_ptr)->scope_level == st->scope_level) {
                Symbol* to_remove = *symbol_ptr;
                *symbol_ptr = to_remove->next;
                free(to_remove);
                st->symbol_count--;
            } else {
                symbol_ptr = &(*symbol_ptr)->next;
            }
        }
    }
    
    st->scope_level--;
}

/* Imprimir tabela de símbolos */
void symbol_table_print(SymbolTable* st) {
    if (!st) return;
    
    printf("\n=== TABELA DE SÍMBOLOS ===\n");
    printf("Escopo atual: %d\n", st->scope_level);
    printf("Total de símbolos: %d\n\n", st->symbol_count);
    
    for (int i = 0; i < MAX_SYMBOL_TABLE_SIZE; i++) {
        Symbol* symbol = st->table[i];
        if (symbol) {
            printf("Índice %d:\n", i);
            while (symbol) {
                printf("  %s: %s (escopo %d)\n",
                       symbol->name,
                       data_type_to_string(symbol->type),
                       symbol->scope_level);
                
                if (symbol->is_function) {
                    printf("    Função com %d parâmetros\n", symbol->param_count);
                    for (int j = 0; j < symbol->param_count; j++) {
                        printf("    Param %d: %s (%s)\n",
                               j + 1,
                               symbol->param_names[j],
                               data_type_to_string(symbol->param_types[j]));
                    }
                }
                
                symbol = symbol->next;
            }
        }
    }
    
    printf("========================\n\n");
} 