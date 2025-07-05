#include "../include/compiler.h"

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
    
    /* Programa deve começar com função principal */
    if (!match_token(parser, TOKEN_PRINCIPAL)) {
        parser_error(parser, "Programa deve começar com função 'principal'");
        ast_destroy(program);
        return NULL;
    }
    
    /* Analisar função principal */
    ASTNode* main_func = parse_function(parser);
    if (!main_func) {
        ast_destroy(program);
        return NULL;
    }
    
    ast_add_child(program, main_func);
    return program;
}

/* Analisar função */
static ASTNode* parse_function(Parser* parser) {
    ASTNode* func = create_node(parser, AST_FUNCTION_DEF);
    if (!func) return NULL;
    
    /* Nome da função */
    Token func_token = parser->lexer->current_token;
    consume_token(parser, TOKEN_PRINCIPAL); /* ou TOKEN_FUNCAO_ID para outras funções */
    
    /* Copiar nome da função */
    strncpy(func->data.function.name, func_token.value, MAX_IDENTIFIER_LENGTH - 1);
    func->data.function.name[MAX_IDENTIFIER_LENGTH - 1] = '\0';
    
    /* Parâmetros */
    if (!expect_token(parser, TOKEN_ABRE_PAREN)) {
        ast_destroy(func);
        return NULL;
    }
    
    /* TODO: Analisar parâmetros quando implementar funções gerais */
    
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
            
        case TOKEN_RETORNO:
            return parse_return_statement(parser);
            
        case TOKEN_LEIA:
        case TOKEN_ESCREVA:
            return parse_io_statement(parser);
            
        case TOKEN_VARIAVEL:
            return parse_assignment(parser);
            
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
    switch (type_token.type) {
        case TOKEN_INTEIRO:
            var_decl->data.var_decl.var_type = TYPE_INTEIRO;
            break;
        case TOKEN_TEXTO:
            var_decl->data.var_decl.var_type = TYPE_TEXTO;
            break;
        case TOKEN_DECIMAL:
            var_decl->data.var_decl.var_type = TYPE_DECIMAL;
            break;
        default:
            parser_error(parser, "Tipo de variável inválido");
            ast_destroy(var_decl);
            return NULL;
    }
    
    consume_token(parser, type_token.type);
    
    /* Nome da variável */
    if (!expect_token(parser, TOKEN_VARIAVEL)) {
        ast_destroy(var_decl);
        return NULL;
    }
    
    /* Adicionar à tabela de símbolos */
    Symbol* symbol = symbol_table_insert(
        parser->symbol_table,
        parser->lexer->current_token.value,
        var_decl->data.var_decl.var_type
    );
    
    if (!symbol) {
        parser_error(parser, "Variável já declarada neste escopo");
        ast_destroy(var_decl);
        return NULL;
    }
    
    /* Verificar se tem dimensões (para texto e decimal) */
    if (match_token(parser, TOKEN_ABRE_COLCH)) {
        consume_token(parser, TOKEN_ABRE_COLCH);
        
        /* Ler dimensão */
        Token dim_token = parser->lexer->current_token;
        if (dim_token.type != TOKEN_NUMERO_INT) {
            parser_error(parser, "Dimensão deve ser um número inteiro");
            ast_destroy(var_decl);
            return NULL;
        }
        
        var_decl->data.var_decl.type_info.size = string_to_int(dim_token.value);
        consume_token(parser, TOKEN_NUMERO_INT);
        
        /* Para decimal, pode ter parte decimal */
        if (var_decl->data.var_decl.var_type == TYPE_DECIMAL &&
            match_token(parser, TOKEN_PONTO)) {
            
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
    /* TODO: Implementar análise de expressões */
    /* Por enquanto, só aceita literais básicos */
    
    ASTNode* expr = create_node(parser, AST_LITERAL);
    if (!expr) return NULL;
    
    Token token = parser->lexer->current_token;
    
    switch (token.type) {
        case TOKEN_NUMERO_INT:
            expr->data.literal.int_val = string_to_int(token.value);
            consume_token(parser, TOKEN_NUMERO_INT);
            break;
            
        case TOKEN_NUMERO_DEC:
            expr->data.literal.decimal_val = string_to_double(token.value);
            consume_token(parser, TOKEN_NUMERO_DEC);
            break;
            
        case TOKEN_STRING:
            strncpy(expr->data.literal.string_val, token.value, MAX_STRING_LENGTH - 1);
            expr->data.literal.string_val[MAX_STRING_LENGTH - 1] = '\0';
            consume_token(parser, TOKEN_STRING);
            break;
            
        default:
            parser_error(parser, "Expressão inválida");
            ast_destroy(expr);
            return NULL;
    }
    
    return expr;
}

/* Função principal de análise */
ASTNode* parser_parse(Parser* parser) {
    return parse_program(parser);
} 