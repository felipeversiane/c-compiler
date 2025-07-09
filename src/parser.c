#include "../include/compiler.h"

/* Declarações de funções estáticas */
static int match_token(Parser* parser, TokenType expected);
static int consume_token(Parser* parser, TokenType expected);
static int expect_token(Parser* parser, TokenType expected);
static ASTNode* create_node(Parser* parser, ASTNodeType type);
static ASTNode* parse_program(Parser* parser);
static ASTNode* parse_main_function(Parser* parser);
static ASTNode* parse_function(Parser* parser);
static ASTNode* parse_block(Parser* parser);
static ASTNode* parse_statement(Parser* parser);
static ASTNode* parse_var_declaration(Parser* parser);
static ASTNode* parse_expression(Parser* parser);
static ASTNode* parse_if_statement(Parser* parser);
static ASTNode* parse_assignment_without_semicolon(Parser* parser);
static ASTNode* parse_for_statement(Parser* parser);
static ASTNode* parse_while_statement(Parser* parser);
static ASTNode* parse_return_statement(Parser* parser);
static ASTNode* parse_io_statement(Parser* parser);
static ASTNode* parse_assignment(Parser* parser);
static ASTNode* parse_function_call(Parser* parser);
static int parse_type_dimensions(Parser* parser, TypeInfo* type_info);

/* Criar parser */
Parser* parser_create(Lexer* lexer) {
    Parser* parser = (Parser*)malloc(sizeof(Parser));
    if (!parser) {
        error_report(ERROR_MEMORY, 0, 0, "Falha ao alocar parser");
        return NULL;
    }
    
    parser->lexer = lexer;
    parser->symbol_table = symbol_table_create();
    parser->ast = NULL;
    parser->error_count = 0;
    parser->current_scope = 0;
    
    return parser;
}

/* Destruir parser */
void parser_destroy(Parser* parser) {
    if (!parser) return;
    
    if (parser->symbol_table) {
        symbol_table_destroy(parser->symbol_table);
    }
    
    if (parser->ast) {
        ast_destroy(parser->ast);
    }
    
    free(parser);
}

/* Reportar erro sintático */
void parser_error(Parser* parser, const char* message) {
    Token token = parser->lexer->current_token;
    error_report(ERROR_SYNTAX, token.line, token.column, message);
    parser->error_count++;
}

/* Verificar se token atual é do tipo esperado */
static int match_token(Parser* parser, TokenType expected) {
    return parser->lexer->current_token.type == expected;
}

/* Consumir token atual se for do tipo esperado */
static int consume_token(Parser* parser, TokenType expected) {
    if (match_token(parser, expected)) {
        parser->lexer->current_token = lexer_next_token(parser->lexer);
        return 1;
    }
    return 0;
}

/* Exigir token do tipo esperado */
static int expect_token(Parser* parser, TokenType expected) {
    if (!consume_token(parser, expected)) {
        char error[MAX_ERROR_MESSAGE_LENGTH];
        snprintf(error, sizeof(error), 
                "Esperado token '%s', encontrado '%s'",
                token_type_to_string(expected),
                token_type_to_string(parser->lexer->current_token.type));
        parser_error(parser, error);
        return 0;
    }
    return 1;
}

/* Criar nó da AST */
static ASTNode* create_node(Parser* parser, ASTNodeType type) {
    ASTNode* node = ast_create_node(type);
    if (!node) {
        parser_error(parser, "Falha ao criar nó da AST");
        return NULL;
    }
    node->token = parser->lexer->current_token;
    return node;
}

/* Analisar programa */
static ASTNode* parse_program(Parser* parser) {
    ASTNode* program = create_node(parser, AST_PROGRAM);
    if (!program) return NULL;
    
    /* Avançar para primeiro token */
    parser->lexer->current_token = lexer_next_token(parser->lexer);
    
    /* Ler todas as funções do arquivo */
    while (!match_token(parser, TOKEN_EOF)) {
        ASTNode* func = NULL;
        
        /* Verificar tipo de função */
        if (match_token(parser, TOKEN_FUNCAO)) {
            /* Função normal */
            func = parse_function(parser);
        } else if (match_token(parser, TOKEN_PRINCIPAL)) {
            /* Função principal */
            func = parse_main_function(parser);
        } else {
            parser_error(parser, "Esperado declaração de função");
            ast_destroy(program);
            return NULL;
        }
        
        if (!func) {
            ast_destroy(program);
            return NULL;
        }
        
        ast_add_child(program, func);
    }
    
    return program;
}

