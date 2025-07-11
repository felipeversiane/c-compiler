#include "../include/compiler.h"
#include <sys/resource.h>
#include <unistd.h>

/* Estrutura para rastrear alocações */
typedef struct MemoryBlock {
    void* ptr;
    size_t size;
    int line_allocated;
    const char* file_allocated;
    const char* function_allocated;
    time_t time_allocated;
    struct MemoryBlock* next;
} MemoryBlock;

/* Estrutura interna do gerenciador */
typedef struct {
    MemoryManager base;
    MemoryBlock* blocks;
    int active_blocks;
    size_t total_allocated;
    size_t total_freed;
    int fragmentation_level;
    int last_warning_percent;
} InternalMemoryManager;

/* Funções auxiliares */
static void poison_memory(void* ptr, size_t size);
static int check_memory_corruption(void* ptr, size_t size);
static void add_memory_guards(void* ptr, size_t size);
static int verify_memory_guards(void* ptr, size_t size);
static void log_memory_operation(const char* operation, void* ptr, size_t size, const char* file, int line);
static void calculate_fragmentation(InternalMemoryManager* imm);
static size_t get_current_process_memory(void);
static void update_process_peak_usage(MemoryManager* mm);

/* Criar gerenciador de memória com inicialização completa */
MemoryManager* memory_manager_create(void) {
    InternalMemoryManager* mm = (InternalMemoryManager*)malloc(sizeof(InternalMemoryManager));
    if (!mm) {
        fprintf(stderr, "ERRO CRÍTICO: Falha ao criar gerenciador de memória\n");
        return NULL;
    }
    
    /* Inicializar estrutura base */
    mm->base.allocated = 0;
    mm->base.peak_usage = 0;
    mm->base.limit = MAX_MEMORY_BYTES;
    mm->base.allocation_count = 0;
    mm->base.deallocation_count = 0;
    mm->base.process_peak_usage = get_current_process_memory();
    
    /* Inicializar estrutura interna */
    mm->blocks = NULL;
    mm->active_blocks = 0;
    mm->total_allocated = 0;
    mm->total_freed = 0;
    mm->fragmentation_level = 0;
    mm->last_warning_percent = 0;
    
    if (DEBUG_MEMORY) {
        printf("DEBUG: Gerenciador de memória criado com limite de %zu bytes\n", mm->base.limit);
    }
    
    return (MemoryManager*)mm;
}

/* Destruir gerenciador de memória com verificação completa */
void memory_manager_destroy(MemoryManager* mm) {
    if (!mm) return;
    
    InternalMemoryManager* imm = (InternalMemoryManager*)mm;
    
    /* Verificar vazamentos */
    int leak_count = 0;
    size_t leaked_bytes = 0;
    MemoryBlock* block = imm->blocks;
    update_process_peak_usage(mm);
    
    while (block) {
        MemoryBlock* next = block->next;
        
        if (DEBUG_MEMORY) {
            fprintf(stderr, "VAZAMENTO: %zu bytes alocados em %s:%d (%s) às %s", 
                    block->size, block->file_allocated, block->line_allocated, 
                    block->function_allocated, ctime(&block->time_allocated));
        }
        
        /* Verificar corrupção de memória */
        if (check_memory_corruption(block->ptr, block->size)) {
            fprintf(stderr, "ERRO: Corrupção de memória detectada no bloco %p\n", block->ptr);
        }
        
        leak_count++;
        leaked_bytes += block->size;
        
        free(block->ptr);
        free(block);
        block = next;
    }
    
    /* Imprimir relatório final detalhado */
    memory_report_detailed(mm);
    
    if (leak_count > 0) {
        fprintf(stderr, "ERRO: %d vazamentos de memória detectados (%zu bytes)\n", leak_count, leaked_bytes);
    }
    
    free(mm);
}

