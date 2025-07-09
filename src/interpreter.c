#include "../include/compiler.h"
#include <time.h>

/* Estrutura para valores de runtime */
typedef struct RuntimeValue {
    DataType type;
    union {
        int int_val;
        double decimal_val;
        char* string_val;
    } value;
    int is_initialized;
} RuntimeValue;

/* Estrutura para contexto de execução */
typedef struct ExecutionContext {
    SymbolTable* symbol_table;
    MemoryManager* memory_manager;
    RuntimeValue* variables;
    int variable_count;
    int max_variables;
    int return_flag;
    RuntimeValue return_value;
    int break_flag;
    int continue_flag;
    int error_flag;
    char error_message[MAX_ERROR_MESSAGE_LENGTH];
} ExecutionContext;

/* Protótipos das funções */
static ExecutionContext* create_execution_context(SymbolTable* st, MemoryManager* mm);
static void destroy_execution_context(ExecutionContext* ctx);
static RuntimeValue* get_variable(ExecutionContext* ctx, const char* name);
static void set_variable(ExecutionContext* ctx, const char* name, RuntimeValue value);
static RuntimeValue execute_expression(ExecutionContext* ctx, ASTNode* node);
static void execute_statement(ExecutionContext* ctx, ASTNode* node);
static void execute_block(ExecutionContext* ctx, ASTNode* node);
static void execute_function_call(ExecutionContext* ctx, ASTNode* node);
static void execute_if_statement(ExecutionContext* ctx, ASTNode* node);
static void execute_for_statement(ExecutionContext* ctx, ASTNode* node);
static void execute_while_statement(ExecutionContext* ctx, ASTNode* node);
static void execute_io_statement(ExecutionContext* ctx, ASTNode* node);
static void execute_assignment(ExecutionContext* ctx, ASTNode* node);
static RuntimeValue create_runtime_value(DataType type);
static void copy_runtime_value(RuntimeValue* dest, RuntimeValue* src);
static void free_runtime_value(RuntimeValue* value);
static int runtime_value_to_bool(RuntimeValue* value);
static void runtime_error(ExecutionContext* ctx, const char* message);

/* Criar contexto de execução */
static ExecutionContext* create_execution_context(SymbolTable* st, MemoryManager* mm) {
    ExecutionContext* ctx = (ExecutionContext*)memory_alloc(mm, sizeof(ExecutionContext));
    if (!ctx) {
        return NULL;
    }
    
    ctx->symbol_table = st;
    ctx->memory_manager = mm;
    ctx->variable_count = 0;
    ctx->max_variables = 1024;
    ctx->return_flag = 0;
    ctx->break_flag = 0;
    ctx->continue_flag = 0;
    ctx->error_flag = 0;
    ctx->error_message[0] = '\0';
    
    ctx->variables = (RuntimeValue*)memory_alloc(mm, sizeof(RuntimeValue) * ctx->max_variables);
    if (!ctx->variables) {
        memory_free(mm, ctx);
        return NULL;
    }
    
    ctx->return_value = create_runtime_value(TYPE_VOID);
    
    return ctx;
}

/* Destruir contexto de execução */
static void destroy_execution_context(ExecutionContext* ctx) {
    if (!ctx) return;
    
    /* Liberar valores das variáveis */
    for (int i = 0; i < ctx->variable_count; i++) {
        free_runtime_value(&ctx->variables[i]);
    }
    
    memory_free(ctx->memory_manager, ctx->variables);
    free_runtime_value(&ctx->return_value);
    memory_free(ctx->memory_manager, ctx);
}

/* Criar valor de runtime */
static RuntimeValue create_runtime_value(DataType type) {
    RuntimeValue value;
    value.type = type;
    value.is_initialized = 0;
    
    switch (type) {
        case TYPE_INTEIRO:
            value.value.int_val = 0;
            break;
        case TYPE_DECIMAL:
            value.value.decimal_val = 0.0;
            break;
        case TYPE_TEXTO:
            value.value.string_val = NULL;
            break;
        default:
            break;
    }
    
    return value;
}