/* Analisar função principal */
static ASTNode* parse_main_function(Parser* parser) {
    ASTNode* func = create_node(parser, AST_FUNCTION_DEF);
    if (!func) return NULL;
    
    /* Consumir 'principal' */
    consume_token(parser, TOKEN_PRINCIPAL);
    
    /* Copiar nome da função */
    strncpy(func->data.function.name, "principal", MAX_IDENTIFIER_LENGTH - 1);
    func->data.function.name[MAX_IDENTIFIER_LENGTH - 1] = '\0';
    func->data.function.return_type = TYPE_INTEIRO; /* principal sempre retorna inteiro */
    
    /* Parâmetros */
    if (!expect_token(parser, TOKEN_ABRE_PAREN)) {
        ast_destroy(func);
        return NULL;
    }
    
    if (!expect_token(parser, TOKEN_FECHA_PAREN)) {
        ast_destroy(func);
        return NULL;
    }
    
    /* Corpo da função */
    if (!expect_token(parser, TOKEN_ABRE_CHAVE)) {
        ast_destroy(func);
        return NULL;
    }
    
    /* Analisar bloco de código */
    ASTNode* body = parse_block(parser);
    if (!body) {
        ast_destroy(func);
        return NULL;
    }
    
    ast_add_child(func, body);
    
    if (!expect_token(parser, TOKEN_FECHA_CHAVE)) {
        ast_destroy(func);
        return NULL;
    }
    
    return func;
}

/* Analisar função */
static ASTNode* parse_function(Parser* parser) {
    ASTNode* func = create_node(parser, AST_FUNCTION_DEF);
    if (!func) return NULL;
    
    /* Consumir 'funcao' */
    consume_token(parser, TOKEN_FUNCAO);
    
    /* Nome da função */
    if (!expect_token(parser, TOKEN_FUNCAO_ID)) {
        ast_destroy(func);
        return NULL;
    }
    
    /* Copiar nome da função */
    strncpy(func->data.function.name, parser->lexer->current_token.value, MAX_IDENTIFIER_LENGTH - 1);
    func->data.function.name[MAX_IDENTIFIER_LENGTH - 1] = '\0';
    
    /* Parâmetros */
    if (!expect_token(parser, TOKEN_ABRE_PAREN)) {
        ast_destroy(func);
        return NULL;
    }
    
    /* Lista de parâmetros */
    int param_count = 0;
    if (!match_token(parser, TOKEN_FECHA_PAREN)) {
        do {
            /* Tipo do parâmetro */
            Token type_token = parser->lexer->current_token;
            DataType param_type;
            TypeInfo type_info = {0};
            
            switch (type_token.type) {
                case TOKEN_INTEIRO:
                    param_type = TYPE_INTEIRO;
                    break;
                case TOKEN_TEXTO:
                    param_type = TYPE_TEXTO;
                    break;
                case TOKEN_DECIMAL:
                    param_type = TYPE_DECIMAL;
                    break;
                default:
                    parser_error(parser, "Tipo de parâmetro inválido");
                    ast_destroy(func);
                    return NULL;
            }
            
            consume_token(parser, type_token.type);
            
            /* Nome do parâmetro */
            if (!expect_token(parser, TOKEN_VARIAVEL)) {
                ast_destroy(func);
                return NULL;
            }
            
            /* Dimensões do tipo (se houver) */
            if (!parse_type_dimensions(parser, &type_info)) {
                ast_destroy(func);
                return NULL;
            }
            
            /* Adicionar parâmetro */
            if (param_count < MAX_FUNCTION_PARAMS) {
                func->data.function.param_types[param_count] = param_type;
                func->data.function.param_type_infos[param_count] = type_info;
                strncpy(func->data.function.param_names[param_count],
                        parser->lexer->current_token.value,
                        MAX_IDENTIFIER_LENGTH - 1);
                param_count++;
            } else {
                parser_error(parser, "Número máximo de parâmetros excedido");
                ast_destroy(func);
                return NULL;
            }
            
            /* Próximo parâmetro */
            if (match_token(parser, TOKEN_VIRGULA)) {
                consume_token(parser, TOKEN_VIRGULA);
            } else {
                break;
            }
        } while (1);
    }
    
    func->data.function.param_count = param_count;
    
    if (!expect_token(parser, TOKEN_FECHA_PAREN)) {
        ast_destroy(func);
        return NULL;
    }
    
    /* Corpo da função */
    if (!expect_token(parser, TOKEN_ABRE_CHAVE)) {
        ast_destroy(func);
        return NULL;
    }
    
    /* Analisar bloco de código */
    ASTNode* body = parse_block(parser);
    if (!body) {
        ast_destroy(func);
        return NULL;
    }
    
    ast_add_child(func, body);
    
    if (!expect_token(parser, TOKEN_FECHA_CHAVE)) {
        ast_destroy(func);
        return NULL;
    }
    
    return func;
}

