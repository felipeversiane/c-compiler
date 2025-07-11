#include "../include/compiler.h"

/* Função para testar o analisador léxico */
void test_lexer(const char* source_code) {
    printf("=== TESTANDO ANALISADOR LÉXICO ===\n");
    printf("Código fonte:\n%s\n", source_code);
    printf("=== TOKENS GERADOS ===\n");
    
    /* Criar lexer */
    Lexer* lexer = lexer_create(source_code);
    if (!lexer) {
        fprintf(stderr, "Erro ao criar lexer\n");
        return;
    }
    
    /* Processar todos os tokens */
    Token token;
    int token_count = 0;
    
    do {
        token = lexer_next_token(lexer);
        print_token(token);
        token_count++;
        
        /* Prevenir loop infinito */
        if (token_count > 1000) {
            printf("ERRO: Muitos tokens processados - possível loop infinito\n");
            break;
        }
        
    } while (token.type != TOKEN_EOF && token.type != TOKEN_ERROR);
    
    printf("=== ESTATÍSTICAS ===\n");
    printf("Total de tokens: %d\n", token_count);
    printf("Erros encontrados: %d\n", lexer->error_count);
    
    /* Limpar */
    lexer_destroy(lexer);
    printf("=== TESTE CONCLUÍDO ===\n\n");
}

/* Função para testar o analisador sintático e semântico */
void test_parser(const char* source_code) {
    printf("=== TESTANDO ANALISADOR SINTÁTICO ===\n");
    printf("Código fonte:\n%s\n", source_code);
    
    /* Criar lexer */
    Lexer* lexer = lexer_create(source_code);
    if (!lexer) {
        fprintf(stderr, "Erro ao criar lexer\n");
        return;
    }
    
    /* Criar parser */
    Parser* parser = parser_create(lexer);
    if (!parser) {
        fprintf(stderr, "Erro ao criar parser\n");
        lexer_destroy(lexer);
        return;
    }
    
    /* Analisar código */
    ASTNode* ast = parser_parse(parser);
    
    /* Imprimir resultados da análise sintática */
    printf("=== ESTATÍSTICAS SINTÁTICAS ===\n");
    printf("Erros léxicos: %d\n", lexer->error_count);
    printf("Erros sintáticos: %d\n", parser->error_count);

    if (lexer->error_count > 0 || parser->error_count > 0 || !ast) {
        printf("Nenhuma AST gerada devido a erros\n");
    } else {
        /* Imprimir AST */
        ast_print(ast, 0);

        /* Análise semântica */
        printf("\n=== TESTANDO ANALISADOR SEMÂNTICO ===\n");
        int semantic_ok = semantic_analyze(ast, parser->symbol_table);

        if (semantic_ok) {
            printf("Análise semântica concluída com sucesso!\n");
        } else {
            printf("Erros semânticos encontrados.\n");
        }

        ast_destroy(ast);
    }
    
    /* Limpar */
    parser_destroy(parser);
    lexer_destroy(lexer);
    printf("=== TESTE CONCLUÍDO ===\n\n");
}

/* Função para testar o interpretador */
void test_interpreter(const char* source_code) {
    printf("=== TESTANDO INTERPRETADOR ===\n");
    printf("Código fonte:\n%s\n", source_code);
    
    /* Criar lexer */
    Lexer* lexer = lexer_create(source_code);
    if (!lexer) {
        fprintf(stderr, "Erro ao criar lexer\n");
        return;
    }
    
    /* Criar parser */
    Parser* parser = parser_create(lexer);
    if (!parser) {
        fprintf(stderr, "Erro ao criar parser\n");
        lexer_destroy(lexer);
        return;
    }
    
    /* Analisar código */
    ASTNode* ast = parser_parse(parser);

    if (lexer->error_count > 0 || parser->error_count > 0 || !ast) {
        printf("Erro na análise sintática - não é possível executar\n");
        parser_destroy(parser);
        lexer_destroy(lexer);
        if (ast) ast_destroy(ast);
        return;
    }

    /* Análise semântica */
    int semantic_ok = semantic_analyze(ast, parser->symbol_table);

    if (!semantic_ok) {
        printf("Erro na análise semântica - não é possível executar\n");
        ast_destroy(ast);
        parser_destroy(parser);
        lexer_destroy(lexer);
        return;
    }
    
    /* Criar e executar interpretador */
    Interpreter* interpreter = interpreter_create(ast, parser->symbol_table);
    if (!interpreter) {
        printf("Erro ao criar interpretador\n");
        ast_destroy(ast);
        parser_destroy(parser);
        lexer_destroy(lexer);
        return;
    }
    
    /* Executar código */
    int execution_ok = interpreter_execute(interpreter);
    
    if (execution_ok) {
        printf("Código executado com sucesso!\n");
    } else {
        printf("Erro durante a execução\n");
    }
    
    /* Limpar */
    interpreter_destroy(interpreter);
    ast_destroy(ast);
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    printf("=== TESTE DO INTERPRETADOR CONCLUÍDO ===\n\n");
}