/* Copiar valor de runtime */
static void copy_runtime_value(RuntimeValue* dest, RuntimeValue* src) {
    if (!dest || !src) return;
    
    dest->type = src->type;
    dest->is_initialized = src->is_initialized;
    
    switch (src->type) {
        case TYPE_INTEIRO:
            dest->value.int_val = src->value.int_val;
            break;
        case TYPE_DECIMAL:
            dest->value.decimal_val = src->value.decimal_val;
            break;
        case TYPE_TEXTO:
            if (src->value.string_val) {
                dest->value.string_val = strdup(src->value.string_val);
            } else {
                dest->value.string_val = NULL;
            }
            break;
        default:
            break;
    }
}

/* Liberar valor de runtime */
static void free_runtime_value(RuntimeValue* value) {
    if (!value) return;
    
    if (value->type == TYPE_TEXTO && value->value.string_val) {
        free(value->value.string_val);
        value->value.string_val = NULL;
    }
    
    value->is_initialized = 0;
}

/* Obter variável */
static RuntimeValue* get_variable(ExecutionContext* ctx, const char* name) {
    for (int i = 0; i < ctx->variable_count; i++) {
        /* Assumindo que o nome está armazenado em algum lugar */
        /* Esta implementação precisa ser ajustada para funcionar com a tabela de símbolos */
    }
    return NULL;
}

/* Definir variável */
static void set_variable(ExecutionContext* ctx, const char* name, RuntimeValue value) {
    /* Implementação para definir variável */
    /* Esta implementação precisa ser ajustada para funcionar com a tabela de símbolos */
}

/* Converter valor para booleano */
static int runtime_value_to_bool(RuntimeValue* value) {
    if (!value || !value->is_initialized) return 0;
    
    switch (value->type) {
        case TYPE_INTEIRO:
            return value->value.int_val != 0;
        case TYPE_DECIMAL:
            return value->value.decimal_val != 0.0;
        case TYPE_TEXTO:
            return value->value.string_val != NULL && strlen(value->value.string_val) > 0;
        default:
            return 0;
    }
}

/* Reportar erro de runtime */
static void runtime_error(ExecutionContext* ctx, const char* message) {
    ctx->error_flag = 1;
    strncpy(ctx->error_message, message, MAX_ERROR_MESSAGE_LENGTH - 1);
    ctx->error_message[MAX_ERROR_MESSAGE_LENGTH - 1] = '\0';
}