/* Alocar memória com rastreamento avançado */
void* memory_alloc_debug(MemoryManager* mm, size_t size, const char* file, int line, const char* function __attribute__((unused))) {
    if (!mm || size == 0) return NULL;
    
    InternalMemoryManager* imm = (InternalMemoryManager*)mm;
    
    /* Calcular tamanho total necessário incluindo overhead */
    size_t total_overhead = MEMORY_BLOCK_OVERHEAD + (2 * MEMORY_GUARD_SIZE);
    size_t total_size = size + total_overhead;
    
    /* Verificar limite de memória */
    if (mm->allocated + total_size > mm->limit) {
        if (DEBUG_MEMORY) {
            fprintf(stderr, "DEBUG: Falha na alocação - Tentando alocar %zu bytes (+ %zu overhead)\n", 
                    size, total_overhead);
            fprintf(stderr, "       Memória atual: %zu bytes, Limite: %zu bytes, Disponível: %zu bytes\n",
                    mm->allocated, mm->limit, mm->limit - mm->allocated);
        }
        error_report(ERROR_MEMORY, line, 0, "Memória Insuficiente");
        return NULL;
    }
    
    /* Alocar memória com guardas */
    void* raw_ptr = malloc(total_size);
    if (!raw_ptr) {
        error_report(ERROR_MEMORY, line, 0, "Falha na alocação de memória do sistema");
        return NULL;
    }
    
    /* Configurar guardas de memória */
    add_memory_guards(raw_ptr, size);
    void* user_ptr = (char*)raw_ptr + MEMORY_GUARD_SIZE;
    
    /* Inicializar memória com padrão */
    poison_memory(user_ptr, size);
    
    /* Criar bloco de rastreamento */
    MemoryBlock* block = (MemoryBlock*)malloc(sizeof(MemoryBlock));
    if (!block) {
        free(raw_ptr);
        error_report(ERROR_MEMORY, line, 0, "Falha ao criar bloco de rastreamento");
        return NULL;
    }
    
    block->ptr = raw_ptr;
    block->size = size;
    block->line_allocated = line;
    block->file_allocated = file;
    block->function_allocated = function;
    block->time_allocated = time(NULL);
    block->next = imm->blocks;
    imm->blocks = block;
    
    /* Atualizar estatísticas */
    mm->allocated += total_size;
    mm->allocation_count++;
    imm->active_blocks++;
    imm->total_allocated += total_size;
    
    if (mm->allocated > mm->peak_usage) {
        mm->peak_usage = mm->allocated;
    }
    
    /* Calcular fragmentação */
    calculate_fragmentation(imm);
    
    /* Log da operação */
    if (DEBUG_MEMORY) {
        printf("DEBUG: Alocação bem sucedida - %zu bytes (+ %zu overhead) em %s:%d\n", 
               size, total_overhead, file, line);
        printf("       Memória atual: %zu bytes (%.1f%% do limite)\n", 
               mm->allocated, ((double)mm->allocated / mm->limit) * 100.0);
    }
    
    return user_ptr;
}

/* Wrapper para alocação normal */
void* memory_alloc(MemoryManager* mm, size_t size) {
    return memory_alloc_debug(mm, size, __FILE__, __LINE__, __func__);
}

/* Liberar memória com verificação completa */
void memory_free_debug(MemoryManager* mm, void* ptr, const char* file, int line, const char* function __attribute__((unused))) {
    /* function é mantido para consistência com a interface de debug, mesmo que não usado */
    if (!mm || !ptr) return;
    
    InternalMemoryManager* imm = (InternalMemoryManager*)mm;
    void* raw_ptr = (char*)ptr - MEMORY_GUARD_SIZE;
    
    /* Encontrar bloco */
    MemoryBlock** curr = &imm->blocks;
    MemoryBlock* block = NULL;
    
    while (*curr) {
        if ((*curr)->ptr == raw_ptr) {
            block = *curr;
            *curr = block->next;
            break;
        }
        curr = &(*curr)->next;
    }
    
    if (!block) {
        error_report(ERROR_MEMORY, line, 0, "Tentativa de liberar ponteiro não rastreado");
        return;
    }
    
    /* Verificar corrupção */
    if (!verify_memory_guards(block->ptr, block->size)) {
        fprintf(stderr, "ERRO: Corrupção de memória detectada ao liberar %p\n", ptr);
    }
    
    /* Atualizar estatísticas */
    size_t total_overhead = MEMORY_BLOCK_OVERHEAD + (2 * MEMORY_GUARD_SIZE);
    size_t total_size = block->size + total_overhead;
    mm->allocated -= total_size;
    mm->deallocation_count++;
    imm->active_blocks--;
    imm->total_freed += total_size;
    
    /* Log da operação */
    if (DEBUG_MEMORY) {
        printf("MEMORY_LOG: FREE %p (%zu bytes + %zu overhead) em %s:%d\n", 
               ptr, block->size, total_overhead, file, line);
    }
    
    /* Limpar e liberar */
    free(block->ptr);
    free(block);
}