/* Analisar bloco de código */
static ASTNode* parse_block(Parser* parser) {
    ASTNode* block = create_node(parser, AST_BLOCK);
    if (!block) return NULL;
    
    /* Entrar em novo escopo */
    parser->current_scope++;
    symbol_table_enter_scope(parser->symbol_table);
    
    /* Analisar declarações e comandos */
    while (!match_token(parser, TOKEN_FECHA_CHAVE) && 
           !match_token(parser, TOKEN_EOF)) {
        
        ASTNode* stmt = parse_statement(parser);
        if (!stmt) {
            ast_destroy(block);
            return NULL;
        }
        
        ast_add_child(block, stmt);
    }
    
    /* Sair do escopo */
    symbol_table_exit_scope(parser->symbol_table);
    parser->current_scope--;
    
    return block;
}

/* Analisar declaração ou comando */
static ASTNode* parse_statement(Parser* parser) {
    Token token = parser->lexer->current_token;
    
    switch (token.type) {
        case TOKEN_INTEIRO:
        case TOKEN_TEXTO:
        case TOKEN_DECIMAL:
            return parse_var_declaration(parser);
            
        case TOKEN_SE:
            return parse_if_statement(parser);
            
        case TOKEN_PARA:
            return parse_for_statement(parser);
            
        case TOKEN_ENQUANTO:
            return parse_while_statement(parser);
            
        case TOKEN_RETORNO:
            return parse_return_statement(parser);
            
        case TOKEN_LEIA:
        case TOKEN_ESCREVA:
            return parse_io_statement(parser);
            
        case TOKEN_VARIAVEL:
            return parse_assignment(parser);
            
        case TOKEN_FUNCAO_ID:
            return parse_function_call(parser);
            
        default:
            parser_error(parser, "Comando inválido");
            return NULL;
    }
}

