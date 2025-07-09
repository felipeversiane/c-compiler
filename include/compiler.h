#ifndef COMPILER_H
#define COMPILER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <stdint.h>

/* Configurações globais */
#define MAX_MEMORY_KB 2048
#define MAX_MEMORY_BYTES (MAX_MEMORY_KB * 1024)
#define MAX_TOKEN_LENGTH 256
#define MAX_IDENTIFIER_LENGTH 64
#define MAX_STRING_LENGTH 512
#define MAX_LINE_LENGTH 1024
#define MAX_FUNCTION_PARAMS 16
#define MAX_SYMBOL_TABLE_SIZE 1024
#define MAX_ERROR_MESSAGE_LENGTH 512

/* Configurações de debug */
#define DEBUG_LEXER 0
#define DEBUG_PARSER 0
#define DEBUG_SEMANTIC 0
#define DEBUG_INTERPRETER 0
#define VERBOSE_OUTPUT 0

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

/* Tipos de dados da linguagem */
typedef enum {
    TYPE_VOID,
    TYPE_INTEIRO,
    TYPE_TEXTO,
    TYPE_DECIMAL
} DataType;

/* Estrutura para dimensões de arrays */
typedef struct {
    int size;           /* Para texto[size] */
    int precision;      /* Para decimal[antes.depois] - parte antes */
    int scale;          /* Para decimal[antes.depois] - parte depois */
} TypeInfo;

/* Entrada da tabela de símbolos */
typedef struct Symbol {
    char name[MAX_IDENTIFIER_LENGTH];
    DataType type;
    TypeInfo type_info;
    int is_function;
    int is_parameter;
    int scope_level;
    int line_declared;
    int is_initialized;
    
    /* Para variáveis */
    union {
        int int_value;
        char string_value[MAX_STRING_LENGTH];
        double decimal_value;
    } value;
    
    /* Para funções */
    int param_count;
    DataType param_types[MAX_FUNCTION_PARAMS];
    TypeInfo param_type_infos[MAX_FUNCTION_PARAMS];
    char param_names[MAX_FUNCTION_PARAMS][MAX_IDENTIFIER_LENGTH];
    
    struct Symbol* next; /* Para lista ligada */
} Symbol;

/* Tabela de símbolos */
typedef struct {
    Symbol* table[MAX_SYMBOL_TABLE_SIZE];
    int scope_level;
    int symbol_count;
} SymbolTable;

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

/* Estrutura de erro */
typedef struct {
    ErrorType type;
    char message[MAX_ERROR_MESSAGE_LENGTH];
    int line;
    int column;
    char context[MAX_LINE_LENGTH];
} Error;

/* Nó da AST */
typedef enum {
    AST_PROGRAM,
    AST_FUNCTION_DEF,
    AST_VAR_DECL,
    AST_ASSIGNMENT,
    AST_IF_STMT,
    AST_FOR_STMT,
    AST_WHILE_STMT,
    AST_RETURN_STMT,
    AST_FUNCTION_CALL,
    AST_BINARY_OP,
    AST_UNARY_OP,
    AST_IDENTIFIER,
    AST_LITERAL,
    AST_BLOCK
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    Token token;
    DataType data_type;
    
    /* Filhos do nó */
    struct ASTNode** children;
    int child_count;
    int child_capacity;
    
    /* Informações específicas do nó */
    union {
        struct {
            char name[MAX_IDENTIFIER_LENGTH];
            DataType return_type;
            int param_count;
            DataType param_types[MAX_FUNCTION_PARAMS];
            TypeInfo param_type_infos[MAX_FUNCTION_PARAMS];
            char param_names[MAX_FUNCTION_PARAMS][MAX_IDENTIFIER_LENGTH];
        } function;
        
        struct {
            DataType var_type;
            TypeInfo type_info;
        } var_decl;
        
        struct {
            TokenType operator;
        } binary_op;
        
        struct {
            int int_val;
            double decimal_val;
            char string_val[MAX_STRING_LENGTH];
        } literal;
    } data;
} ASTNode;