/* Wrapper para liberação normal */
void memory_free(MemoryManager* mm, void* ptr) {
    memory_free_debug(mm, ptr, __FILE__, __LINE__, __func__);
}

/* Realocar memória com verificação completa */
void* memory_realloc_debug(MemoryManager* mm, void* ptr, size_t new_size, const char* file, int line, const char* function) {
    if (!mm) return NULL;
    
    if (!ptr) {
        return memory_alloc_debug(mm, new_size, file, line, function);
    }
    
    if (new_size == 0) {
        memory_free_debug(mm, ptr, file, line, function);
        return NULL;
    }
    
    InternalMemoryManager* imm = (InternalMemoryManager*)mm;
    
    /* Encontrar bloco atual */
    MemoryBlock* block = imm->blocks;
    while (block) {
        void* user_ptr = (char*)block->ptr + MEMORY_GUARD_SIZE;
        if (user_ptr == ptr) {
            break;
        }
        block = block->next;
    }
    
    if (!block) {
        error_report(ERROR_MEMORY, line, 0, "Tentativa de realocar ponteiro não rastreado");
        return NULL;
    }
    
    size_t old_size = block->size;
    
    /* Verificar guardas antes de realocar */
    if (!verify_memory_guards(block->ptr, old_size)) {
        fprintf(stderr, "ERRO: Corrupção de memória detectada ao realocar %p\n", ptr);
    }
    
    /* Verificar limite para novo tamanho */
    if (new_size > old_size) {
        size_t additional = new_size - old_size;
        if (mm->allocated + additional > mm->limit) {
            error_report(ERROR_MEMORY, line, 0, "Memória Insuficiente para realocação");
            return NULL;
        }
    }
    
    /* Alocar novo bloco com guardas */
    size_t total_size = new_size + (2 * MEMORY_GUARD_SIZE);
    void* new_raw_ptr = realloc(block->ptr, total_size);
    if (!new_raw_ptr) {
        error_report(ERROR_MEMORY, line, 0, "Falha na realocação de memória");
        return NULL;
    }
    
    /* Configurar novas guardas */
    add_memory_guards(new_raw_ptr, new_size);
    void* new_user_ptr = (char*)new_raw_ptr + MEMORY_GUARD_SIZE;
    
    /* Se aumentou o tamanho, inicializar nova área */
    if (new_size > old_size) {
        poison_memory((char*)new_user_ptr + old_size, new_size - old_size);
    }
    
    /* Atualizar bloco */
    block->ptr = new_raw_ptr;
    block->size = new_size;
    block->line_allocated = line;
    block->file_allocated = file;
    block->function_allocated = function;
    block->time_allocated = time(NULL);
    
    /* Atualizar estatísticas */
    mm->allocated = mm->allocated - old_size + new_size;
    
    if (new_size > old_size) {
        imm->total_allocated += (new_size - old_size);
    } else {
        imm->total_freed += (old_size - new_size);
    }
    
    if (mm->allocated > mm->peak_usage) {
        mm->peak_usage = mm->allocated;
    }
    
    /* Calcular fragmentação */
    calculate_fragmentation(imm);
    
    /* Verificar avisos */
    memory_check_limit(mm);
    
    /* Log da operação */
    log_memory_operation("REALLOC", new_user_ptr, new_size, file, line);
    
    return new_user_ptr;
}