/* Executar expressão */
static RuntimeValue execute_expression(ExecutionContext* ctx, ASTNode* node) {
    RuntimeValue result = create_runtime_value(TYPE_VOID);
    
    if (!node || ctx->error_flag) {
        return result;
    }
    
    switch (node->type) {
        case AST_LITERAL:
            result.type = node->data_type;
            result.is_initialized = 1;
            
            switch (node->data_type) {
                case TYPE_INTEIRO:
                    result.value.int_val = node->data.literal.int_val;
                    break;
                case TYPE_DECIMAL:
                    result.value.decimal_val = node->data.literal.decimal_val;
                    break;
                case TYPE_TEXTO:
                    result.value.string_val = strdup(node->data.literal.string_val);
                    break;
                default:
                    break;
            }
            break;
            
        case AST_IDENTIFIER: {
            const char* var_name = node->data.literal.string_val;
            Symbol* symbol = symbol_table_lookup(ctx->symbol_table, var_name);
            
            if (!symbol) {
                runtime_error(ctx, "Variável não declarada");
                return result;
            }
            
            result.type = symbol->type;
            result.is_initialized = symbol->is_initialized;
            
            if (symbol->is_initialized) {
                switch (symbol->type) {
                    case TYPE_INTEIRO:
                        result.value.int_val = symbol->value.int_value;
                        break;
                    case TYPE_DECIMAL:
                        result.value.decimal_val = symbol->value.decimal_value;
                        break;
                    case TYPE_TEXTO:
                        result.value.string_val = strdup(symbol->value.string_value);
                        break;
                    default:
                        break;
                }
            }
            break;
        }
        
        case AST_BINARY_OP: {
            RuntimeValue left = execute_expression(ctx, node->children[0]);
            RuntimeValue right = execute_expression(ctx, node->children[1]);
            
            if (ctx->error_flag) {
                free_runtime_value(&left);
                free_runtime_value(&right);
                return result;
            }
            
            TokenType op = node->data.binary_op.operator;
            
            /* Operadores aritméticos */
            if (op == TOKEN_MAIS || op == TOKEN_MENOS || op == TOKEN_MULT || op == TOKEN_DIV) {
                if (left.type == TYPE_DECIMAL || right.type == TYPE_DECIMAL) {
                    result.type = TYPE_DECIMAL;
                    result.is_initialized = 1;
                    
                    double left_val = (left.type == TYPE_DECIMAL) ? left.value.decimal_val : left.value.int_val;
                    double right_val = (right.type == TYPE_DECIMAL) ? right.value.decimal_val : right.value.int_val;
                    
                    switch (op) {
                        case TOKEN_MAIS:
                            result.value.decimal_val = left_val + right_val;
                            break;
                        case TOKEN_MENOS:
                            result.value.decimal_val = left_val - right_val;
                            break;
                        case TOKEN_MULT:
                            result.value.decimal_val = left_val * right_val;
                            break;
                        case TOKEN_DIV:
                            if (right_val == 0.0) {
                                runtime_error(ctx, "Divisão por zero");
                            } else {
                                result.value.decimal_val = left_val / right_val;
                            }
                            break;
                        default:
                            break;
                    }
                } else {
                    result.type = TYPE_INTEIRO;
                    result.is_initialized = 1;
                    
                    switch (op) {
                        case TOKEN_MAIS:
                            result.value.int_val = left.value.int_val + right.value.int_val;
                            break;
                        case TOKEN_MENOS:
                            result.value.int_val = left.value.int_val - right.value.int_val;
                            break;
                        case TOKEN_MULT:
                            result.value.int_val = left.value.int_val * right.value.int_val;
                            break;
                        case TOKEN_DIV:
                            if (right.value.int_val == 0) {
                                runtime_error(ctx, "Divisão por zero");
                            } else {
                                result.value.int_val = left.value.int_val / right.value.int_val;
                            }
                            break;
                        default:
                            break;
                    }
                }
            }
            /* Operadores relacionais */
            else if (op == TOKEN_IGUAL || op == TOKEN_DIFERENTE || 
                     op == TOKEN_MENOR || op == TOKEN_MENOR_IGUAL ||
                     op == TOKEN_MAIOR || op == TOKEN_MAIOR_IGUAL) {
                
                result.type = TYPE_INTEIRO;
                result.is_initialized = 1;
                
                if (left.type == TYPE_TEXTO && right.type == TYPE_TEXTO) {
                    int cmp = strcmp(left.value.string_val, right.value.string_val);
                    switch (op) {
                        case TOKEN_IGUAL:
                            result.value.int_val = (cmp == 0) ? 1 : 0;
                            break;
                        case TOKEN_DIFERENTE:
                            result.value.int_val = (cmp != 0) ? 1 : 0;
                            break;
                        default:
                            runtime_error(ctx, "Operador não suportado para texto");
                            break;
                    }
                } else {
                    double left_val = (left.type == TYPE_DECIMAL) ? left.value.decimal_val : left.value.int_val;
                    double right_val = (right.type == TYPE_DECIMAL) ? right.value.decimal_val : right.value.int_val;
                    
                    switch (op) {
                        case TOKEN_IGUAL:
                            result.value.int_val = (left_val == right_val) ? 1 : 0;
                            break;
                        case TOKEN_DIFERENTE:
                            result.value.int_val = (left_val != right_val) ? 1 : 0;
                            break;
                        case TOKEN_MENOR:
                            result.value.int_val = (left_val < right_val) ? 1 : 0;
                            break;
                        case TOKEN_MENOR_IGUAL:
                            result.value.int_val = (left_val <= right_val) ? 1 : 0;
                            break;
                        case TOKEN_MAIOR:
                            result.value.int_val = (left_val > right_val) ? 1 : 0;
                            break;
                        case TOKEN_MAIOR_IGUAL:
                            result.value.int_val = (left_val >= right_val) ? 1 : 0;
                            break;
                        default:
                            break;
                    }
                }
            }
            /* Operadores lógicos */
            else if (op == TOKEN_E || op == TOKEN_OU) {
                result.type = TYPE_INTEIRO;
                result.is_initialized = 1;
                
                int left_bool = runtime_value_to_bool(&left);
                int right_bool = runtime_value_to_bool(&right);
                
                switch (op) {
                    case TOKEN_E:
                        result.value.int_val = (left_bool && right_bool) ? 1 : 0;
                        break;
                    case TOKEN_OU:
                        result.value.int_val = (left_bool || right_bool) ? 1 : 0;
                        break;
                    default:
                        break;
                }
            }
            
            free_runtime_value(&left);
            free_runtime_value(&right);
            break;
        }
        
        case AST_FUNCTION_CALL:
            /* Implementar chamada de função */
            runtime_error(ctx, "Chamada de função não implementada ainda");
            break;
            
        default:
            runtime_error(ctx, "Tipo de expressão não suportado");
            break;
    }
    
    return result;
}

