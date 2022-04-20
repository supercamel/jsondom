#include "allocator.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define CHUNK_SIZE 128 

typedef struct _JsonDomAllocatorPage JsonDomAllocatorPage;

typedef struct _BlockHead {
    void* next;
} BlockHead;

typedef union _Block {
    BlockHead head;
    uint8_t* bytes;
} Block;

struct _JsonDomAllocatorPage {
    Block* free_head;
    void* start;
    size_t n_blocks;
};


JsonDomAllocatorPage* json_dom_allocator_page_new(size_t reserve_blocks, size_t chunk_size)
{
    printf("making new page\n");
    printf("reserving %u blocks\n", reserve_blocks);
    printf("chunk size %u \n", chunk_size);

    JsonDomAllocatorPage* alloc = (JsonDomAllocatorPage*)malloc(sizeof(JsonDomAllocatorPage));
    alloc->n_blocks = reserve_blocks;
    alloc->start = malloc(chunk_size*reserve_blocks);
    alloc->free_head = (Block*)alloc->start;
    alloc->free_head->head.next = 0;

    uint8_t* n = (uint8_t*)alloc->free_head;
    uint8_t* end = &n[chunk_size*(reserve_blocks-1)];

    Block* nb;
    size_t count = 0;
    while(count++ < reserve_blocks) {
        nb = (Block*)n;
        nb->head.next = &n[chunk_size];
        n = nb->head.next;
    }
    nb = n;
    nb->head.next = 0;
    return alloc;
}




void json_dom_allocator_page_delete(JsonDomAllocatorPage* self)
{
    free(self->start);
    free(self);
}


void* json_dom_allocator_page_alloc(JsonDomAllocatorPage* self)
{
    Block* n = self->free_head;
    if(n != 0) {
        self->free_head = (Block*)n->head.next;
        return n;
    }
    return 0;
}

void json_dom_allocator_page_free(JsonDomAllocatorPage* self, void* ptr)
{
    if(ptr != 0) 
    {
        Block* block = (Block*)ptr;
        block->head.next = self->free_head;
        self->free_head = block;
    }
}


struct _JsonDomAllocator{
    JsonDomAllocatorPage** pages;
    size_t n_pages;
    int current_page;
    size_t chunk_size;
};


JsonDomAllocator* json_dom_allocator_new(size_t chunk_size, size_t reserve_pages)
{
    printf("allocator new\n");
    JsonDomAllocator* allocator = (JsonDomAllocator*)malloc(sizeof(JsonDomAllocator));
    allocator->n_pages = reserve_pages;
    allocator->pages = (JsonDomAllocatorPage**)malloc(sizeof(JsonDomAllocatorPage*)*reserve_pages);
    allocator->current_page = 0;
    allocator->chunk_size = chunk_size;

    for(int i = 0; i < reserve_pages; i++) {
        allocator->pages[i] = json_dom_allocator_page_new(1024*1024, chunk_size);
    }
    printf("allocator done\n");

    return allocator;
}

void json_dom_allocator_delete(JsonDomAllocator* self)
{
    for(size_t i = 0; i < self->n_pages; i++) {
        json_dom_allocator_page_delete(self->pages[i]);
    }

    free(self->pages);
    free(self);
}


void* json_dom_allocator_alloc(JsonDomAllocator* self, size_t sz) 
{
    if(sz > self->chunk_size) {
        return malloc(sz);
    }
    else {
        return json_dom_allocator_alloc_chunk(self);
    }
}

void* json_dom_allocator_alloc_chunk(JsonDomAllocator* self) 
{
    void* r = json_dom_allocator_page_alloc(self->pages[self->current_page]);
    if(r == 0) {
        for(size_t i = 0; i < self->n_pages; i++) 
        {
            r = json_dom_allocator_page_alloc(self->pages[i]);
            if(r != 0) 
            {
                self->current_page = i;
                break;
            }
        }

        if(r == 0) {
            self->n_pages++;
            self->pages = realloc(self->pages, sizeof(JsonDomAllocatorPage*)*self->n_pages);
            self->current_page = self->n_pages-1;

            size_t chunks = 1024*1024;
            JsonDomAllocatorPage* page = json_dom_allocator_page_new(chunks, self->chunk_size);
            self->pages[self->n_pages-1] = page;

            r = json_dom_allocator_page_alloc(page);
        }
    }

    return r;
}


void json_dom_allocator_free(JsonDomAllocator* self, void* ptr)
{
    for(size_t i = 0; i < self->n_pages; i++) {
        void* start = self->pages[i]->start;
        Block* end_block = &((Block*)start)[self->pages[i]->n_blocks];
        void* end = end_block;

        if((ptr >= start) && (ptr <= end)) {
            json_dom_allocator_page_free(self->pages[i], ptr);
            return;
        }
    }

    free(ptr);
}