/* Parser */
typedef struct {
    Lexer* lexer;
    SymbolTable* symbol_table;
    ASTNode* ast;
    int error_count;
    int current_scope;
} Parser;

/* Interpretador */
typedef struct {
    ASTNode* ast;
    SymbolTable* symbol_table;
    MemoryManager* memory_manager;
    int running;
    int return_flag;
    union {
        int int_val;
        double decimal_val;
        char string_val[MAX_STRING_LENGTH];
    } return_value;
} Interpreter;

/* ================================
   PROTÓTIPOS DE FUNÇÃO
   ================================ */

/* Gerenciador de Memória */
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
int memory_stress_test(MemoryManager* mm);
int memory_validate_integrity(MemoryManager* mm);

/* Macros para facilitar debug de memória */
#define MEMORY_ALLOC(mm, size) memory_alloc_debug(mm, size, __FILE__, __LINE__, __func__)
#define MEMORY_FREE(mm, ptr) memory_free_debug(mm, ptr, __FILE__, __LINE__, __func__)
#define MEMORY_REALLOC(mm, ptr, size) memory_realloc_debug(mm, ptr, size, __FILE__, __LINE__, __func__)

/* Analisador Léxico */
Lexer* lexer_create(const char* source);
void lexer_destroy(Lexer* lexer);
Token lexer_next_token(Lexer* lexer);
Token lexer_peek_token(Lexer* lexer);
void lexer_skip_whitespace(Lexer* lexer);
void lexer_skip_comment(Lexer* lexer);
int lexer_is_keyword(const char* str);
TokenType lexer_get_keyword_type(const char* str);

/* Tabela de Símbolos */
SymbolTable* symbol_table_create(void);
void symbol_table_destroy(SymbolTable* st);
Symbol* symbol_table_insert(SymbolTable* st, const char* name, DataType type);
Symbol* symbol_table_lookup(SymbolTable* st, const char* name);
void symbol_table_enter_scope(SymbolTable* st);
void symbol_table_exit_scope(SymbolTable* st);
void symbol_table_print(SymbolTable* st);
unsigned int symbol_hash(const char* str);

/* AST */
ASTNode* ast_create_node(ASTNodeType type);
void ast_destroy(ASTNode* node);
void ast_add_child(ASTNode* parent, ASTNode* child);
void ast_print(ASTNode* node, int depth);

/* Parser */
Parser* parser_create(Lexer* lexer);
void parser_destroy(Parser* parser);
ASTNode* parser_parse(Parser* parser);
ASTNode* parser_parse_program(Parser* parser);
ASTNode* parser_parse_function(Parser* parser);
ASTNode* parser_parse_statement(Parser* parser);
ASTNode* parser_parse_expression(Parser* parser);
void parser_error(Parser* parser, const char* message);

/* Analisador Semântico */
int semantic_analyze(ASTNode* ast, SymbolTable* st);
int semantic_check_types(ASTNode* node, SymbolTable* st);
int semantic_check_scopes(ASTNode* node, SymbolTable* st);

/* Interpretador */
Interpreter* interpreter_create(ASTNode* ast, SymbolTable* st);
void interpreter_destroy(Interpreter* interpreter);
int interpreter_execute(Interpreter* interpreter);
int interpreter_execute_node(Interpreter* interpreter, ASTNode* node);

/* Tratamento de Erros */
void error_report(ErrorType type, int line, int column, const char* message);
void error_print_context(const char* source, int line, int column);

/* Utilitários */
char* string_duplicate(const char* str);
int string_to_int(const char* str);
double string_to_double(const char* str);
void print_token(Token token);
const char* token_type_to_string(TokenType type);
const char* data_type_to_string(DataType type);

/* Variáveis globais */
extern MemoryManager* g_memory_manager;
extern int g_error_count;
extern int g_warning_count;

#endif /* COMPILER_H */ 