/* Função para ler arquivo */
char* read_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Erro: Não foi possível abrir o arquivo '%s'\n", filename);
        return NULL;
    }
    
    /* Obter tamanho do arquivo */
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (size <= 0) {
        fprintf(stderr, "Erro: Arquivo vazio ou erro ao ler tamanho\n");
        fclose(file);
        return NULL;
    }
    
    /* Alocar buffer usando gerenciador de memória */
    char* content = (char*)memory_alloc(g_memory_manager, size + 1);
    if (!content) {
        fprintf(stderr, "Erro: Falha ao alocar memória para arquivo\n");
        fclose(file);
        return NULL;
    }
    
    /* Ler conteúdo */
    size_t read_size = fread(content, 1, size, file);
    content[read_size] = '\0';
    
    fclose(file);
    return content;
}

/* Função principal */
int main(int argc, char* argv[]) {
    printf("=== COMPILADOR DE LINGUAGEM PERSONALIZADA ===\n");
    printf("Versão: 1.0.0\n");
    printf("Desenvolvido seguindo especificações ISO/IEC 9899-1990\n\n");
    
    /* Inicializar gerenciador de memória global */
    g_memory_manager = memory_manager_create();
    if (!g_memory_manager) {
        fprintf(stderr, "ERRO CRÍTICO: Falha ao inicializar gerenciador de memória\n");
        return 1;
    }
    
    /* Validar integridade da memória */
    memory_validate_integrity(g_memory_manager);
    
    /* Verificar argumentos */
    if (argc < 2) {
        printf("Uso: %s <arquivo_fonte>\n", argv[0]);
        printf("Exemplo: %s examples/hello_world.txt\n", argv[0]);
        
        /* Executar teste básico */
        printf("\n=== EXECUTANDO TESTE BÁSICO ===\n");
        const char* test_code = 
            "principal() {\n"
            "    inteiro !x = 10;\n"
            "    inteiro !y = 20;\n"
            "    inteiro !resultado;\n"
            "    \n"
            "    !resultado = !x + !y;\n"
            "    escreva(\"Soma: \", !resultado);\n"
            "    \n"
            "    retorno 0;\n"
            "}\n";
        
        test_lexer(test_code);
        test_parser(test_code);
        test_interpreter(test_code);
        
        memory_manager_destroy(g_memory_manager);
        return 0;
    }
    
    /* Ler arquivo fonte */
    char* source_code = read_file(argv[1]);
    if (!source_code) {
        memory_manager_destroy(g_memory_manager);
        return 1;
    }
    
    printf("Arquivo: %s\n", argv[1]);
    printf("Tamanho: %zu bytes\n\n", strlen(source_code));
    
    /* === Pipeline de compilação === */

    /* 1. Análise léxica */
    Lexer* lex = lexer_create(source_code);
    if (!lex) {
        memory_free(g_memory_manager, source_code);
        memory_manager_destroy(g_memory_manager);
        return 1;
    }

    Token t;
    do {
        t = lexer_next_token(lex);
    } while (t.type != TOKEN_EOF && t.type != TOKEN_ERROR);

    if (lex->error_count > 0) {
        printf("Erro léxico encontrado. Abortando.\n");
        lexer_destroy(lex);
        memory_free(g_memory_manager, source_code);
        memory_manager_destroy(g_memory_manager);
        return 1;
    }

    lexer_destroy(lex);

    /* 2. Análise sintática */
    Lexer* lexer2 = lexer_create(source_code);
    if (!lexer2) {
        memory_free(g_memory_manager, source_code);
        memory_manager_destroy(g_memory_manager);
        return 1;
    }

    Parser* parser = parser_create(lexer2);
    if (!parser) {
        lexer_destroy(lexer2);
        memory_free(g_memory_manager, source_code);
        memory_manager_destroy(g_memory_manager);
        return 1;
    }

    ASTNode* ast = parser_parse(parser);

    if (lexer2->error_count > 0 || parser->error_count > 0 || !ast) {
        printf("Erro sintático encontrado. Abortando.\n");
        if (ast) ast_destroy(ast);
        parser_destroy(parser);
        lexer_destroy(lexer2);
        memory_free(g_memory_manager, source_code);
        memory_manager_destroy(g_memory_manager);
        return 1;
    }

    /* 3. Análise semântica */
    int semantic_ok = semantic_analyze(ast, parser->symbol_table);
    if (!semantic_ok) {
        printf("Erro semântico encontrado. Abortando.\n");
        ast_destroy(ast);
        parser_destroy(parser);
        lexer_destroy(lexer2);
        memory_free(g_memory_manager, source_code);
        memory_manager_destroy(g_memory_manager);
        return 1;
    }

    /* 4. Execução */
    Interpreter* interpreter = interpreter_create(ast, parser->symbol_table);
    if (!interpreter) {
        ast_destroy(ast);
        parser_destroy(parser);
        lexer_destroy(lexer2);
        memory_free(g_memory_manager, source_code);
        memory_manager_destroy(g_memory_manager);
        return 1;
    }

    int exec_ok = interpreter_execute(interpreter);
    if (!exec_ok) {
        printf("Erro durante a execução.\n");
    } else {
        printf("COMPILAÇÃO E EXECUÇÃO CONCLUÍDAS COM SUCESSO!\n");
    }

    interpreter_destroy(interpreter);
    ast_destroy(ast);
    parser_destroy(parser);
    lexer_destroy(lexer2);
    
    /* Limpar e finalizar */
    memory_free(g_memory_manager, source_code);
    memory_manager_destroy(g_memory_manager);
    
    return 0;
} 
