#ifndef COMPILER_H
#define COMPILER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdint.h>

/* Configurações globais */
#define MAX_MEMORY_KB 512  
#define MAX_MEMORY_BYTES (MAX_MEMORY_KB * 1024)
#define MAX_TOKEN_LENGTH 256
#define MAX_IDENTIFIER_LENGTH 64
#define MAX_STRING_LENGTH 512
#define MAX_LINE_LENGTH 1024
#define MAX_ERROR_MESSAGE_LENGTH 512

/* Configurações de memória interna */
#define MEMORY_BLOCK_OVERHEAD 32  /* Bytes extras para cada bloco alocado */
#define MEMORY_GUARD_SIZE 16      /* Tamanho dos guardas de memória */
#define DEBUG_MEMORY 1            /* Ativar debug de memória */
#define MEMORY_POISON_VALUE 0xDEADBEEF  /* Valor usado para envenenar memória liberada */

/* Tipos de token */
typedef enum {
    TOKEN_EOF = 0,
    
    /* Palavras-chave */
    TOKEN_PRINCIPAL,    /* principal */
    TOKEN_FUNCAO,      /* funcao */
    TOKEN_LEIA,        /* leia */
    TOKEN_ESCREVA,     /* escreva */
    TOKEN_SE,          /* se */
    TOKEN_SENAO,       /* senao */
    TOKEN_PARA,        /* para */
    TOKEN_ENQUANTO,    /* enquanto */
    TOKEN_RETORNO,     /* retorno */
    
    /* Tipos de dados */
    TOKEN_INTEIRO,     /* inteiro */
    TOKEN_TEXTO,       /* texto */
    TOKEN_DECIMAL,     /* decimal */
    
    /* Identificadores */
    TOKEN_VARIAVEL,    /* !nome */
    TOKEN_FUNCAO_ID,   /* __nome */
    
    /* Literais */
    TOKEN_NUMERO_INT,  /* 123 */
    TOKEN_NUMERO_DEC,  /* 123.45 */
    TOKEN_STRING,      /* "texto" */
    
    /* Operadores aritméticos */
    TOKEN_MAIS,        /* + */
    TOKEN_MENOS,       /* - */
    TOKEN_MULT,        /* * */
    TOKEN_DIV,         /* / */
    TOKEN_POT,         /* ^ */
    
    /* Operadores relacionais */
    TOKEN_IGUAL,       /* == */
    TOKEN_DIFERENTE,   /* <> */
    TOKEN_MENOR,       /* < */
    TOKEN_MENOR_IGUAL, /* <= */
    TOKEN_MAIOR,       /* > */
    TOKEN_MAIOR_IGUAL, /* >= */
    
    /* Operadores lógicos */
    TOKEN_E,           /* && */
    TOKEN_OU,          /* || */
    
    /* Operadores de atribuição */
    TOKEN_ATRIB,       /* = */
    
    /* Delimitadores */
    TOKEN_ABRE_PAREN,  /* ( */
    TOKEN_FECHA_PAREN, /* ) */
    TOKEN_ABRE_CHAVE,  /* { */
    TOKEN_FECHA_CHAVE, /* } */
    TOKEN_ABRE_COLCH,  /* [ */
    TOKEN_FECHA_COLCH, /* ] */
    
    /* Pontuação */
    TOKEN_PONTO_VIRG,  /* ; */
    TOKEN_VIRGULA,     /* , */
    TOKEN_PONTO,       /* . */
    
    /* Tokens especiais */
    TOKEN_NEWLINE,     /* quebra de linha */
    TOKEN_UNKNOWN,     /* token desconhecido */
    TOKEN_ERROR        /* erro léxico */
} TokenType;

/* Estrutura do token */
typedef struct {
    TokenType type;
    char value[MAX_TOKEN_LENGTH];
    int line;
    int column;
} Token;

/* Estado do lexer */
typedef struct {
    FILE* input;
    char* source;
    int pos;
    int line;
    int column;
    int length;
    Token current_token;
    int error_count;
} Lexer;

/* Gerenciador de memória */
typedef struct {
    size_t allocated;
    size_t peak_usage;
    size_t process_peak_usage;
    size_t limit;
    int allocation_count;
    int deallocation_count;
} MemoryManager;

/* Tipos de erro */
typedef enum {
    ERROR_LEXICAL,
    ERROR_SYNTAX,
    ERROR_SEMANTIC,
    ERROR_RUNTIME,
    ERROR_MEMORY
} ErrorType;

/* Funções do analisador léxico */
Lexer* lexer_create(const char* source);
void lexer_destroy(Lexer* lexer);
Token lexer_next_token(Lexer* lexer);
Token lexer_peek_token(Lexer* lexer);
void lexer_skip_whitespace(Lexer* lexer);
void lexer_skip_comment(Lexer* lexer);
int lexer_is_keyword(const char* str);
TokenType lexer_get_keyword_type(const char* str);

/* Funções de gerenciamento de memória */
MemoryManager* memory_manager_create(void);
void memory_manager_destroy(MemoryManager* mm);
void* memory_alloc(MemoryManager* mm, size_t size);
void* memory_alloc_debug(MemoryManager* mm, size_t size, const char* file, int line, const char* function);
void memory_free(MemoryManager* mm, void* ptr);
void memory_free_debug(MemoryManager* mm, void* ptr, const char* file, int line, const char* function);
void* memory_realloc(MemoryManager* mm, void* ptr, size_t new_size);
void* memory_realloc_debug(MemoryManager* mm, void* ptr, size_t new_size, const char* file, int line, const char* function);
void memory_report(MemoryManager* mm);
void memory_report_detailed(MemoryManager* mm);
int memory_check_limit(MemoryManager* mm);
int memory_validate_integrity(MemoryManager* mm);

/* Macros para facilitar debug de memória */
#define MEMORY_ALLOC(mm, size) memory_alloc_debug(mm, size, __FILE__, __LINE__, __func__)
#define MEMORY_FREE(mm, ptr) memory_free_debug(mm, ptr, __FILE__, __LINE__, __func__)
#define MEMORY_REALLOC(mm, ptr, size) memory_realloc_debug(mm, ptr, size, __FILE__, __LINE__, __func__)

/* Tratamento de Erros */
void error_report(ErrorType type, int line, int column, const char* message);
void error_print_context(const char* source, int line, int column);

/* Utilitários */
char* string_duplicate(const char* str);
int string_to_int(const char* str);
double string_to_double(const char* str);
void print_token(Token token);
const char* token_type_to_string(TokenType type);

/* Variáveis globais */
extern MemoryManager* g_memory_manager;
extern int g_error_count;
extern int g_warning_count;

#endif /* COMPILER_H */ 