/* Analisar declaração de variável */
static ASTNode* parse_var_declaration(Parser* parser) {
    ASTNode* var_decl = create_node(parser, AST_VAR_DECL);
    if (!var_decl) return NULL;
    
    /* Tipo da variável */
    Token type_token = parser->lexer->current_token;
    DataType var_type;
    switch (type_token.type) {
        case TOKEN_INTEIRO:
            var_type = TYPE_INTEIRO;
            break;
        case TOKEN_TEXTO:
            var_type = TYPE_TEXTO;
            break;
        case TOKEN_DECIMAL:
            var_type = TYPE_DECIMAL;
            break;
        default:
            parser_error(parser, "Tipo de variável inválido");
            ast_destroy(var_decl);
            return NULL;
    }
    
    var_decl->data.var_decl.var_type = var_type;
    consume_token(parser, type_token.type);
    
    /* Nome da variável */
    if (!match_token(parser, TOKEN_VARIAVEL)) {
        parser_error(parser, "Esperado nome de variável");
        ast_destroy(var_decl);
        return NULL;
    }
    
    /* SALVAR O NOME DA VARIÁVEL NO TOKEN DO NÓ AST */
    var_decl->token = parser->lexer->current_token; /* Salvar o token da variável */
    
    /* SALVAR O NOME DA VARIÁVEL ANTES DE CONSUMIR O TOKEN */
    char var_name[MAX_IDENTIFIER_LENGTH];
    strncpy(var_name, parser->lexer->current_token.value, MAX_IDENTIFIER_LENGTH - 1);
    var_name[MAX_IDENTIFIER_LENGTH - 1] = '\0';
    
    /* Agora consumir o token da variável */
    consume_token(parser, TOKEN_VARIAVEL);
    
    /* Criar nó identificador para o nome da variável */
    ASTNode* var_identifier = create_node(parser, AST_IDENTIFIER);
    if (!var_identifier) {
        ast_destroy(var_decl);
        return NULL;
    }
    strncpy(var_identifier->data.literal.string_val, var_name, MAX_STRING_LENGTH - 1);
    var_identifier->data.literal.string_val[MAX_STRING_LENGTH - 1] = '\0';
    
    /* Adicionar identificador como primeiro filho */
    ast_add_child(var_decl, var_identifier);
    
    /* Verificar se tem dimensões */
    if (match_token(parser, TOKEN_ABRE_COLCH)) {
        consume_token(parser, TOKEN_ABRE_COLCH);
        
        /* Ler dimensão - aceitar tanto inteiro quanto decimal */
        Token dim_token = parser->lexer->current_token;
        
        if (dim_token.type == TOKEN_NUMERO_INT) {
            var_decl->data.var_decl.type_info.size = string_to_int(dim_token.value);
            consume_token(parser, TOKEN_NUMERO_INT);
        } else if (dim_token.type == TOKEN_NUMERO_DEC) {
            /* Para decimal, interpretar número decimal como precision.scale */
            if (var_type == TYPE_DECIMAL) {
                /* Parse decimal[3.2] onde 3 é precision e 2 é scale */
                char* dot_pos = strchr(dim_token.value, '.');
                if (dot_pos) {
                    /* Separar precision e scale */
                    *dot_pos = '\0'; /* Temporariamente separar */
                    char* precision_str = dim_token.value;
                    char* scale_str = dot_pos + 1;
                    
                    var_decl->data.var_decl.type_info.precision = string_to_int(precision_str);
                    var_decl->data.var_decl.type_info.scale = string_to_int(scale_str);
                    
                    *dot_pos = '.'; /* Restaurar o ponto */
                } else {
                    /* Se não tem ponto, usar como precision e scale=0 */
                    var_decl->data.var_decl.type_info.precision = string_to_int(dim_token.value);
                    var_decl->data.var_decl.type_info.scale = 0;
                }
            } else {
                /* Para outros tipos, usar apenas a parte inteira como size */
                double decimal_val = string_to_double(dim_token.value);
                var_decl->data.var_decl.type_info.size = (int)decimal_val;
            }
            consume_token(parser, TOKEN_NUMERO_DEC);
        } else {
            parser_error(parser, "Dimensão deve ser um número");
            ast_destroy(var_decl);
            return NULL;
        }
        
        /* Para decimal, pode ter formato adicional: decimal[5.2] onde 5 é size e 2 é scale */
        if (var_type == TYPE_DECIMAL && match_token(parser, TOKEN_PONTO)) {
            consume_token(parser, TOKEN_PONTO);
            
            Token scale_token = parser->lexer->current_token;
            if (scale_token.type != TOKEN_NUMERO_INT) {
                parser_error(parser, "Precisão decimal deve ser um número inteiro");
                ast_destroy(var_decl);
                return NULL;
            }
            
            var_decl->data.var_decl.type_info.scale = string_to_int(scale_token.value);
            consume_token(parser, TOKEN_NUMERO_INT);
        }
        
        if (!expect_token(parser, TOKEN_FECHA_COLCH)) {
            ast_destroy(var_decl);
            return NULL;
        }
    }
    
    /* Adicionar à tabela de símbolos USANDO O NOME SALVO */
    Symbol* symbol = symbol_table_insert(
        parser->symbol_table,
        var_name,
        var_decl->data.var_decl.var_type
    );
    
    if (!symbol) {
        parser_error(parser, "Variável já declarada neste escopo");
        ast_destroy(var_decl);
        return NULL;
    }
    
    /* Copiar informações de tipo para o símbolo */
    symbol->type_info = var_decl->data.var_decl.type_info;
    
    /* Verificar atribuição inicial */
    if (match_token(parser, TOKEN_ATRIB)) {
        consume_token(parser, TOKEN_ATRIB);
        
        ASTNode* init_expr = parse_expression(parser);
        if (!init_expr) {
            ast_destroy(var_decl);
            return NULL;
        }
        
        ast_add_child(var_decl, init_expr);
        symbol->is_initialized = 1;
    }
    
    if (!expect_token(parser, TOKEN_PONTO_VIRG)) {
        ast_destroy(var_decl);
        return NULL;
    }
    
    return var_decl;
}