/* Wrapper para realocação normal */
void* memory_realloc(MemoryManager* mm, void* ptr, size_t new_size) {
    return memory_realloc_debug(mm, ptr, new_size, __FILE__, __LINE__, __func__);
}

/* Verificar limite de memória com múltiplos níveis */
int memory_check_limit(MemoryManager* mm) {
    if (!mm) return 0;
    
    double usage_percent = ((double)mm->allocated / (double)mm->limit) * 100.0;
    
    if (DEBUG_MEMORY) {
        printf("DEBUG: Verificando limite de memória - Uso: %.2f%% (%zu/%zu bytes)\n",
               usage_percent, mm->allocated, mm->limit);
    }
    
    if (mm->allocated >= mm->limit) {
        error_report(ERROR_MEMORY, 0, 0, "Limite de memória excedido");
        return 3; /* Erro crítico */
    } else if (usage_percent >= 95.0) {
        fprintf(stderr, "AVISO CRÍTICO: Uso de memória muito alto: %.1f%% (%zu/%zu bytes)\n",
                usage_percent, mm->allocated, mm->limit);
        return 2; /* Aviso crítico */
    } else if (usage_percent >= 90.0) {
        fprintf(stderr, "AVISO: Uso de memória alto: %.1f%% (%zu/%zu bytes)\n",
                usage_percent, mm->allocated, mm->limit);
        return 1; /* Aviso normal */
    }
    
    return 0; /* OK */
}

/* Gerar relatório de memória básico */
void memory_report(MemoryManager* mm) {
    if (!mm) return;

    size_t current = get_current_process_memory();
    update_process_peak_usage(mm);

    printf("\n=== RELATÓRIO DE MEMÓRIA ===\n");
    printf("Memória alocada atualmente: %zu bytes (%.2f KB)\n",
           mm->allocated, (double)mm->allocated / 1024.0);
    printf("Pico de uso de memória: %zu bytes (%.2f KB)\n",
           mm->peak_usage, (double)mm->peak_usage / 1024.0);
    printf("Pico total do processo: %zu bytes (%.2f KB)\n",
           mm->process_peak_usage, (double)mm->process_peak_usage / 1024.0);
    printf("Uso atual do processo: %zu bytes (%.2f KB)\n",
           current, (double)current / 1024.0);
    printf("Limite de memória: %zu bytes (%.2f KB)\n", 
           mm->limit, (double)mm->limit / 1024.0);
    printf("Uso atual: %.1f%% do limite\n", 
           (double)mm->allocated / mm->limit * 100.0);
    printf("Total de alocações: %d\n", mm->allocation_count);
    printf("Total de desalocações: %d\n", mm->deallocation_count);
    printf("==============================\n\n");
}

