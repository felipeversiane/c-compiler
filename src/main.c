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
    
    if (ast) {
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
    } else {
        printf("Nenhuma AST gerada devido a erros\n");
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
    
    if (!ast) {
        printf("Erro na análise sintática - não é possível executar\n");
        parser_destroy(parser);
        lexer_destroy(lexer);
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
    
    /* Alocar buffer */
    char* content = (char*)malloc(size + 1);
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
    
    /* Executar teste de estresse da memória */
    printf("=== TESTE DE ESTRESSE DA MEMÓRIA ===\n");
    int memory_test_ok = memory_stress_test(g_memory_manager);
    if (memory_test_ok) {
        printf("Teste de memória PASSOU!\n");
    } else {
        printf("Teste de memória FALHOU!\n");
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
    
    /* Testar analisador léxico */
    test_lexer(source_code);
    
    /* Testar analisador sintático e semântico */
    test_parser(source_code);
    
    /* Testar interpretador */
    test_interpreter(source_code);
    
    /* Verificar se houve erros */
    if (g_error_count > 0) {
        printf("COMPILAÇÃO FALHOU: %d erro(s) encontrado(s)\n", g_error_count);
        free(source_code);
        memory_manager_destroy(g_memory_manager);
        return 1;
    }
    
    printf("COMPILAÇÃO E EXECUÇÃO CONCLUÍDAS COM SUCESSO!\n");
    
    /* Limpar e finalizar */
    free(source_code);
    memory_manager_destroy(g_memory_manager);
    
    return 0;
} 