/* Analisar expressão */
static ASTNode* parse_expression(Parser* parser) {
    ASTNode* left = NULL;
    Token token = parser->lexer->current_token;
    
    switch (token.type) {
        case TOKEN_NUMERO_INT:
            left = create_node(parser, AST_LITERAL);
            if (!left) return NULL;
            left->data.literal.int_val = string_to_int(token.value);
            left->data_type = TYPE_INTEIRO;
            consume_token(parser, TOKEN_NUMERO_INT);
            break;
            
        case TOKEN_NUMERO_DEC:
            left = create_node(parser, AST_LITERAL);
            if (!left) return NULL;
            left->data.literal.decimal_val = string_to_double(token.value);
            left->data_type = TYPE_DECIMAL;
            consume_token(parser, TOKEN_NUMERO_DEC);
            break;
            
        case TOKEN_STRING:
            left = create_node(parser, AST_LITERAL);
            if (!left) return NULL;
            strncpy(left->data.literal.string_val, token.value, MAX_STRING_LENGTH - 1);
            left->data.literal.string_val[MAX_STRING_LENGTH - 1] = '\0';
            left->data_type = TYPE_TEXTO;
            consume_token(parser, TOKEN_STRING);
            break;
            
        case TOKEN_VARIAVEL:
            /* Referência a variável */
            left = create_node(parser, AST_IDENTIFIER);
            if (!left) return NULL;
            strncpy(left->data.literal.string_val, token.value, MAX_STRING_LENGTH - 1);
            consume_token(parser, TOKEN_VARIAVEL);
            break;
            
        default:
            parser_error(parser, "Expressão inválida");
            return NULL;
    }
    
    /* Verificar se tem operador binário */
    token = parser->lexer->current_token;
    if (token.type == TOKEN_MAIS || token.type == TOKEN_MENOS ||
        token.type == TOKEN_MULT || token.type == TOKEN_DIV ||
        token.type == TOKEN_IGUAL || token.type == TOKEN_DIFERENTE ||
        token.type == TOKEN_MENOR || token.type == TOKEN_MENOR_IGUAL ||
        token.type == TOKEN_MAIOR || token.type == TOKEN_MAIOR_IGUAL) {
        
        /* Criar nó para operador binário */
        ASTNode* op = create_node(parser, AST_BINARY_OP);
        if (!op) {
            ast_destroy(left);
            return NULL;
        }
        
        op->data.binary_op.operator = token.type;
        consume_token(parser, token.type);
        
        /* Analisar lado direito da expressão */
        ASTNode* right = parse_expression(parser);
        if (!right) {
            ast_destroy(left);
            ast_destroy(op);
            return NULL;
        }
        
        /* Montar árvore */
        ast_add_child(op, left);
        ast_add_child(op, right);
        return op;
    }
    
    return left;
}

