#include "../include/compiler.h"

/* Contexto de análise semântica */
typedef struct {
    SymbolTable* symbol_table;
    Symbol* current_function;
    int in_loop;
    int error_count;
    int warning_count;
    int main_function_found;
} SemanticContext;

/* Declarações de funções estáticas */
static SemanticContext* create_context(SymbolTable* st);
static void destroy_context(SemanticContext* ctx);
static void semantic_error(SemanticContext* ctx, Token token, const char* message);
static void semantic_warning(SemanticContext* ctx, Token token, const char* message);
static int check_type_compatibility(DataType type1, DataType type2);
static int check_type_strict_compatibility(DataType type1, DataType type2);
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
static int validate_function_name(const char* name);
static int validate_variable_name(const char* name);
static int validate_main_function(SemanticContext* ctx, ASTNode* node);
static int validate_function_parameters(SemanticContext* ctx, ASTNode* node);
static int validate_type_dimensions(SemanticContext* ctx, DataType type, TypeInfo info, Token token);

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
    ctx->warning_count = 0;
    ctx->main_function_found = 0;
    
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

/* Reportar aviso semântico */
static void semantic_warning(SemanticContext* ctx, Token token, const char* message) {
    printf("AVISO SEMÂNTICO - Linha %d, Coluna %d: %s\n", token.line, token.column, message);
    ctx->warning_count++;
}

/* Verificar compatibilidade de tipos (com conversões implícitas) */
static int check_type_compatibility(DataType type1, DataType type2) {
    /* Tipos iguais são sempre compatíveis */
    if (type1 == type2) return 1;
    
    /* Inteiro pode ser convertido para decimal */
    if (type1 == TYPE_INTEIRO && type2 == TYPE_DECIMAL) return 1;
    if (type2 == TYPE_INTEIRO && type1 == TYPE_DECIMAL) return 1;
    
    return 0;
}

/* Verificar compatibilidade estrita de tipos (sem conversões) */
static int check_type_strict_compatibility(DataType type1, DataType type2) {
    return type1 == type2;
}

/* Validar nome de função */
static int validate_function_name(const char* name) {
    if (strcmp(name, "principal") == 0) {
        return 1; /* Nome principal é válido */
    }
    
    /* Verificar se começa com __ */
    if (strlen(name) < 3 || name[0] != '_' || name[1] != '_') {
        return 0;
    }
    
    /* Verificar se o terceiro caractere é válido */
    char third = name[2];
    if (!isalnum(third)) {
        return 0;
    }
    
    /* Verificar resto do nome */
    for (int i = 3; name[i]; i++) {
        if (!isalnum(name[i])) {
            return 0;
        }
    }
    
    return 1;
}

/* Validar nome de variável */
static int validate_variable_name(const char* name) {
    if (strlen(name) < 2 || name[0] != '!') {
        return 0;
    }
    
    /* Verificar se o segundo caractere é minúsculo */
    if (!islower(name[1])) {
        return 0;
    }
    
    /* Verificar resto do nome */
    for (int i = 2; name[i]; i++) {
        if (!isalnum(name[i])) {
            return 0;
        }
    }
    
    return 1;
}

/* Validar função principal */
static int validate_main_function(SemanticContext* ctx, ASTNode* node) {
    if (strcmp(node->data.function.name, "principal") != 0) {
        return 1; /* Não é função principal */
    }
    
    ctx->main_function_found = 1;
    
    /* Verificar se não tem parâmetros */
    if (node->data.function.param_count > 0) {
        semantic_error(ctx, node->token, "Função principal não pode ter parâmetros");
        return 0;
    }
    
    return 1;
}

/* Validar parâmetros de função */
static int validate_function_parameters(SemanticContext* ctx, ASTNode* node) {
    for (int i = 0; i < node->data.function.param_count; i++) {
        /* Verificar nome do parâmetro */
        if (!validate_variable_name(node->data.function.param_names[i])) {
            semantic_error(ctx, node->token, "Nome de parâmetro inválido - deve começar com ! seguido de letra minúscula");
            return 0;
        }
        
        /* Verificar se não há duplicatas */
        for (int j = i + 1; j < node->data.function.param_count; j++) {
            if (strcmp(node->data.function.param_names[i], node->data.function.param_names[j]) == 0) {
                semantic_error(ctx, node->token, "Parâmetro duplicado");
                return 0;
            }
        }
    }
    
    return 1;
}

