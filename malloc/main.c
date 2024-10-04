#include <stdio.h>
#include <stddef.h>

#define POOL_SIZE 1024 
#define BLOCK_SIZE 16   

static unsigned char memory_pool[POOL_SIZE];

typedef struct Block {
    size_t size;          
    struct Block* next;   
    int free;              
} Block;

static Block* free_list = (Block*)memory_pool;

void inicializa_pool() {
    free_list->size = POOL_SIZE - sizeof(Block);  
    free_list->next = NULL;                      
    free_list->free = 1;                  
}

void* aloca(size_t tamanho) {
    Block* current = free_list;
    
    while (current != NULL) {
        if (current->free && current->size >= tamanho) {
            current->free = 0;

            if (current->size > tamanho + sizeof(Block)) {
                Block* novo_bloco = (Block*)((unsigned char*)current + sizeof(Block) + tamanho);
                novo_bloco->size = current->size - tamanho - sizeof(Block);
                novo_bloco->next = current->next;
                novo_bloco->free = 1;
                current->next = novo_bloco;
                current->size = tamanho;
            }

            return (unsigned char*)current + sizeof(Block);
        }

        current = current->next;
    }

    return NULL;  
}

void libera(void* ptr) {
    if (ptr == NULL) {
        return;
    }

    Block* bloco = (Block*)((unsigned char*)ptr - sizeof(Block));
    bloco->free = 1; 

    Block* current = free_list;
    while (current != NULL) {
        if (current->free && current->next && current->next->free) {
            current->size += current->next->size + sizeof(Block);
            current->next = current->next->next;
        }
        current = current->next;
    }
}

int main() {

    inicializa_pool();

    int *ptr1 = (int *)aloca(5 * sizeof(int)); 
    int *ptr2 = (int *)aloca(5 * sizeof(int)); 

    if (ptr1 == NULL || ptr2 == NULL) {
        printf("Falha na alocação de memória.\n");
        return 1;
    }

    for (int i = 0; i < 5; i++) {
        ptr1[i] = i * 2;
        ptr2[i] = i * 3;
    }

    int *p = ptr1;
    for (int i = 0; i < 5; i++) {
        printf("ptr1[%d] = %d, endereço = %p\n", i, *p, (void*)p);
        p++;
    }

    if (ptr1 < ptr2) {
        printf("O bloco de ptr1 começa antes de ptr2 na memória.\n");
    } else {
        printf("O bloco de ptr2 começa antes de ptr1 na memória.\n");
    }

    libera(ptr1);
    libera(ptr2);

    return 0;
}
