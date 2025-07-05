#include "../include/compiler.h"

/* Contexto de análise semântica */
typedef struct {
    SymbolTable* symbol_table;
    Symbol* current_function;
    int in_loop;
    int error_count;
} SemanticContext;

/* Declarações de funções estáticas */
static SemanticContext* create_context(SymbolTable* st);
static void destroy_context(SemanticContext* ctx);
static void semantic_error(SemanticContext* ctx, Token token, const char* message);
static int check_type_compatibility(DataType type1, DataType type2);
static DataType check_binary_operator(TokenType op, DataType left, DataType right);
static DataType analyze_expression(SemanticContext* ctx, ASTNode* node);
static void analyze_var_declaration(SemanticContext* ctx, ASTNode* node);
static void analyze_if_statement(SemanticContext* ctx, ASTNode* node);
static void analyze_for_statement(SemanticContext* ctx, ASTNode* node);
static void analyze_while_statement(SemanticContext* ctx, ASTNode* node);
static void analyze_return_statement(SemanticContext* ctx, ASTNode* node);
static void analyze_io_statement(SemanticContext* ctx, ASTNode* node);
static void analyze_assignment(SemanticContext* ctx, ASTNode* node);
static void analyze_statement(SemanticContext* ctx, ASTNode* node);
static void analyze_block(SemanticContext* ctx, ASTNode* node);
static void analyze_function(SemanticContext* ctx, ASTNode* node);
static void analyze_program(SemanticContext* ctx, ASTNode* node);

/* Criar contexto */
static SemanticContext* create_context(SymbolTable* st) {
    SemanticContext* ctx = (SemanticContext*)malloc(sizeof(SemanticContext));
    if (!ctx) {
        error_report(ERROR_MEMORY, 0, 0, "Falha ao alocar contexto semântico");
        return NULL;
    }
    
    ctx->symbol_table = st;
    ctx->current_function = NULL;
    ctx->in_loop = 0;
    ctx->error_count = 0;
    
    return ctx;
}

/* Destruir contexto */
static void destroy_context(SemanticContext* ctx) {
    free(ctx);
}

/* Reportar erro semântico */
static void semantic_error(SemanticContext* ctx, Token token, const char* message) {
    error_report(ERROR_SEMANTIC, token.line, token.column, message);
    ctx->error_count++;
}

/* Verificar compatibilidade de tipos */
static int check_type_compatibility(DataType type1, DataType type2) {
    /* Tipos iguais são sempre compatíveis */
    if (type1 == type2) return 1;
    
    /* Inteiro pode ser convertido para decimal */
    if (type1 == TYPE_INTEIRO && type2 == TYPE_DECIMAL) return 1;
    if (type2 == TYPE_INTEIRO && type1 == TYPE_DECIMAL) return 1;
    
    return 0;
}

/* Verificar operador binário */
static DataType check_binary_operator(TokenType op, DataType left, DataType right) {
    /* Operadores aritméticos */
    if (op == TOKEN_MAIS || op == TOKEN_MENOS || op == TOKEN_MULT || op == TOKEN_DIV) {
        if (left == TYPE_TEXTO || right == TYPE_TEXTO) {
            return TYPE_VOID; /* Erro: não pode usar operadores aritméticos com texto */
        }
        
        /* Se algum operando é decimal, resultado é decimal */
        if (left == TYPE_DECIMAL || right == TYPE_DECIMAL) {
            return TYPE_DECIMAL;
        }
        
        return TYPE_INTEIRO;
    }
    
    /* Operadores relacionais */
    if (op == TOKEN_IGUAL || op == TOKEN_DIFERENTE || 
        op == TOKEN_MENOR || op == TOKEN_MENOR_IGUAL ||
        op == TOKEN_MAIOR || op == TOKEN_MAIOR_IGUAL) {
        
        /* Tipos devem ser compatíveis */
        if (!check_type_compatibility(left, right)) {
            return TYPE_VOID; /* Erro: tipos incompatíveis */
        }
        
        return TYPE_INTEIRO; /* Resultado é sempre inteiro (0 ou 1) */
    }
    
    /* Operadores lógicos */
    if (op == TOKEN_E || op == TOKEN_OU) {
        if (left != TYPE_INTEIRO || right != TYPE_INTEIRO) {
            return TYPE_VOID; /* Erro: operandos devem ser inteiros */
        }
        
        return TYPE_INTEIRO;
    }
    
    return TYPE_VOID; /* Operador desconhecido */
}