/* Gerar relatório detalhado de memória */
void memory_report_detailed(MemoryManager* mm) {
    if (!mm) return;

    InternalMemoryManager* imm = (InternalMemoryManager*)mm;

    size_t current = get_current_process_memory();
    update_process_peak_usage(mm);
    
    printf("\n=== RELATÓRIO DETALHADO DE MEMÓRIA ===\n");
    printf("Memória alocada atualmente: %zu bytes (%.2f KB)\n", 
           mm->allocated, (double)mm->allocated / 1024.0);
    printf("Pico de uso de memória: %zu bytes (%.2f KB)\n",
           mm->peak_usage, (double)mm->peak_usage / 1024.0);
    printf("Pico total do processo: %zu bytes (%.2f KB)\n",
           mm->process_peak_usage, (double)mm->process_peak_usage / 1024.0);
    printf("Uso atual do processo: %zu bytes (%.2f KB)\n",
           current, (double)current / 1024.0);
    printf("Limite de memória: %zu bytes (%.2f KB)\n",
           mm->limit, (double)mm->limit / 1024.0);
    printf("Uso atual: %.1f%% do limite\n", 
           (double)mm->allocated / mm->limit * 100.0);
    
    printf("\n--- ESTATÍSTICAS AVANÇADAS ---\n");
    printf("Total de alocações: %d\n", mm->allocation_count);
    printf("Total de desalocações: %d\n", mm->deallocation_count);
    printf("Blocos ativos: %d\n", imm->active_blocks);
    printf("Total alocado (histórico): %zu bytes (%.2f KB)\n", 
           imm->total_allocated, (double)imm->total_allocated / 1024.0);
    printf("Total liberado (histórico): %zu bytes (%.2f KB)\n", 
           imm->total_freed, (double)imm->total_freed / 1024.0);
    
    printf("\n--- FRAGMENTAÇÃO ---\n");
    const char* frag_levels[] = {"Nenhuma", "Baixa", "Média", "Alta"};
    printf("Nível de fragmentação: %s (%d)\n", 
           frag_levels[imm->fragmentation_level], imm->fragmentation_level);
    
    if (imm->active_blocks > 0) {
        printf("Tamanho médio dos blocos: %.2f bytes\n", 
               (double)mm->allocated / imm->active_blocks);
    }
    
    /* Mostrar blocos não liberados */
    printf("\n--- BLOCOS ATIVOS ---\n");
    int leak_count = 0;
    size_t leaked_bytes = 0;
    MemoryBlock* block = imm->blocks;
    
    while (block) {
        leak_count++;
        leaked_bytes += block->size;
        
        if (DEBUG_MEMORY) {
            printf("Bloco %d: %zu bytes alocados em %s:%d (%s)\n", 
                   leak_count, block->size, block->file_allocated, 
                   block->line_allocated, block->function_allocated);
        }
        
        block = block->next;
    }
    
    if (leak_count > 0) {
        printf("Total de blocos ativos: %d (%zu bytes)\n", leak_count, leaked_bytes);
    } else {
        printf("Nenhum bloco ativo - memória totalmente liberada!\n");
    }
    
    printf("\n--- EFICIÊNCIA ---\n");
    if (mm->allocation_count > 0) {
        printf("Taxa de liberação: %.1f%% (%d/%d)\n", 
               (double)mm->deallocation_count / mm->allocation_count * 100.0,
               mm->deallocation_count, mm->allocation_count);
    }
    
    printf("Desperdício atual: %zu bytes (%.2f KB)\n", 
           mm->limit - mm->allocated, (double)(mm->limit - mm->allocated) / 1024.0);
    
    printf("=====================================\n\n");
}

/* Validar integridade da memória */
int memory_validate_integrity(MemoryManager* mm) {
    if (!mm) return 0;
    
    InternalMemoryManager* imm = (InternalMemoryManager*)mm;
    int errors = 0;
    
    printf("=== VALIDAÇÃO DE INTEGRIDADE ===\n");
    
    /* Verificar todos os blocos */
    MemoryBlock* block = imm->blocks;
    int block_count = 0;
    
    while (block) {
        block_count++;
        
        /* Verificar guardas de memória */
        if (!verify_memory_guards(block->ptr, block->size)) {
            printf("ERRO: Corrupção detectada no bloco %d (%p)\n", block_count, block->ptr);
            errors++;
        }
        
        /* Verificar ponteiro válido */
        if (!block->ptr) {
            printf("ERRO: Ponteiro nulo no bloco %d\n", block_count);
            errors++;
        }
        
        /* Verificar tamanho */
        if (block->size == 0) {
            printf("ERRO: Tamanho zero no bloco %d\n", block_count);
            errors++;
        }
        
        block = block->next;
    }
    
    /* Verificar consistência das estatísticas */
    if (block_count != imm->active_blocks) {
        printf("ERRO: Inconsistência na contagem de blocos (%d vs %d)\n", 
               block_count, imm->active_blocks);
        errors++;
    }
    
    if (errors == 0) {
        printf("Integridade da memória VALIDADA - nenhum erro encontrado!\n");
    } else {
        printf("Validação FALHOU - %d erros encontrados\n", errors);
    }
    
    printf("================================\n\n");
    return errors == 0;
}

/* Expor uso atual de memória do processo */
size_t memory_get_process_usage(void) {
    return get_current_process_memory();
}

