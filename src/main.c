#include "../include/compiler.h"
#include <sys/stat.h>

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
    
    /* Verificar se há memória suficiente para o arquivo + buffer de segurança */
    size_t required_memory = size + 1024; // 1KB extra para segurança
    if (g_memory_manager->allocated + required_memory > g_memory_manager->limit) {
        error_report(ERROR_MEMORY, 0, 0, "Memória insuficiente para abrir o arquivo");
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

    /* Verificar uso de memória após leitura */
    int warning_level = memory_check_limit(g_memory_manager);
    if (warning_level > 0) {
        printf("\nAVISO: Alto uso de memória após leitura do arquivo.\n");
        printf("Memória alocada: %zu bytes\n", g_memory_manager->allocated);
        printf("Limite de memória: %zu bytes\n", g_memory_manager->limit);
        printf("Uso atual: %.1f%%\n\n", 
            (double)g_memory_manager->allocated / g_memory_manager->limit * 100.0);
    }

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
    
    /* Verificar se houve erros */
    if (g_error_count > 0) {
        printf("ANÁLISE LÉXICA FALHOU: %d erro(s) encontrado(s)\n", g_error_count);
    } else {
        printf("ANÁLISE LÉXICA CONCLUÍDA COM SUCESSO!\n");
    }
    
    /* Limpar e finalizar */
    memory_free(g_memory_manager, source_code);
    memory_manager_destroy(g_memory_manager);
    
    return 0;
} 
