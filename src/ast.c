#include "../include/compiler.h"

/* Criar nó da AST */
ASTNode* ast_create_node(ASTNodeType type) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) {
        error_report(ERROR_MEMORY, 0, 0, "Falha ao alocar nó da AST");
        return NULL;
    }
    
    node->type = type;
    node->data_type = TYPE_VOID;
    node->children = NULL;
    node->child_count = 0;
    node->child_capacity = 0;
    
    /* Zerar dados específicos do nó */
    memset(&node->data, 0, sizeof(node->data));
    
    return node;
}

/* Destruir nó da AST e seus filhos */
void ast_destroy(ASTNode* node) {
    if (!node) return;
    
    /* Destruir filhos */
    if (node->children) {
        for (int i = 0; i < node->child_count; i++) {
            ast_destroy(node->children[i]);
        }
        free(node->children);
    }
    
    free(node);
}

/* Adicionar filho a um nó */
void ast_add_child(ASTNode* parent, ASTNode* child) {
    if (!parent || !child) return;
    
    /* Verificar se precisa aumentar capacidade */
    if (parent->child_count >= parent->child_capacity) {
        int new_capacity = parent->child_capacity == 0 ? 4 : parent->child_capacity * 2;
        ASTNode** new_children = (ASTNode**)realloc(parent->children, 
                                                   new_capacity * sizeof(ASTNode*));
        
        if (!new_children) {
            error_report(ERROR_MEMORY, 0, 0, "Falha ao realocar filhos da AST");
            return;
        }
        
        parent->children = new_children;
        parent->child_capacity = new_capacity;
    }
    
    parent->children[parent->child_count++] = child;
}

/* Imprimir AST (para debug) */
static void print_ast_recursive(ASTNode* node, int depth) {
    if (!node) return;
    
    /* Indentação */
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
    
    /* Tipo do nó */
    printf("- ");
    switch (node->type) {
        case AST_PROGRAM:
            printf("Programa");
            break;
        case AST_FUNCTION_DEF:
            printf("Função: %s", node->data.function.name);
            break;
        case AST_VAR_DECL:
            printf("Variável: tipo %s", data_type_to_string(node->data.var_decl.var_type));
            break;
        case AST_ASSIGNMENT:
            printf("Atribuição");
            break;
        case AST_IF_STMT:
            printf("Se");
            break;
        case AST_FOR_STMT:
            printf("Para");
            break;
        case AST_RETURN_STMT:
            printf("Retorno");
            break;
        case AST_FUNCTION_CALL:
            printf("Chamada de Função");
            break;
        case AST_BINARY_OP:
            printf("Operação Binária: %s", token_type_to_string(node->data.binary_op.operator));
            break;
        case AST_UNARY_OP:
            printf("Operação Unária");
            break;
        case AST_IDENTIFIER:
            printf("Identificador");
            break;
        case AST_LITERAL:
            switch (node->data_type) {
                case TYPE_INTEIRO:
                    printf("Literal Inteiro: %d", node->data.literal.int_val);
                    break;
                case TYPE_DECIMAL:
                    printf("Literal Decimal: %f", node->data.literal.decimal_val);
                    break;
                case TYPE_TEXTO:
                    printf("Literal Texto: \"%s\"", node->data.literal.string_val);
                    break;
                default:
                    printf("Literal (tipo desconhecido)");
                    break;
            }
            break;
        case AST_BLOCK:
            printf("Bloco");
            break;
        default:
            printf("Nó Desconhecido");
            break;
    }
    printf("\n");
    
    /* Imprimir filhos */
    for (int i = 0; i < node->child_count; i++) {
        print_ast_recursive(node->children[i], depth + 1);
    }
}

/* Função pública para imprimir AST */
void ast_print(ASTNode* root, int depth) {
    printf("\n=== ÁRVORE SINTÁTICA ABSTRATA ===\n");
    print_ast_recursive(root, depth);
    printf("================================\n\n");
} 