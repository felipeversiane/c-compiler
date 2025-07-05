#include "../include/compiler.h"

/* Estrutura para rastrear alocações */
typedef struct MemoryBlock {
    void* ptr;
    size_t size;
    struct MemoryBlock* next;
} MemoryBlock;

/* Estrutura interna do gerenciador */
typedef struct {
    MemoryManager base;
    MemoryBlock* blocks;
} InternalMemoryManager;

/* Criar gerenciador de memória */
MemoryManager* memory_manager_create(void) {
    InternalMemoryManager* mm = (InternalMemoryManager*)malloc(sizeof(InternalMemoryManager));
    if (!mm) {
        fprintf(stderr, "ERRO CRÍTICO: Falha ao criar gerenciador de memória\n");
        return NULL;
    }
    
    mm->base.allocated = 0;
    mm->base.peak_usage = 0;
    mm->base.limit = MAX_MEMORY_BYTES;
    mm->base.allocation_count = 0;
    mm->base.deallocation_count = 0;
    mm->blocks = NULL;
    
    return (MemoryManager*)mm;
}

/* Destruir gerenciador de memória */
void memory_manager_destroy(MemoryManager* mm) {
    if (!mm) return;
    
    InternalMemoryManager* imm = (InternalMemoryManager*)mm;
    
    /* Liberar todos os blocos restantes */
    MemoryBlock* block = imm->blocks;
    while (block) {
        MemoryBlock* next = block->next;
        fprintf(stderr, "AVISO: Vazamento de memória detectado - %zu bytes em %p\n", 
                block->size, block->ptr);
        free(block->ptr);
        free(block);
        block = next;
    }
    
    /* Imprimir relatório final */
    memory_report(mm);
    
    free(mm);
}

/* Alocar memória */
void* memory_alloc(MemoryManager* mm, size_t size) {
    if (!mm || size == 0) return NULL;
    
    InternalMemoryManager* imm = (InternalMemoryManager*)mm;
    
    /* Verificar limite de memória */
    if (mm->allocated + size > mm->limit) {
        error_report(ERROR_MEMORY, 0, 0, "Memória Insuficiente");
        fprintf(stderr, "Tentativa de alocar %zu bytes, mas apenas %zu bytes disponíveis\n",
                size, mm->limit - mm->allocated);
        return NULL;
    }
    
    /* Alocar memória */
    void* ptr = malloc(size);
    if (!ptr) {
        error_report(ERROR_MEMORY, 0, 0, "Falha na alocação de memória do sistema");
        return NULL;
    }
    
    /* Criar bloco de rastreamento */
    MemoryBlock* block = (MemoryBlock*)malloc(sizeof(MemoryBlock));
    if (!block) {
        free(ptr);
        error_report(ERROR_MEMORY, 0, 0, "Falha ao criar bloco de rastreamento");
        return NULL;
    }
    
    block->ptr = ptr;
    block->size = size;
    block->next = imm->blocks;
    imm->blocks = block;
    
    /* Atualizar estatísticas */
    mm->allocated += size;
    mm->allocation_count++;
    
    if (mm->allocated > mm->peak_usage) {
        mm->peak_usage = mm->allocated;
    }
    
    /* Verificar avisos de memória */
    memory_check_limit(mm);
    
    return ptr;
}

/* Liberar memória */
void memory_free(MemoryManager* mm, void* ptr) {
    if (!mm || !ptr) return;
    
    InternalMemoryManager* imm = (InternalMemoryManager*)mm;
    
    /* Encontrar e remover bloco */
    MemoryBlock* prev = NULL;
    MemoryBlock* block = imm->blocks;
    
    while (block) {
        if (block->ptr == ptr) {
            /* Remover da lista */
            if (prev) {
                prev->next = block->next;
            } else {
                imm->blocks = block->next;
            }
            
            /* Atualizar estatísticas */
            mm->allocated -= block->size;
            mm->deallocation_count++;
            
            /* Liberar memória */
            free(ptr);
            free(block);
            return;
        }
        
        prev = block;
        block = block->next;
    }
    
    /* Ponteiro não encontrado - possível erro */
    fprintf(stderr, "AVISO: Tentativa de liberar ponteiro não rastreado: %p\n", ptr);
}

