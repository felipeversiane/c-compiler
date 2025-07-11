#include "../include/compiler.h"

/* Tabela de palavras-chave */
static const struct {
    const char* word;
    TokenType type;
} keywords[] = {
    {"principal", TOKEN_PRINCIPAL},
    {"funcao", TOKEN_FUNCAO},
    {"leia", TOKEN_LEIA},
    {"escreva", TOKEN_ESCREVA},
    {"se", TOKEN_SE},
    {"senao", TOKEN_SENAO},
    {"para", TOKEN_PARA},
    {"enquanto", TOKEN_ENQUANTO},
    {"retorno", TOKEN_RETORNO},
    {"inteiro", TOKEN_INTEIRO},
    {"texto", TOKEN_TEXTO},
    {"decimal", TOKEN_DECIMAL},
    {NULL, TOKEN_EOF}
};

/* Criar lexer */
Lexer* lexer_create(const char* source) {
    if (!source) return NULL;
    
    Lexer* lexer = (Lexer*)memory_alloc(g_memory_manager, sizeof(Lexer));
    if (!lexer) {
        error_report(ERROR_MEMORY, 0, 0, "Falha ao alocar lexer");
        return NULL;
    }
    
    lexer->source = string_duplicate(source);
    if (!lexer->source) {
        memory_free(g_memory_manager, lexer);
        error_report(ERROR_MEMORY, 0, 0, "Falha ao duplicar código fonte");
        return NULL;
    }
    
    lexer->pos = 0;
    lexer->line = 1;
    lexer->column = 1;
    lexer->length = strlen(source);
    lexer->error_count = 0;
    lexer->input = NULL;
    
    return lexer;
}

/* Destruir lexer */
void lexer_destroy(Lexer* lexer) {
    if (!lexer) return;
    
    if (lexer->source) {
        memory_free(g_memory_manager, lexer->source);
    }
    
    if (lexer->input) {
        fclose(lexer->input);
    }
    
    memory_free(g_memory_manager, lexer);
}

/* Caractere atual */
static char current_char(Lexer* lexer) {
    if (lexer->pos >= lexer->length) {
        return '\0';
    }
    return lexer->source[lexer->pos];
}

/* Próximo caractere */
static char peek_char(Lexer* lexer, int offset) {
    int pos = lexer->pos + offset;
    if (pos >= lexer->length) {
        return '\0';
    }
    return lexer->source[pos];
}

/* Avançar posição */
static void advance(Lexer* lexer) {
    if (lexer->pos < lexer->length) {
        if (lexer->source[lexer->pos] == '\n') {
            lexer->line++;
            lexer->column = 1;
        } else {
            lexer->column++;
        }
        lexer->pos++;
    }
}

/* Pular espaços em branco */
void lexer_skip_whitespace(Lexer* lexer) {
    while (lexer->pos < lexer->length) {
        char c = current_char(lexer);
        if (c == ' ' || c == '\t' || c == '\r') {
            advance(lexer);
        } else if (c == '\n') {
            advance(lexer);
            /* Quebra de linha pode ser significativa em alguns contextos */
        } else {
            break;
        }
    }
}

/* Pular comentários (assumindo // para comentários de linha) */
void lexer_skip_comment(Lexer* lexer) {
    if (current_char(lexer) == '/' && peek_char(lexer, 1) == '/') {
        /* Comentário de linha - pular até fim da linha */
        while (lexer->pos < lexer->length && current_char(lexer) != '\n') {
            advance(lexer);
        }
        if (current_char(lexer) == '\n') {
            advance(lexer);
        }
    }
}

/* Verificar se string é palavra-chave */
int lexer_is_keyword(const char* str) {
    for (int i = 0; keywords[i].word != NULL; i++) {
        if (strcmp(str, keywords[i].word) == 0) {
            return 1;
        }
    }
    return 0;
}

/* Obter tipo de token para palavra-chave */
TokenType lexer_get_keyword_type(const char* str) {
    for (int i = 0; keywords[i].word != NULL; i++) {
        if (strcmp(str, keywords[i].word) == 0) {
            return keywords[i].type;
        }
    }
    return TOKEN_UNKNOWN;
}

