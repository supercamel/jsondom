#ifndef JSON_DOM_ALLOCATOR_H
#define JSON_DOM_ALLOCATOR_H

#include <stddef.h>
#include <stdint.h>


typedef struct _JsonDomAllocatorPage JsonDomAllocatorPage;

extern JsonDomAllocatorPage* json_dom_allocator_page_new(size_t reserve_blocks, size_t chunk_size);
extern void json_dom_allocator_page_delete(JsonDomAllocatorPage* page);
extern void* json_dom_allocator_page_alloc(JsonDomAllocatorPage* page);
extern void json_dom_allocator_page_free(JsonDomAllocatorPage* page, void* ptr);


typedef struct _JsonDomAllocator JsonDomAllocator;

extern JsonDomAllocator* json_dom_allocator_new(size_t chunk_size, size_t initial_pages);
extern void json_dom_allocator_delete(JsonDomAllocator* alloc);
extern void* json_dom_allocator_alloc(JsonDomAllocator* alloc, size_t size);
extern void* json_dom_allocator_alloc_chunk(JsonDomAllocator* alloc);
extern char* json_dom_allocator_strdup(JsonDomAllocator* alloc, const char* str);
extern void json_dom_allocator_free(JsonDomAllocator* alloc, void* ptr);


#endif