/* Validar dimensões de tipo */
static int validate_type_dimensions(SemanticContext* ctx, DataType type, TypeInfo info, Token token) {
    switch (type) {
        case TYPE_TEXTO:
            if (info.size <= 0) {
                semantic_error(ctx, token, "Tamanho do texto deve ser maior que zero");
                return 0;
            }
            break;
            
        case TYPE_DECIMAL:
            if (info.precision <= 0 || info.scale < 0) {
                semantic_error(ctx, token, "Dimensões do decimal inválidas");
                return 0;
            }
            break;
            
        case TYPE_INTEIRO:
            /* Inteiro não precisa de dimensões */
            break;
            
        default:
            break;
    }
    
    return 1;
}

/* Verificar operador binário */
static DataType check_binary_operator(TokenType op, DataType left, DataType right) {
    /* Operadores aritméticos */
    if (op == TOKEN_MAIS || op == TOKEN_MENOS || op == TOKEN_MULT || op == TOKEN_DIV || op == TOKEN_POT) {
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
        
        /* Para texto, só == e <> são válidos */
        if (left == TYPE_TEXTO || right == TYPE_TEXTO) {
            if (op != TOKEN_IGUAL && op != TOKEN_DIFERENTE) {
                return TYPE_VOID;
            }
            if (left != TYPE_TEXTO || right != TYPE_TEXTO) {
                return TYPE_VOID; /* Ambos devem ser texto */
            }
        } else {
            /* Para números, tipos devem ser compatíveis */
            if (!check_type_compatibility(left, right)) {
                return TYPE_VOID;
            }
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
            const char* name = node->data.literal.string_val;
            
            /* Validar nome da variável */
            if (!validate_variable_name(name)) {
                semantic_error(ctx, node->token, "Nome de variável inválido - deve começar com ! seguido de letra minúscula");
                return TYPE_VOID;
            }
            
            Symbol* symbol = symbol_table_lookup(ctx->symbol_table, name);
            if (!symbol) {
                semantic_error(ctx, node->token, "Variável não declarada");
                return TYPE_VOID;
            }
            
            /* Verificar se variável foi inicializada */
            if (!symbol->is_initialized) {
                semantic_warning(ctx, node->token, "Variável pode não ter sido inicializada");
            }
            
            return symbol->type;
        }
        
        case AST_BINARY_OP: {
            DataType left = analyze_expression(ctx, node->children[0]);
            DataType right = analyze_expression(ctx, node->children[1]);
            
            if (left == TYPE_VOID || right == TYPE_VOID) {
                return TYPE_VOID;
            }
            
            DataType result = check_binary_operator(node->data.binary_op.operator, left, right);
            if (result == TYPE_VOID) {
                semantic_error(ctx, node->token, "Operador inválido para os tipos dados");
                return TYPE_VOID;
            }
            
            /* Avisar sobre conversões implícitas */
            if (left != right && check_type_compatibility(left, right)) {
                semantic_warning(ctx, node->token, "Conversão implícita de tipos");
            }
            
            return result;
        }
        
        case AST_FUNCTION_CALL: {
            const char* func_name = node->data.literal.string_val;
            
            /* Validar nome da função */
            if (!validate_function_name(func_name)) {
                semantic_error(ctx, node->token, "Nome de função inválido - deve ser 'principal' ou começar com '__'");
                return TYPE_VOID;
            }
            
            Symbol* func = symbol_table_lookup(ctx->symbol_table, func_name);
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
                if (arg_type == TYPE_VOID) {
                    return TYPE_VOID;
                }
                
                if (!check_type_compatibility(arg_type, func->param_types[i])) {
                    semantic_error(ctx, node->token, "Tipo de argumento incompatível");
                    return TYPE_VOID;
                }
                
                /* Avisar sobre conversões implícitas */
                if (arg_type != func->param_types[i]) {
                    semantic_warning(ctx, node->token, "Conversão implícita de tipo no argumento");
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
    const char* var_name = node->token.value;
    
    /* Validar nome da variável */
    if (!validate_variable_name(var_name)) {
        semantic_error(ctx, node->token, "Nome de variável inválido - deve começar com ! seguido de letra minúscula");
        return;
    }
    
    /* Verificar se já existe no escopo atual */
    Symbol* existing = symbol_table_lookup(ctx->symbol_table, var_name);
    if (existing && existing->scope_level == ctx->symbol_table->scope_level) {
        semantic_error(ctx, node->token, "Variável já declarada neste escopo");
        return;
    }
    
    /* Validar dimensões do tipo */
    if (!validate_type_dimensions(ctx, node->data.var_decl.var_type, node->data.var_decl.type_info, node->token)) {
        return;
    }
    
    /* Verificar inicialização */
    if (node->child_count > 0) {
        DataType init_type = analyze_expression(ctx, node->children[0]);
        if (init_type == TYPE_VOID) {
            return;
        }
        
        if (!check_type_compatibility(init_type, node->data.var_decl.var_type)) {
            semantic_error(ctx, node->token, "Tipo incompatível na inicialização");
            return;
        }
        
        /* Avisar sobre conversões implícitas */
        if (init_type != node->data.var_decl.var_type) {
            semantic_warning(ctx, node->token, "Conversão implícita de tipo na inicialização");
        }
    }
}

/* Analisar comando if */
static void analyze_if_statement(SemanticContext* ctx, ASTNode* node) {
    /* Verificar condição */
    DataType cond_type = analyze_expression(ctx, node->children[0]);
    if (cond_type == TYPE_VOID) {
        return;
    }
    
    if (cond_type != TYPE_INTEIRO) {
        semantic_error(ctx, node->token, "Condição deve ser do tipo inteiro");
        return;
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
    if (node->children[0]) {
        analyze_statement(ctx, node->children[0]);
    }
    
    /* Verificar condição */
    if (node->children[1]) {
        DataType cond_type = analyze_expression(ctx, node->children[1]);
        if (cond_type != TYPE_VOID && cond_type != TYPE_INTEIRO) {
            semantic_error(ctx, node->token, "Condição deve ser do tipo inteiro");
        }
    }
    
    /* Verificar incremento */
    if (node->children[2]) {
        analyze_statement(ctx, node->children[2]);
    }
    
    /* Analisar bloco */
    if (node->children[3]) {
        analyze_block(ctx, node->children[3]);
    }
    
    ctx->in_loop--;
}

/* Analisar comando while */
static void analyze_while_statement(SemanticContext* ctx, ASTNode* node) {
    ctx->in_loop++;
    
    /* Verificar condição */
    DataType cond_type = analyze_expression(ctx, node->children[0]);
    if (cond_type == TYPE_VOID) {
        ctx->in_loop--;
        return;
    }
    
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
        if (return_type == TYPE_VOID) {
            return;
        }
        
        if (!check_type_compatibility(return_type, ctx->current_function->type)) {
            semantic_error(ctx, node->token, "Tipo de retorno incompatível");
            return;
        }
        
        /* Avisar sobre conversões implícitas */
        if (return_type != ctx->current_function->type) {
            semantic_warning(ctx, node->token, "Conversão implícita de tipo no retorno");
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
            return;
        }
        
        /* Para leia(), todos os argumentos devem ser variáveis */
        if (node->token.type == TOKEN_LEIA) {
            if (node->children[i]->type != AST_IDENTIFIER) {
                semantic_error(ctx, node->token, "Comando leia() só aceita variáveis");
                return;
            }
            
            /* Marcar variável como inicializada */
            const char* var_name = node->children[i]->data.literal.string_val;
            Symbol* var = symbol_table_lookup(ctx->symbol_table, var_name);
            if (var) {
                var->is_initialized = 1;
            }
        }
    }
}

/* Analisar atribuição */
static void analyze_assignment(SemanticContext* ctx, ASTNode* node) {
    if (node->child_count < 2) {
        semantic_error(ctx, node->token, "Atribuição malformada");
        return;
    }
    
    /* Verificar se o lado esquerdo é uma variável */
    if (node->children[0]->type != AST_IDENTIFIER) {
        semantic_error(ctx, node->token, "Lado esquerdo da atribuição deve ser uma variável");
        return;
    }
    
    const char* var_name = node->children[0]->data.literal.string_val;
    
    /* Validar nome da variável */
    if (!validate_variable_name(var_name)) {
        semantic_error(ctx, node->token, "Nome de variável inválido - deve começar com ! seguido de letra minúscula");
        return;
    }
    
    /* Verificar se variável existe */
    Symbol* var = symbol_table_lookup(ctx->symbol_table, var_name);
    if (!var) {
        semantic_error(ctx, node->token, "Variável não declarada");
        return;
    }
    
    /* Verificar tipo da expressão */
    DataType expr_type = analyze_expression(ctx, node->children[1]);
    if (expr_type == TYPE_VOID) {
        return;
    }
    
    if (!check_type_compatibility(expr_type, var->type)) {
        semantic_error(ctx, node->token, "Tipo incompatível em atribuição");
        return;
    }
    
    /* Avisar sobre conversões implícitas */
    if (expr_type != var->type) {
        semantic_warning(ctx, node->token, "Conversão implícita de tipo na atribuição");
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
            /* Para outros tipos de comando, apenas analisar recursivamente */
            for (int i = 0; i < node->child_count; i++) {
                analyze_statement(ctx, node->children[i]);
            }
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
    const char* func_name = node->data.function.name;
    
    /* Validar nome da função */
    if (!validate_function_name(func_name)) {
        semantic_error(ctx, node->token, "Nome de função inválido - deve ser 'principal' ou começar com '__'");
        return;
    }
    
    /* Buscar função existente (já deve ter sido declarada na primeira passada) */
    Symbol* func = symbol_table_lookup(ctx->symbol_table, func_name);
    if (!func) {
        semantic_error(ctx, node->token, "Função não encontrada na tabela de símbolos");
        return;
    }
    
    /* Validar função principal */
    if (!validate_main_function(ctx, node)) {
        return;
    }
    
    /* Validar parâmetros */
    if (!validate_function_parameters(ctx, node)) {
        return;
    }
    
    /* Entrar em escopo da função */
    symbol_table_enter_scope(ctx->symbol_table);
    
    /* Adicionar parâmetros ao escopo da função */
    for (int i = 0; i < node->data.function.param_count; i++) {
        Symbol* param = symbol_table_insert(ctx->symbol_table, 
                                           node->data.function.param_names[i], 
                                           node->data.function.param_types[i]);
        if (param) {
            param->is_parameter = 1;
            param->is_initialized = 1; /* Parâmetros são sempre inicializados */
            param->type_info = node->data.function.param_type_infos[i];
        }
    }
    
    /* Analisar corpo da função */
    ctx->current_function = func;
    
    if (node->child_count > 0) {
        analyze_block(ctx, node->children[0]);
    }
    
    /* Verificar se função não-void tem retorno */
    if (func->type != TYPE_VOID && strcmp(func_name, "principal") != 0) {
        /* Esta verificação seria mais precisa com análise de fluxo de controle */
        semantic_warning(ctx, node->token, "Função pode não ter retorno em todos os caminhos");
    }
    
    ctx->current_function = NULL;
    
    /* Sair do escopo da função */
    symbol_table_exit_scope(ctx->symbol_table);
}

/* Analisar programa */
static void analyze_program(SemanticContext* ctx, ASTNode* node) {
    if (!node || node->type != AST_PROGRAM) return;
    
    /* Primeira passada: declarar todas as funções */
    for (int i = 0; i < node->child_count; i++) {
        if (node->children[i]->type == AST_FUNCTION_DEF) {
            const char* func_name = node->children[i]->data.function.name;
            
            /* Validar nome da função */
            if (!validate_function_name(func_name)) {
                semantic_error(ctx, node->children[i]->token, "Nome de função inválido");
                continue;
            }
            
            /* Verificar se função já foi declarada */
            Symbol* existing = symbol_table_lookup(ctx->symbol_table, func_name);
            if (existing && existing->scope_level == ctx->symbol_table->scope_level) {
                semantic_error(ctx, node->children[i]->token, "Função já declarada");
                continue;
            }
            
            /* Adicionar função à tabela de símbolos */
            Symbol* func = symbol_table_insert(ctx->symbol_table, func_name, 
                                             node->children[i]->data.function.return_type);
            if (func) {
                func->is_function = 1;
                func->param_count = node->children[i]->data.function.param_count;
                
                /* Copiar informações dos parâmetros */
                for (int j = 0; j < func->param_count; j++) {
                    func->param_types[j] = node->children[i]->data.function.param_types[j];
                    func->param_type_infos[j] = node->children[i]->data.function.param_type_infos[j];
                    strncpy(func->param_names[j], node->children[i]->data.function.param_names[j], 
                           MAX_IDENTIFIER_LENGTH - 1);
                    func->param_names[j][MAX_IDENTIFIER_LENGTH - 1] = '\0';
                }
                
                /* Verificar se é função principal */
                if (strcmp(func_name, "principal") == 0) {
                    ctx->main_function_found = 1;
                    if (func->param_count > 0) {
                        semantic_error(ctx, node->children[i]->token, "Função principal não pode ter parâmetros");
                    }
                }
            }
        }
    }
    
    /* Segunda passada: analisar corpos das funções */
    for (int i = 0; i < node->child_count; i++) {
        if (node->children[i]->type == AST_FUNCTION_DEF) {
            analyze_function(ctx, node->children[i]);
        }
    }
    
    /* Verificar se existe função principal */
    if (!ctx->main_function_found) {
        semantic_error(ctx, node->token, "Módulo Principal Inexistente");
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
    
    /* Imprimir estatísticas */
    printf("\n=== ANÁLISE SEMÂNTICA CONCLUÍDA ===\n");
    printf("Erros encontrados: %d\n", ctx->error_count);
    printf("Avisos encontrados: %d\n", ctx->warning_count);
    printf("Função principal encontrada: %s\n", ctx->main_function_found ? "Sim" : "Não");
    
    /* Verificar erros */
    int success = (ctx->error_count == 0);
    
    /* Limpar */
    destroy_context(ctx);
    
    return success;
} 