/* Analisar expressão */
static DataType analyze_expression(SemanticContext* ctx, ASTNode* node) {
    if (!node) return TYPE_VOID;
    
    switch (node->type) {
        case AST_LITERAL:
            return node->data_type;
            
        case AST_IDENTIFIER: {
            Symbol* symbol = symbol_table_lookup(ctx->symbol_table, node->data.literal.string_val);
            if (!symbol) {
                semantic_error(ctx, node->token, "Variável não declarada");
                return TYPE_VOID;
            }
            return symbol->type;
        }
        
        case AST_BINARY_OP: {
            DataType left = analyze_expression(ctx, node->children[0]);
            DataType right = analyze_expression(ctx, node->children[1]);
            
            DataType result = check_binary_operator(node->data.binary_op.operator, left, right);
            if (result == TYPE_VOID) {
                semantic_error(ctx, node->token, "Operador inválido para os tipos dados");
            }
            return result;
        }
        
        case AST_FUNCTION_CALL: {
            Symbol* func = symbol_table_lookup(ctx->symbol_table, node->data.literal.string_val);
            if (!func) {
                semantic_error(ctx, node->token, "Função não declarada");
                return TYPE_VOID;
            }
            
            if (!func->is_function) {
                semantic_error(ctx, node->token, "Identificador não é uma função");
                return TYPE_VOID;
            }
            
            /* Verificar número de argumentos */
            if (node->child_count != func->param_count) {
                semantic_error(ctx, node->token, "Número incorreto de argumentos");
                return TYPE_VOID;
            }
            
            /* Verificar tipos dos argumentos */
            for (int i = 0; i < node->child_count; i++) {
                DataType arg_type = analyze_expression(ctx, node->children[i]);
                if (!check_type_compatibility(arg_type, func->param_types[i])) {
                    semantic_error(ctx, node->token, "Tipo de argumento incompatível");
                    return TYPE_VOID;
                }
            }
            
            return func->type;
        }
        
        default:
            return TYPE_VOID;
    }
}

/* Analisar declaração de variável */
static void analyze_var_declaration(SemanticContext* ctx, ASTNode* node) {
    /* Verificar se já existe no escopo atual */
    Symbol* existing = symbol_table_lookup(ctx->symbol_table, node->token.value);
    if (existing && existing->scope_level == ctx->symbol_table->scope_level) {
        semantic_error(ctx, node->token, "Variável já declarada neste escopo");
        return;
    }
    
    /* Verificar inicialização */
    if (node->child_count > 0) {
        DataType init_type = analyze_expression(ctx, node->children[0]);
        if (!check_type_compatibility(init_type, node->data.var_decl.var_type)) {
            semantic_error(ctx, node->token, "Tipo incompatível na inicialização");
        }
    }
}

/* Analisar comando if */
static void analyze_if_statement(SemanticContext* ctx, ASTNode* node) {
    /* Verificar condição */
    DataType cond_type = analyze_expression(ctx, node->children[0]);
    if (cond_type != TYPE_INTEIRO) {
        semantic_error(ctx, node->token, "Condição deve ser do tipo inteiro");
    }
    
    /* Analisar blocos then e else */
    analyze_block(ctx, node->children[1]);
    if (node->child_count > 2) {
        analyze_block(ctx, node->children[2]);
    }
}

/* Analisar comando for */
static void analyze_for_statement(SemanticContext* ctx, ASTNode* node) {
    ctx->in_loop++;
    
    /* Verificar inicialização */
    analyze_statement(ctx, node->children[0]);
    
    /* Verificar condição */
    DataType cond_type = analyze_expression(ctx, node->children[1]);
    if (cond_type != TYPE_INTEIRO) {
        semantic_error(ctx, node->token, "Condição deve ser do tipo inteiro");
    }
    
    /* Verificar incremento */
    analyze_statement(ctx, node->children[2]);
    
    /* Analisar bloco */
    analyze_block(ctx, node->children[3]);
    
    ctx->in_loop--;
}

/* Analisar comando while */
static void analyze_while_statement(SemanticContext* ctx, ASTNode* node) {
    ctx->in_loop++;
    
    /* Verificar condição */
    DataType cond_type = analyze_expression(ctx, node->children[0]);
    if (cond_type != TYPE_INTEIRO) {
        semantic_error(ctx, node->token, "Condição deve ser do tipo inteiro");
    }
    
    /* Analisar bloco */
    analyze_block(ctx, node->children[1]);
    
    ctx->in_loop--;
}

/* Analisar comando de retorno */
static void analyze_return_statement(SemanticContext* ctx, ASTNode* node) {
    if (!ctx->current_function) {
        semantic_error(ctx, node->token, "Comando retorno fora de função");
        return;
    }
    
    /* Verificar tipo do valor retornado */
    if (node->child_count > 0) {
        DataType return_type = analyze_expression(ctx, node->children[0]);
        if (!check_type_compatibility(return_type, ctx->current_function->type)) {
            semantic_error(ctx, node->token, "Tipo de retorno incompatível");
        }
    } else if (ctx->current_function->type != TYPE_VOID) {
        semantic_error(ctx, node->token, "Função deve retornar um valor");
    }
}