/* Analisar comando if */
static ASTNode* parse_if_statement(Parser* parser) {
    ASTNode* if_stmt = create_node(parser, AST_IF_STMT);
    if (!if_stmt) return NULL;
    
    /* Consumir 'se' */
    consume_token(parser, TOKEN_SE);
    
    /* Condição */
    if (!expect_token(parser, TOKEN_ABRE_PAREN)) {
        ast_destroy(if_stmt);
        return NULL;
    }
    
    ASTNode* condition = parse_expression(parser);
    if (!condition) {
        ast_destroy(if_stmt);
        return NULL;
    }
    
    ast_add_child(if_stmt, condition);
    
    if (!expect_token(parser, TOKEN_FECHA_PAREN)) {
        ast_destroy(if_stmt);
        return NULL;
    }
    
    /* Consumir '{' antes de chamar parse_block */
    if (!expect_token(parser, TOKEN_ABRE_CHAVE)) {
        ast_destroy(if_stmt);
        return NULL;
    }
    
    /* Bloco then */
    ASTNode* then_block = parse_block(parser);
    if (!then_block) {
        ast_destroy(if_stmt);
        return NULL;
    }
    
    ast_add_child(if_stmt, then_block);
    
    /* Consumir '}' após o bloco */
    if (!expect_token(parser, TOKEN_FECHA_CHAVE)) {
        ast_destroy(if_stmt);
        return NULL;
    }
    
    /* Bloco else (opcional) */
    if (match_token(parser, TOKEN_SENAO)) {
        consume_token(parser, TOKEN_SENAO);
        
        /* Consumir '{' antes de chamar parse_block para o else */
        if (!expect_token(parser, TOKEN_ABRE_CHAVE)) {
            ast_destroy(if_stmt);
            return NULL;
        }
        
        ASTNode* else_block = parse_block(parser);
        if (!else_block) {
            ast_destroy(if_stmt);
            return NULL;
        }
        
        ast_add_child(if_stmt, else_block);
        
        /* Consumir '}' após o bloco else */
        if (!expect_token(parser, TOKEN_FECHA_CHAVE)) {
            ast_destroy(if_stmt);
            return NULL;
        }
    }
    
    return if_stmt;
}

/* Analisar atribuição sem ponto e vírgula (para uso em for loops) */
static ASTNode* parse_assignment_without_semicolon(Parser* parser) {
    ASTNode* assign = create_node(parser, AST_ASSIGNMENT);
    if (!assign) return NULL;
    
    /* Variável */
    if (!match_token(parser, TOKEN_VARIAVEL)) {
        parser_error(parser, "Esperado nome de variável");
        ast_destroy(assign);
        return NULL;
    }
    
    /* Criar nó para variável */
    ASTNode* var = create_node(parser, AST_IDENTIFIER);
    if (!var) {
        ast_destroy(assign);
        return NULL;
    }
    
    strncpy(var->data.literal.string_val, parser->lexer->current_token.value, MAX_STRING_LENGTH - 1);
    var->data.literal.string_val[MAX_STRING_LENGTH - 1] = '\0';
    consume_token(parser, TOKEN_VARIAVEL);
    
    ast_add_child(assign, var);
    
    /* Operador de atribuição */
    if (!expect_token(parser, TOKEN_ATRIB)) {
        ast_destroy(assign);
        return NULL;
    }
    
    /* Expressão */
    ASTNode* expr = parse_expression(parser);
    if (!expr) {
        ast_destroy(assign);
        return NULL;
    }
    
    ast_add_child(assign, expr);
    
    return assign;
}

/* Analisar comando for */
static ASTNode* parse_for_statement(Parser* parser) {
    ASTNode* for_stmt = create_node(parser, AST_FOR_STMT);
    if (!for_stmt) return NULL;
    
    /* Consumir 'para' */
    consume_token(parser, TOKEN_PARA);
    
    /* Inicialização, condição e incremento */
    if (!expect_token(parser, TOKEN_ABRE_PAREN)) {
        ast_destroy(for_stmt);
        return NULL;
    }
    
    /* Inicialização */
    ASTNode* init = parse_assignment_without_semicolon(parser);
    if (!init) {
        ast_destroy(for_stmt);
        return NULL;
    }
    
    ast_add_child(for_stmt, init);
    
    if (!expect_token(parser, TOKEN_PONTO_VIRG)) {
        ast_destroy(for_stmt);
        return NULL;
    }
    
    /* Condição */
    ASTNode* condition = parse_expression(parser);
    if (!condition) {
        ast_destroy(for_stmt);
        return NULL;
    }
    
    ast_add_child(for_stmt, condition);
    
    if (!expect_token(parser, TOKEN_PONTO_VIRG)) {
        ast_destroy(for_stmt);
        return NULL;
    }
    
    /* Incremento */
    ASTNode* increment = parse_assignment_without_semicolon(parser);
    if (!increment) {
        ast_destroy(for_stmt);
        return NULL;
    }
    
    ast_add_child(for_stmt, increment);
    
    if (!expect_token(parser, TOKEN_FECHA_PAREN)) {
        ast_destroy(for_stmt);
        return NULL;
    }
    
    /* Consumir '{' antes de chamar parse_block */
    if (!expect_token(parser, TOKEN_ABRE_CHAVE)) {
        ast_destroy(for_stmt);
        return NULL;
    }
    
    /* Bloco do loop */
    ASTNode* body = parse_block(parser);
    if (!body) {
        ast_destroy(for_stmt);
        return NULL;
    }
    
    ast_add_child(for_stmt, body);
    
    /* Consumir '}' após o bloco */
    if (!expect_token(parser, TOKEN_FECHA_CHAVE)) {
        ast_destroy(for_stmt);
        return NULL;
    }
    
    return for_stmt;
}