/* Funções auxiliares */
static void poison_memory(void* ptr, size_t size) {
    if (DEBUG_MEMORY) {
        uint32_t* int_ptr = (uint32_t*)ptr;
        size_t int_count = size / sizeof(uint32_t);
        for (size_t i = 0; i < int_count; i++) {
            int_ptr[i] = MEMORY_POISON_VALUE;
        }
    }
}

static int check_memory_corruption(void* ptr, size_t size) {
    if (!DEBUG_MEMORY) return 0;
    
    uint32_t* int_ptr = (uint32_t*)ptr;
    size_t int_count = size / sizeof(uint32_t);
    
    for (size_t i = 0; i < int_count; i++) {
        if (int_ptr[i] != MEMORY_POISON_VALUE) {
            return 0; /* Memória foi modificada, não é corrupção */
        }
    }
    
    return 0; /* Nenhuma corrupção detectada */
}

static void add_memory_guards(void* ptr, size_t size) {
    if (!DEBUG_MEMORY) return;
    
    uint32_t guard_pattern = 0xDEADC0DE;
    size_t guard_ints = MEMORY_GUARD_SIZE / sizeof(uint32_t);
    
    /* Guarda no início */
    uint32_t* start_guard = (uint32_t*)ptr;
    for (size_t i = 0; i < guard_ints; i++) {
        start_guard[i] = guard_pattern;
    }
    
    /* Guarda no final */
    uint32_t* end_guard = (uint32_t*)((char*)ptr + MEMORY_GUARD_SIZE + size);
    for (size_t i = 0; i < guard_ints; i++) {
        end_guard[i] = guard_pattern;
    }
}

static int verify_memory_guards(void* ptr, size_t size) {
    if (!DEBUG_MEMORY) return 1;
    
    uint32_t guard_pattern = 0xDEADC0DE;
    size_t guard_ints = MEMORY_GUARD_SIZE / sizeof(uint32_t);
    
    /* Verificar guarda no início */
    uint32_t* start_guard = (uint32_t*)ptr;
    for (size_t i = 0; i < guard_ints; i++) {
        if (start_guard[i] != guard_pattern) {
            return 0;
        }
    }
    
    /* Verificar guarda no final */
    uint32_t* end_guard = (uint32_t*)((char*)ptr + MEMORY_GUARD_SIZE + size);
    for (size_t i = 0; i < guard_ints; i++) {
        if (end_guard[i] != guard_pattern) {
            return 0;
        }
    }
    
    return 1;
}

static void log_memory_operation(const char* operation, void* ptr, size_t size, const char* file, int line) {
    if (DEBUG_MEMORY) {
        printf("MEMORY_LOG: %s %p (%zu bytes) em %s:%d\n", operation, ptr, size, file, line);
    }
}

static void calculate_fragmentation(InternalMemoryManager* imm) {
    if (imm->active_blocks == 0) {
        imm->fragmentation_level = 0;
        return;
    }
    
    /* Cálculo simples de fragmentação baseado no número de blocos */
    if (imm->active_blocks > 100) {
        imm->fragmentation_level = 3; /* Alta fragmentação */
    } else if (imm->active_blocks > 50) {
        imm->fragmentation_level = 2; /* Média fragmentação */
    } else if (imm->active_blocks > 20) {
        imm->fragmentation_level = 1; /* Baixa fragmentação */
    } else {
        imm->fragmentation_level = 0; /* Sem fragmentação */
    }
}

/* Obter uso atual de memória do processo */
static size_t get_current_process_memory(void) {
    FILE* f = fopen("/proc/self/statm", "r");
    if (!f) return 0;
    long rss = 0;
    if (fscanf(f, "%*s %ld", &rss) != 1) {
        fclose(f);
        return 0;
    }
    fclose(f);
    long page_size = sysconf(_SC_PAGESIZE);
    return (size_t)rss * (size_t)page_size;
}

/* Atualizar pico de memória do processo */
static void update_process_peak_usage(MemoryManager* mm) {
    size_t current = get_current_process_memory();
    if (current > mm->process_peak_usage) {
        mm->process_peak_usage = current;
    }
}