/* Ler identificador (variável ou função) */
static Token read_identifier(Lexer* lexer) {
    Token token;
    token.line = lexer->line;
    token.column = lexer->column;
    
    int start_pos = lexer->pos;
    char c = current_char(lexer);
    
    /* Verificar prefixo */
    if (c == '!') {
        /* Variável: ! + letra minúscula + [a-zA-Z0-9]* */
        advance(lexer);
        c = current_char(lexer);
        
        if (!islower(c)) {
            token.type = TOKEN_ERROR;
            snprintf(token.value, MAX_TOKEN_LENGTH, "Variável deve começar com ! seguido de letra minúscula");
            lexer->error_count++;
            return token;
        }
        
        token.type = TOKEN_VARIAVEL;
    } else if (c == '_' && peek_char(lexer, 1) == '_') {
        /* Função: __ + [a-zA-Z0-9] + [a-zA-Z0-9]* */
        advance(lexer); /* primeiro _ */
        advance(lexer); /* segundo _ */
        c = current_char(lexer);
        
        if (!isalnum(c)) {
            token.type = TOKEN_ERROR;
            snprintf(token.value, MAX_TOKEN_LENGTH, "Função deve começar com __ seguido de letra ou número");
            lexer->error_count++;
            return token;
        }
        
        token.type = TOKEN_FUNCAO_ID;
    } else {
        /* Palavra normal - pode ser palavra-chave */
        token.type = TOKEN_UNKNOWN;
    }
    
    /* Ler resto do identificador */
    char buffer[MAX_TOKEN_LENGTH];
    int i = 0;
    
    /* Copiar prefixo */
    for (int j = start_pos; j < lexer->pos && i < MAX_TOKEN_LENGTH - 1; j++) {
        buffer[i++] = lexer->source[j];
    }
    
    /* Ler caracteres alfanuméricos */
    while (lexer->pos < lexer->length && i < MAX_TOKEN_LENGTH - 1) {
        c = current_char(lexer);
        if (isalnum(c)) {
            buffer[i++] = c;
            advance(lexer);
        } else {
            break;
        }
    }
    
    buffer[i] = '\0';
    strcpy(token.value, buffer);
    
    /* Se não tem prefixo especial, verificar se é palavra-chave */
    if (token.type == TOKEN_UNKNOWN) {
        if (lexer_is_keyword(buffer)) {
            token.type = lexer_get_keyword_type(buffer);
        } else {
            /* Erro: identificador malformado */
            token.type = TOKEN_ERROR;
            snprintf(token.value, MAX_TOKEN_LENGTH,
                     "ID malformado: %.200s",
                     buffer);
            return token;
        }

        /* Erro: identificador sem marcador correto */
        token.type = TOKEN_ERROR;
        snprintf(token.value, MAX_TOKEN_LENGTH,
                 "ID deve começar com ! (var) ou __ (func): %.200s",
                 buffer);
        return token;
    }
    
    return token;
}

/* Ler número (inteiro ou decimal) */
static Token read_number(Lexer* lexer) {
    Token token;
    token.line = lexer->line;
    token.column = lexer->column;
    
    char buffer[MAX_TOKEN_LENGTH];
    int i = 0;
    int has_dot = 0;
    
    /* Ler dígitos e ponto decimal */
    while (lexer->pos < lexer->length && i < MAX_TOKEN_LENGTH - 1) {
        char c = current_char(lexer);
        
        if (isdigit(c)) {
            buffer[i++] = c;
            advance(lexer);
        } else if (c == '.' && !has_dot) {
            /* Verificar se próximo caractere é dígito */
            if (isdigit(peek_char(lexer, 1))) {
                has_dot = 1;
                buffer[i++] = c;
                advance(lexer);
            } else {
                break;
            }
        } else {
            break;
        }
    }
    
    buffer[i] = '\0';
    strcpy(token.value, buffer);
    
    if (has_dot) {
        token.type = TOKEN_NUMERO_DEC;
    } else {
        token.type = TOKEN_NUMERO_INT;
    }
    
    return token;
}

/* Ler string literal */
static Token read_string(Lexer* lexer) {
    Token token;
    token.line = lexer->line;
    token.column = lexer->column;
    
    char buffer[MAX_STRING_LENGTH];
    int i = 0;
    
    /* Pular aspas inicial */
    advance(lexer);
    
    /* Ler até aspas final */
    while (lexer->pos < lexer->length && i < MAX_STRING_LENGTH - 1) {
        char c = current_char(lexer);
        
        if (c == '"') {
            /* Aspas final encontrada */
            advance(lexer);
            buffer[i] = '\0';
            strcpy(token.value, buffer);
            token.type = TOKEN_STRING;
            return token;
        } else if (c == '\n') {
            /* String não pode quebrar linha */
            token.type = TOKEN_ERROR;
            snprintf(token.value, MAX_TOKEN_LENGTH, "String não fechada - quebra de linha encontrada");
            lexer->error_count++;
            return token;
        } else {
            buffer[i++] = c;
            advance(lexer);
        }
    }
    
    /* String não fechada */
    token.type = TOKEN_ERROR;
    snprintf(token.value, MAX_TOKEN_LENGTH, "String não fechada - fim de arquivo alcançado");
    lexer->error_count++;
    return token;
}