/* Analisar comando while */
static ASTNode* parse_while_statement(Parser* parser) {
    ASTNode* while_stmt = create_node(parser, AST_WHILE_STMT);
    if (!while_stmt) return NULL;
    
    /* Consumir 'enquanto' */
    consume_token(parser, TOKEN_ENQUANTO);
    
    /* Condição */
    if (!expect_token(parser, TOKEN_ABRE_PAREN)) {
        ast_destroy(while_stmt);
        return NULL;
    }
    
    ASTNode* condition = parse_expression(parser);
    if (!condition) {
        ast_destroy(while_stmt);
        return NULL;
    }
    
    ast_add_child(while_stmt, condition);
    
    if (!expect_token(parser, TOKEN_FECHA_PAREN)) {
        ast_destroy(while_stmt);
        return NULL;
    }
    
    /* Bloco do loop */
    if (!expect_token(parser, TOKEN_ABRE_CHAVE)) {
        ast_destroy(while_stmt);
        return NULL;
    }
    
    ASTNode* body = parse_block(parser);
    if (!body) {
        ast_destroy(while_stmt);
        return NULL;
    }
    
    ast_add_child(while_stmt, body);
    
    if (!expect_token(parser, TOKEN_FECHA_CHAVE)) {
        ast_destroy(while_stmt);
        return NULL;
    }
    
    return while_stmt;
}

/* Analisar comando de retorno */
static ASTNode* parse_return_statement(Parser* parser) {
    ASTNode* return_stmt = create_node(parser, AST_RETURN_STMT);
    if (!return_stmt) return NULL;
    
    /* Consumir 'retorno' */
    consume_token(parser, TOKEN_RETORNO);
    
    /* Expressão de retorno */
    ASTNode* expr = parse_expression(parser);
    if (!expr) {
        ast_destroy(return_stmt);
        return NULL;
    }
    
    ast_add_child(return_stmt, expr);
    
    if (!expect_token(parser, TOKEN_PONTO_VIRG)) {
        ast_destroy(return_stmt);
        return NULL;
    }
    
    return return_stmt;
}

/* Analisar comando de entrada/saída */
static ASTNode* parse_io_statement(Parser* parser) {
    ASTNode* io_stmt = create_node(parser, AST_FUNCTION_CALL);
    if (!io_stmt) return NULL;
    
    /* Tipo de operação */
    TokenType op_type = parser->lexer->current_token.type;
    consume_token(parser, op_type);
    
    /* Parâmetros */
    if (!expect_token(parser, TOKEN_ABRE_PAREN)) {
        ast_destroy(io_stmt);
        return NULL;
    }
    
    /* Lista de expressões */
    do {
        ASTNode* expr = parse_expression(parser);
        if (!expr) {
            ast_destroy(io_stmt);
            return NULL;
        }
        
        ast_add_child(io_stmt, expr);
        
        if (match_token(parser, TOKEN_VIRGULA)) {
            consume_token(parser, TOKEN_VIRGULA);
        } else {
            break;
        }
    } while (1);
    
    if (!expect_token(parser, TOKEN_FECHA_PAREN)) {
        ast_destroy(io_stmt);
        return NULL;
    }
    
    if (!expect_token(parser, TOKEN_PONTO_VIRG)) {
        ast_destroy(io_stmt);
        return NULL;
    }
    
    return io_stmt;
}

