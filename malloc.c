//malloc.c
#include "minicrt.h"

typedef struct _heap_header
{
    enum{
        HEAP_BLOCK_FREE = 0XABABABAB,
        HEAP_BLOCK_USED = 0XCDCDCDCD,
    } type;

    unsigned size;
    struct _heap_header *next;
    struct _heap_header *prev;
} heap_header;

#define ADDR_ADD(a,o) (((char*)(a)) + o)
#define HEADER_SIZE (sizeof(heap_header))

static heap_header* list_head = NULL;

void free(void *ptr)
{
    heap_header* header = (heap_header*) ADDR_ADD(ptr, -HEADER_SIZE);
    if(header->type != HEAP_BLOCK_USED)
            return;

    header->type = HEAP_BLOCK_FREE;
    if(header->prev!=NULL && header->prev->type == HEAP_BLOCK_FREE){
        header->prev->next = header->next;
        if(header->next != NULL)
            header->next->prev = header->prev;
        header->prev->size += header->size;

        header = header->prev;
    }

    if(header->next != NULL && header->next->type == HEAP_BLOCK_FREE){
        header->size += header->next->size;
        header->next = header->next->next;
        if(header->next != NULL)
            header->next->prev = header;
    }
}

void* malloc( unsigned size)
{
    heap_header *header;

    if(size == 0)
        return NULL;

    header = list_head;

    while(header != 0){
        if(header->type == HEAP_BLOCK_USED){
            header = header->next;
            continue;
        }

        if(header->size > size + HEADER_SIZE && header->size <= size+HEADER_SIZE*2){
            header->type = HEAP_BLOCK_USED;
        }

        if(header->size > size+HEADER_SIZE*2){
            heap_header *next = (heap_header*) ADDR_ADD(header, size+HEADER_SIZE);
            next->prev = header;
            next->next = header->next;
            next->type = HEAP_BLOCK_FREE;
            next->size = header->size - (size + HEADER_SIZE);
            header->next = next;
            header->size = size + HEADER_SIZE;
            header->type = HEAP_BLOCK_USED;
            return ADDR_ADD(header, HEADER_SIZE);
        }
        header = header->next;
    }
    return NULL;
}

#ifndef WIN32
static void* brk(void* end_data_segment){
    void *ret = 0;
    #ifdef X64
    asm("movq $45, %%rax \n\t"
        "movq %1, %%rbx \n\t"
        "int $0x80 \n\t"
        "movq %%rax, %0 \n\t": "=r"(ret) : "m"(end_data_segment)
        );
    #else
    asm("movl $45, %%eax \n\t"
        "movl %1, %%ebx \n\t"
        "int $0x80 \n\t"
        "mov %%eax, %0 \n\t": "=r"(ret) : "m"(end_data_segment)
        );
    #endif // X64
    return ret;
}
#endif // WIN32

#ifdef WIN32
#include<Windows.h>
#endif // WIN32

int mini_crt_heap_init()
{
    void* base = NULL;
    heap_header *header = NULL;
    unsigned heap_size = 32*1024*1024;

#ifdef WIN32
    base = VirtualAlloc(0, heap_size, MEM_COMMIT | MEM_RESERVE , PAGE_READWRITE);
    if(base == NULL)
        return 0;
#else
    base = brk(0);
    void* end = ADDR_ADD(base, heap_size);
    end =  brk(end);
    if(!end)
        return 0;
#endif // WIN32

    header = (heap_header*) base;

    header->size = heap_size;
    header->type = HEAP_BLOCK_FREE;
    header->next = NULL;
    header->prev = NULL;

    list_head = header;
    return 1;
}