/* Próximo token */
Token lexer_next_token(Lexer* lexer) {
    Token token;
    
    /* Pular espaços e comentários */
    while (lexer->pos < lexer->length) {
        lexer_skip_whitespace(lexer);
        if (current_char(lexer) == '/' && peek_char(lexer, 1) == '/') {
            lexer_skip_comment(lexer);
        } else {
            break;
        }
    }
    
    /* Verificar fim de arquivo */
    if (lexer->pos >= lexer->length) {
        token.type = TOKEN_EOF;
        token.value[0] = '\0';
        token.line = lexer->line;
        token.column = lexer->column;
        return token;
    }
    
    char c = current_char(lexer);
    token.line = lexer->line;
    token.column = lexer->column;
    
    /* Números */
    if (isdigit(c)) {
        return read_number(lexer);
    }
    
    /* Strings */
    if (c == '"') {
        return read_string(lexer);
    }
    
    /* Identificadores, variáveis, funções, palavras-chave */
    if (isalpha(c) || c == '!' || c == '_') {
        return read_identifier(lexer);
    }
    
    /* Operadores e delimitadores de dois caracteres */
    char next_c = peek_char(lexer, 1);
    
    if (c == '=' && next_c == '=') {
        token.type = TOKEN_IGUAL;
        strcpy(token.value, "==");
        advance(lexer);
        advance(lexer);
        return token;
    }
    
    if (c == '<' && next_c == '>') {
        token.type = TOKEN_DIFERENTE;
        strcpy(token.value, "<>");
        advance(lexer);
        advance(lexer);
        return token;
    }
    
    if (c == '<' && next_c == '=') {
        token.type = TOKEN_MENOR_IGUAL;
        strcpy(token.value, "<=");
        advance(lexer);
        advance(lexer);
        return token;
    }
    
    if (c == '>' && next_c == '=') {
        token.type = TOKEN_MAIOR_IGUAL;
        strcpy(token.value, ">=");
        advance(lexer);
        advance(lexer);
        return token;
    }
    
    if (c == '&' && next_c == '&') {
        token.type = TOKEN_E;
        strcpy(token.value, "&&");
        advance(lexer);
        advance(lexer);
        return token;
    }
    
    if (c == '|' && next_c == '|') {
        token.type = TOKEN_OU;
        strcpy(token.value, "||");
        advance(lexer);
        advance(lexer);
        return token;
    }
    
    /* Operadores e delimitadores de um caractere */
    token.value[0] = c;
    token.value[1] = '\0';
    advance(lexer);
    
    switch (c) {
        case '+': token.type = TOKEN_MAIS; break;
        case '-': token.type = TOKEN_MENOS; break;
        case '*': token.type = TOKEN_MULT; break;
        case '/': token.type = TOKEN_DIV; break;
        case '^': token.type = TOKEN_POT; break;
        case '<': token.type = TOKEN_MENOR; break;
        case '>': token.type = TOKEN_MAIOR; break;
        case '=': token.type = TOKEN_ATRIB; break;
        case '(': token.type = TOKEN_ABRE_PAREN; break;
        case ')': token.type = TOKEN_FECHA_PAREN; break;
        case '{': token.type = TOKEN_ABRE_CHAVE; break;
        case '}': token.type = TOKEN_FECHA_CHAVE; break;
        case '[': token.type = TOKEN_ABRE_COLCH; break;
        case ']': token.type = TOKEN_FECHA_COLCH; break;
        case ';': token.type = TOKEN_PONTO_VIRG; break;
        case ',': token.type = TOKEN_VIRGULA; break;
        case '.': token.type = TOKEN_PONTO; break;
        case '\n': token.type = TOKEN_NEWLINE; break;
        
        default:
            token.type = TOKEN_ERROR;
            snprintf(token.value, MAX_TOKEN_LENGTH, "Caractere inválido: '%c' (ASCII %d)", c, (int)c);
            lexer->error_count++;
            break;
    }
    
    return token;
}

/* Espiar próximo token sem consumir */
Token lexer_peek_token(Lexer* lexer) {
    /* Salvar estado atual */
    int saved_pos = lexer->pos;
    int saved_line = lexer->line;
    int saved_column = lexer->column;
    
    /* Obter próximo token */
    Token token = lexer_next_token(lexer);
    
    /* Restaurar estado */
    lexer->pos = saved_pos;
    lexer->line = saved_line;
    lexer->column = saved_column;
    
    return token;
} 