/* Analisar comando de entrada/saída */
static void analyze_io_statement(SemanticContext* ctx, ASTNode* node) {
    /* Verificar argumentos */
    for (int i = 0; i < node->child_count; i++) {
        DataType arg_type = analyze_expression(ctx, node->children[i]);
        if (arg_type == TYPE_VOID) {
            semantic_error(ctx, node->token, "Argumento inválido em comando de E/S");
        }
    }
}

/* Analisar atribuição */
static void analyze_assignment(SemanticContext* ctx, ASTNode* node) {
    /* Verificar se variável existe */
    Symbol* var = symbol_table_lookup(ctx->symbol_table, node->children[0]->data.literal.string_val);
    if (!var) {
        semantic_error(ctx, node->token, "Variável não declarada");
        return;
    }
    
    /* Verificar tipo da expressão */
    DataType expr_type = analyze_expression(ctx, node->children[1]);
    if (!check_type_compatibility(expr_type, var->type)) {
        semantic_error(ctx, node->token, "Tipo incompatível em atribuição");
    }
    
    var->is_initialized = 1;
}

/* Analisar comando */
static void analyze_statement(SemanticContext* ctx, ASTNode* node) {
    if (!node) return;
    
    switch (node->type) {
        case AST_VAR_DECL:
            analyze_var_declaration(ctx, node);
            break;
            
        case AST_IF_STMT:
            analyze_if_statement(ctx, node);
            break;
            
        case AST_FOR_STMT:
            analyze_for_statement(ctx, node);
            break;
            
        case AST_WHILE_STMT:
            analyze_while_statement(ctx, node);
            break;
            
        case AST_RETURN_STMT:
            analyze_return_statement(ctx, node);
            break;
            
        case AST_FUNCTION_CALL:
            analyze_expression(ctx, node);
            break;
            
        case AST_ASSIGNMENT:
            analyze_assignment(ctx, node);
            break;
            
        default:
            break;
    }
}

/* Analisar bloco */
static void analyze_block(SemanticContext* ctx, ASTNode* node) {
    if (!node || node->type != AST_BLOCK) return;
    
    /* Entrar em novo escopo */
    symbol_table_enter_scope(ctx->symbol_table);
    
    /* Analisar cada comando */
    for (int i = 0; i < node->child_count; i++) {
        analyze_statement(ctx, node->children[i]);
    }
    
    /* Sair do escopo */
    symbol_table_exit_scope(ctx->symbol_table);
}

/* Analisar função */
static void analyze_function(SemanticContext* ctx, ASTNode* node) {
    /* Verificar se função já existe */
    Symbol* func = symbol_table_lookup(ctx->symbol_table, node->data.function.name);
    if (func && func->scope_level == ctx->symbol_table->scope_level) {
        semantic_error(ctx, node->token, "Função já declarada");
        return;
    }
    
    /* Adicionar função à tabela de símbolos */
    func = symbol_table_insert(ctx->symbol_table, node->data.function.name, node->data.function.return_type);
    if (!func) {
        semantic_error(ctx, node->token, "Erro ao declarar função");
        return;
    }
    
    func->is_function = 1;
    func->param_count = node->data.function.param_count;
    
    /* Copiar informações dos parâmetros */
    for (int i = 0; i < node->data.function.param_count; i++) {
        func->param_types[i] = node->data.function.param_types[i];
        strncpy(func->param_names[i], node->data.function.param_names[i], MAX_IDENTIFIER_LENGTH - 1);
    }
    
    /* Analisar corpo da função */
    ctx->current_function = func;
    analyze_block(ctx, node->children[0]);
    ctx->current_function = NULL;
}

/* Analisar programa */
static void analyze_program(SemanticContext* ctx, ASTNode* node) {
    if (!node || node->type != AST_PROGRAM) return;
    
    /* Analisar cada função */
    for (int i = 0; i < node->child_count; i++) {
        if (node->children[i]->type == AST_FUNCTION_DEF) {
            analyze_function(ctx, node->children[i]);
        }
    }
    
    /* Verificar se existe função principal */
    Symbol* main_func = symbol_table_lookup(ctx->symbol_table, "principal");
    if (!main_func || !main_func->is_function) {
        semantic_error(ctx, node->token, "Função principal não encontrada");
    }
}

/* Função principal de análise semântica */
int semantic_analyze(ASTNode* ast, SymbolTable* st) {
    if (!ast || !st) return 0;
    
    /* Criar contexto */
    SemanticContext* ctx = create_context(st);
    if (!ctx) return 0;
    
    /* Analisar programa */
    analyze_program(ctx, ast);
    
    /* Verificar erros */
    int error_count = ctx->error_count;
    
    /* Limpar */
    destroy_context(ctx);
    
    return error_count == 0;
} 