/* Realocar memória */
void* memory_realloc(MemoryManager* mm, void* ptr, size_t new_size) {
    if (!mm) return NULL;
    
    if (!ptr) {
        return memory_alloc(mm, new_size);
    }
    
    if (new_size == 0) {
        memory_free(mm, ptr);
        return NULL;
    }
    
    InternalMemoryManager* imm = (InternalMemoryManager*)mm;
    
    /* Encontrar bloco atual */
    MemoryBlock* block = imm->blocks;
    while (block) {
        if (block->ptr == ptr) {
            break;
        }
        block = block->next;
    }
    
    if (!block) {
        error_report(ERROR_MEMORY, 0, 0, "Tentativa de realocar ponteiro não rastreado");
        return NULL;
    }
    
    size_t old_size = block->size;
    
    /* Verificar limite para novo tamanho */
    if (new_size > old_size) {
        size_t additional = new_size - old_size;
        if (mm->allocated + additional > mm->limit) {
            error_report(ERROR_MEMORY, 0, 0, "Memória Insuficiente para realocação");
            return NULL;
        }
    }
    
    /* Realocar */
    void* new_ptr = realloc(ptr, new_size);
    if (!new_ptr) {
        error_report(ERROR_MEMORY, 0, 0, "Falha na realocação de memória");
        return NULL;
    }
    
    /* Atualizar bloco */
    block->ptr = new_ptr;
    
    /* Atualizar estatísticas */
    mm->allocated = mm->allocated - old_size + new_size;
    block->size = new_size;
    
    if (mm->allocated > mm->peak_usage) {
        mm->peak_usage = mm->allocated;
    }
    
    /* Verificar avisos */
    memory_check_limit(mm);
    
    return new_ptr;
}

/* Verificar limite de memória */
int memory_check_limit(MemoryManager* mm) {
    if (!mm) return 0;
    
    double usage_percent = (double)mm->allocated / mm->limit * 100.0;
    
    if (usage_percent >= 100.0) {
        error_report(ERROR_MEMORY, 0, 0, "Limite de memória excedido (100%)");
        return 1; /* Erro crítico */
    } else if (usage_percent >= 90.0) {
        fprintf(stderr, "AVISO: Uso de memória alto: %.1f%% (%zu/%zu bytes)\n",
                usage_percent, mm->allocated, mm->limit);
        return 2; /* Aviso */
    }
    
    return 0; /* OK */
}

/* Gerar relatório de memória */
void memory_report(MemoryManager* mm) {
    if (!mm) return;
    
    InternalMemoryManager* imm = (InternalMemoryManager*)mm;
    
    printf("\n=== RELATÓRIO DE MEMÓRIA ===\n");
    printf("Memória alocada atualmente: %zu bytes (%.2f KB)\n", 
           mm->allocated, (double)mm->allocated / 1024.0);
    printf("Pico de uso de memória: %zu bytes (%.2f KB)\n", 
           mm->peak_usage, (double)mm->peak_usage / 1024.0);
    printf("Limite de memória: %zu bytes (%.2f KB)\n", 
           mm->limit, (double)mm->limit / 1024.0);
    printf("Uso atual: %.1f%% do limite\n", 
           (double)mm->allocated / mm->limit * 100.0);
    printf("Total de alocações: %d\n", mm->allocation_count);
    printf("Total de desalocações: %d\n", mm->deallocation_count);
    
    /* Mostrar blocos não liberados */
    int leak_count = 0;
    size_t leaked_bytes = 0;
    MemoryBlock* block = imm->blocks;
    
    while (block) {
        leak_count++;
        leaked_bytes += block->size;
        block = block->next;
    }
    
    if (leak_count > 0) {
        printf("VAZAMENTOS DETECTADOS: %d blocos, %zu bytes\n", 
               leak_count, leaked_bytes);
    } else {
        printf("Nenhum vazamento de memória detectado!\n");
    }
    
    printf("==============================\n\n");
} 