/* Analisar atribuição */
static ASTNode* parse_assignment(Parser* parser) {
    ASTNode* assign = create_node(parser, AST_ASSIGNMENT);
    if (!assign) return NULL;
    
    /* Variável */
    if (!expect_token(parser, TOKEN_VARIAVEL)) {
        ast_destroy(assign);
        return NULL;
    }
    
    /* Criar nó para variável */
    ASTNode* var = create_node(parser, AST_IDENTIFIER);
    if (!var) {
        ast_destroy(assign);
        return NULL;
    }
    
    strncpy(var->data.literal.string_val, parser->lexer->current_token.value, MAX_STRING_LENGTH - 1);
    
    /* Operador de atribuição */
    if (!expect_token(parser, TOKEN_ATRIB)) {
        ast_destroy(assign);
        ast_destroy(var);
        return NULL;
    }
    
    /* Expressão */
    ASTNode* expr = parse_expression(parser);
    if (!expr) {
        ast_destroy(assign);
        ast_destroy(var);
        return NULL;
    }
    
    /* Adicionar filhos após todas as verificações */
    ast_add_child(assign, var);
    ast_add_child(assign, expr);
    
    if (!expect_token(parser, TOKEN_PONTO_VIRG)) {
        ast_destroy(assign);
        return NULL;
    }
    
    return assign;
}

/* Analisar chamada de função */
static ASTNode* parse_function_call(Parser* parser) {
    ASTNode* call = create_node(parser, AST_FUNCTION_CALL);
    if (!call) return NULL;
    
    /* Nome da função */
    if (!expect_token(parser, TOKEN_FUNCAO_ID)) {
        ast_destroy(call);
        return NULL;
    }
    
    strncpy(call->data.literal.string_val, parser->lexer->current_token.value, MAX_STRING_LENGTH - 1);
    
    /* Parâmetros */
    if (!expect_token(parser, TOKEN_ABRE_PAREN)) {
        ast_destroy(call);
        return NULL;
    }
    
    /* Lista de argumentos */
    if (!match_token(parser, TOKEN_FECHA_PAREN)) {
        do {
            ASTNode* arg = parse_expression(parser);
            if (!arg) {
                ast_destroy(call);
                return NULL;
            }
            
            ast_add_child(call, arg);
            
            if (match_token(parser, TOKEN_VIRGULA)) {
                consume_token(parser, TOKEN_VIRGULA);
            } else {
                break;
            }
        } while (1);
    }
    
    if (!expect_token(parser, TOKEN_FECHA_PAREN)) {
        ast_destroy(call);
        return NULL;
    }
    
    if (!expect_token(parser, TOKEN_PONTO_VIRG)) {
        ast_destroy(call);
        return NULL;
    }
    
    return call;
}

/* Analisar dimensões do tipo */
static int parse_type_dimensions(Parser* parser, TypeInfo* type_info) {
    /* Verificar se tem dimensões */
    if (match_token(parser, TOKEN_ABRE_COLCH)) {
        consume_token(parser, TOKEN_ABRE_COLCH);
        
        /* Ler dimensão */
        Token dim_token = parser->lexer->current_token;
        if (dim_token.type != TOKEN_NUMERO_INT && dim_token.type != TOKEN_NUMERO_DEC) {
            parser_error(parser, "Dimensão deve ser um número");
            return 0;
        }
        
        if (dim_token.type == TOKEN_NUMERO_INT) {
            type_info->size = string_to_int(dim_token.value);
            consume_token(parser, TOKEN_NUMERO_INT);
        } else {
            /* Número decimal - pegar parte inteira */
            char* dot = strchr(dim_token.value, '.');
            if (dot) *dot = '\0';
            type_info->size = string_to_int(dim_token.value);
            if (dot) *dot = '.';
            consume_token(parser, TOKEN_NUMERO_DEC);
            
            /* Para decimal, pode ter parte decimal */
            if (match_token(parser, TOKEN_PONTO)) {
                consume_token(parser, TOKEN_PONTO);
                
                Token scale_token = parser->lexer->current_token;
                if (scale_token.type != TOKEN_NUMERO_INT) {
                    parser_error(parser, "Precisão decimal deve ser um número inteiro");
                    return 0;
                }
                
                type_info->scale = string_to_int(scale_token.value);
                consume_token(parser, TOKEN_NUMERO_INT);
            }
        }
        
        if (!expect_token(parser, TOKEN_FECHA_COLCH)) {
            return 0;
        }
    }
    
    return 1;
}

/* Função principal de análise */
ASTNode* parser_parse(Parser* parser) {
    return parse_program(parser);
} 