/* Executar comando de entrada/saída */
static void execute_io_statement(ExecutionContext* ctx, ASTNode* node) {
    if (!node || ctx->error_flag) return;
    
    if (node->token.type == TOKEN_ESCREVA) {
        /* Comando escreva */
        for (int i = 0; i < node->child_count; i++) {
            RuntimeValue value = execute_expression(ctx, node->children[i]);
            
            if (ctx->error_flag) {
                free_runtime_value(&value);
                return;
            }
            
            if (value.is_initialized) {
                switch (value.type) {
                    case TYPE_INTEIRO:
                        printf("%d", value.value.int_val);
                        break;
                    case TYPE_DECIMAL:
                        printf("%.2f", value.value.decimal_val);
                        break;
                    case TYPE_TEXTO:
                        if (value.value.string_val) {
                            printf("%s", value.value.string_val);
                        }
                        break;
                    default:
                        break;
                }
            }
            
            free_runtime_value(&value);
        }
        printf("\n");
    } else if (node->token.type == TOKEN_LEIA) {
        /* Comando leia */
        for (int i = 0; i < node->child_count; i++) {
            if (node->children[i]->type == AST_IDENTIFIER) {
                const char* var_name = node->children[i]->data.literal.string_val;
                Symbol* symbol = symbol_table_lookup(ctx->symbol_table, var_name);
                
                if (!symbol) {
                    runtime_error(ctx, "Variável não declarada");
                    return;
                }
                
                printf("Digite um valor: ");
                fflush(stdout);
                
                switch (symbol->type) {
                    case TYPE_INTEIRO:
                        scanf("%d", &symbol->value.int_value);
                        break;
                    case TYPE_DECIMAL:
                        scanf("%lf", &symbol->value.decimal_value);
                        break;
                    case TYPE_TEXTO:
                        scanf("%s", symbol->value.string_value);
                        break;
                    default:
                        break;
                }
                
                symbol->is_initialized = 1;
            }
        }
    }
}

/* Executar atribuição */
static void execute_assignment(ExecutionContext* ctx, ASTNode* node) {
    if (!node || ctx->error_flag || node->child_count < 2) return;
    
    if (node->children[0]->type != AST_IDENTIFIER) {
        runtime_error(ctx, "Lado esquerdo da atribuição deve ser uma variável");
        return;
    }
    
    const char* var_name = node->children[0]->data.literal.string_val;
    Symbol* symbol = symbol_table_lookup(ctx->symbol_table, var_name);
    
    if (!symbol) {
        runtime_error(ctx, "Variável não declarada");
        return;
    }
    
    RuntimeValue value = execute_expression(ctx, node->children[1]);
    
    if (ctx->error_flag) {
        free_runtime_value(&value);
        return;
    }
    
    /* Atribuir valor */
    switch (symbol->type) {
        case TYPE_INTEIRO:
            if (value.type == TYPE_INTEIRO) {
                symbol->value.int_value = value.value.int_val;
            } else if (value.type == TYPE_DECIMAL) {
                symbol->value.int_value = (int)value.value.decimal_val;
            }
            break;
        case TYPE_DECIMAL:
            if (value.type == TYPE_DECIMAL) {
                symbol->value.decimal_value = value.value.decimal_val;
            } else if (value.type == TYPE_INTEIRO) {
                symbol->value.decimal_value = (double)value.value.int_val;
            }
            break;
        case TYPE_TEXTO:
            if (value.type == TYPE_TEXTO && value.value.string_val) {
                strncpy(symbol->value.string_value, value.value.string_val, MAX_STRING_LENGTH - 1);
                symbol->value.string_value[MAX_STRING_LENGTH - 1] = '\0';
            }
            break;
        default:
            break;
    }
    
    symbol->is_initialized = 1;
    free_runtime_value(&value);
}

