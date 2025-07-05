#include "../include/compiler.h"

/* Variáveis globais */
MemoryManager* g_memory_manager = NULL;
int g_error_count = 0;
int g_warning_count = 0;

/* Duplicar string */
char* string_duplicate(const char* str) {
    if (!str) return NULL;
    
    size_t len = strlen(str);
    char* copy = (char*)malloc(len + 1);
    if (!copy) {
        error_report(ERROR_MEMORY, 0, 0, "Falha ao alocar memória para string");
        return NULL;
    }
    
    strcpy(copy, str);
    return copy;
}

/* Converter string para inteiro */
int string_to_int(const char* str) {
    if (!str) return 0;
    return atoi(str);
}

/* Converter string para double */
double string_to_double(const char* str) {
    if (!str) return 0.0;
    return atof(str);
}

/* Imprimir token */
void print_token(Token token) {
    printf("Token: %s | Value: '%s' | Line: %d | Column: %d\n",
           token_type_to_string(token.type),
           token.value,
           token.line,
           token.column);
}

/* Converter tipo de token para string */
const char* token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_EOF: return "EOF";
        case TOKEN_PRINCIPAL: return "PRINCIPAL";
        case TOKEN_FUNCAO: return "FUNCAO";
        case TOKEN_LEIA: return "LEIA";
        case TOKEN_ESCREVA: return "ESCREVA";
        case TOKEN_SE: return "SE";
        case TOKEN_SENAO: return "SENAO";
        case TOKEN_PARA: return "PARA";
        case TOKEN_RETORNO: return "RETORNO";
        case TOKEN_INTEIRO: return "INTEIRO";
        case TOKEN_TEXTO: return "TEXTO";
        case TOKEN_DECIMAL: return "DECIMAL";
        case TOKEN_VARIAVEL: return "VARIAVEL";
        case TOKEN_FUNCAO_ID: return "FUNCAO_ID";
        case TOKEN_NUMERO_INT: return "NUMERO_INT";
        case TOKEN_NUMERO_DEC: return "NUMERO_DEC";
        case TOKEN_STRING: return "STRING";
        case TOKEN_MAIS: return "MAIS";
        case TOKEN_MENOS: return "MENOS";
        case TOKEN_MULT: return "MULT";
        case TOKEN_DIV: return "DIV";
        case TOKEN_POT: return "POT";
        case TOKEN_IGUAL: return "IGUAL";
        case TOKEN_DIFERENTE: return "DIFERENTE";
        case TOKEN_MENOR: return "MENOR";
        case TOKEN_MENOR_IGUAL: return "MENOR_IGUAL";
        case TOKEN_MAIOR: return "MAIOR";
        case TOKEN_MAIOR_IGUAL: return "MAIOR_IGUAL";
        case TOKEN_E: return "E";
        case TOKEN_OU: return "OU";
        case TOKEN_ATRIB: return "ATRIB";
        case TOKEN_ABRE_PAREN: return "ABRE_PAREN";
        case TOKEN_FECHA_PAREN: return "FECHA_PAREN";
        case TOKEN_ABRE_CHAVE: return "ABRE_CHAVE";
        case TOKEN_FECHA_CHAVE: return "FECHA_CHAVE";
        case TOKEN_ABRE_COLCH: return "ABRE_COLCH";
        case TOKEN_FECHA_COLCH: return "FECHA_COLCH";
        case TOKEN_PONTO_VIRG: return "PONTO_VIRG";
        case TOKEN_VIRGULA: return "VIRGULA";
        case TOKEN_PONTO: return "PONTO";
        case TOKEN_NEWLINE: return "NEWLINE";
        case TOKEN_UNKNOWN: return "UNKNOWN";
        case TOKEN_ERROR: return "ERROR";
        default: return "INVALID";
    }
}

/* Converter tipo de dados para string */
const char* data_type_to_string(DataType type) {
    switch (type) {
        case TYPE_VOID: return "void";
        case TYPE_INTEIRO: return "inteiro";
        case TYPE_TEXTO: return "texto";
        case TYPE_DECIMAL: return "decimal";
        default: return "unknown";
    }
}

/* Reportar erro */
void error_report(ErrorType type, int line, int column, const char* message) {
    const char* error_type_str;
    
    switch (type) {
        case ERROR_LEXICAL: 
            error_type_str = "ERRO LÉXICO";
            g_error_count++;
            break;
        case ERROR_SYNTAX: 
            error_type_str = "ERRO SINTÁTICO";
            g_error_count++;
            break;
        case ERROR_SEMANTIC: 
            error_type_str = "ERRO SEMÂNTICO";
            g_error_count++;
            break;
        case ERROR_RUNTIME: 
            error_type_str = "ERRO DE EXECUÇÃO";
            g_error_count++;
            break;
        case ERROR_MEMORY: 
            error_type_str = "ERRO DE MEMÓRIA";
            g_error_count++;
            break;
        default: 
            error_type_str = "ERRO DESCONHECIDO";
            g_error_count++;
            break;
    }
    
    if (line > 0 && column > 0) {
        fprintf(stderr, "%s: Linha %d, Coluna %d: %s\n", 
                error_type_str, line, column, message);
    } else {
        fprintf(stderr, "%s: %s\n", error_type_str, message);
    }
}

/* Imprimir contexto do erro */
void error_print_context(const char* source, int line, int column) {
    if (!source || line <= 0) return;
    
    const char* ptr = source;
    int current_line = 1;
    const char* line_start = source;
    
    /* Encontrar início da linha do erro */
    while (*ptr && current_line < line) {
        if (*ptr == '\n') {
            current_line++;
            line_start = ptr + 1;
        }
        ptr++;
    }
    
    /* Encontrar fim da linha */
    const char* line_end = line_start;
    while (*line_end && *line_end != '\n') {
        line_end++;
    }
    
    /* Imprimir linha */
    fprintf(stderr, "     | ");
    for (const char* p = line_start; p < line_end; p++) {
        fputc(*p, stderr);
    }
    fprintf(stderr, "\n");
    
    /* Imprimir indicador */
    fprintf(stderr, "     | ");
    for (int i = 1; i < column; i++) {
        fputc(' ', stderr);
    }
    fprintf(stderr, "^\n");
} 