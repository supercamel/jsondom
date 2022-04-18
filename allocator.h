#ifndef JSON_DOM_ALLOCATOR_H
#define JSON_DOM_ALLOCATOR_H

#include <stddef.h>
#include <stdint.h>


typedef struct _JsonDomAllocatorPage JsonDomAllocatorPage;

extern JsonDomAllocatorPage* json_dom_allocator_page_new(size_t reserve_blocks);
extern void json_dom_allocator_page_delete(JsonDomAllocatorPage* page);
extern void* json_dom_allocator_page_alloc(JsonDomAllocatorPage* page);
extern void json_dom_allocator_page_free(JsonDomAllocatorPage* page, void* ptr);


typedef struct _JsonDomAllocator JsonDomAllocator;

extern JsonDomAllocator* json_dom_allocator_new();
extern void json_dom_allocator_delete(JsonDomAllocator* alloc);
extern void* json_dom_allocator_alloc(JsonDomAllocator* alloc, size_t size);
extern void* json_dom_allocator_alloc_chunk(JsonDomAllocator* alloc);
extern void json_dom_allocator_free(JsonDomAllocator* alloc, void* ptr);


#endif