/* Executar comando */
static void execute_statement(ExecutionContext* ctx, ASTNode* node) {
    if (!node || ctx->error_flag) return;
    
    switch (node->type) {
        case AST_ASSIGNMENT:
            execute_assignment(ctx, node);
            break;
            
        case AST_FUNCTION_CALL:
            execute_io_statement(ctx, node);
            break;
            
        default:
            /* Para outros tipos de comando */
            for (int i = 0; i < node->child_count; i++) {
                execute_statement(ctx, node->children[i]);
                if (ctx->error_flag) break;
            }
            break;
    }
}

/* Executar bloco */
static void execute_block(ExecutionContext* ctx, ASTNode* node) {
    if (!node || ctx->error_flag) return;
    
    for (int i = 0; i < node->child_count; i++) {
        execute_statement(ctx, node->children[i]);
        if (ctx->error_flag || ctx->return_flag) break;
    }
}

/* Criar interpretador */
Interpreter* interpreter_create(ASTNode* ast, SymbolTable* st) {
    Interpreter* interpreter = (Interpreter*)malloc(sizeof(Interpreter));
    if (!interpreter) {
        return NULL;
    }
    
    interpreter->ast = ast;
    interpreter->symbol_table = st;
    interpreter->memory_manager = memory_manager_create();
    interpreter->running = 0;
    interpreter->return_flag = 0;
    
    return interpreter;
}

/* Destruir interpretador */
void interpreter_destroy(Interpreter* interpreter) {
    if (!interpreter) return;
    
    if (interpreter->memory_manager) {
        memory_manager_destroy(interpreter->memory_manager);
    }
    
    free(interpreter);
}

/* Executar interpretador */
int interpreter_execute(Interpreter* interpreter) {
    if (!interpreter || !interpreter->ast) return 0;
    
    ExecutionContext* ctx = create_execution_context(interpreter->symbol_table, interpreter->memory_manager);
    if (!ctx) {
        return 0;
    }
    
    interpreter->running = 1;
    
    printf("=== INICIANDO EXECUÇÃO ===\n");
    
    /* Procurar função principal */
    Symbol* main_func = symbol_table_lookup(interpreter->symbol_table, "principal");
    if (!main_func || !main_func->is_function) {
        runtime_error(ctx, "Função principal não encontrada");
        destroy_execution_context(ctx);
        return 0;
    }
    
    /* Executar função principal */
    /* Esta implementação é simplificada - uma implementação completa 
       precisaria de mais lógica para executar funções propriamente */
    
    for (int i = 0; i < interpreter->ast->child_count; i++) {
        ASTNode* node = interpreter->ast->children[i];
        if (node->type == AST_FUNCTION_DEF && 
            strcmp(node->data.function.name, "principal") == 0) {
            
            if (node->child_count > 0) {
                execute_block(ctx, node->children[0]);
            }
            break;
        }
    }
    
    int success = !ctx->error_flag;
    
    if (ctx->error_flag) {
        printf("ERRO DE EXECUÇÃO: %s\n", ctx->error_message);
    } else {
        printf("=== EXECUÇÃO CONCLUÍDA COM SUCESSO ===\n");
    }
    
    destroy_execution_context(ctx);
    interpreter->running = 0;
    
    return success;
} 