#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "collection.h"
#include "slist.h"
#include "parser.h"

/**
 * A Collection has a linked list of IndexNodes which contain a skip list of DOM nodes ordered by the key
 * Indexes contain a SkipList of pointers to the DOM objects. 
 * The 'head' index uses the '_id' field for ordering and it owns the memory of the DOM objects
 * other indexes should not call 'free' when removing entries
 */

typedef struct _JsonDomIndexNode JsonDomIndexNode;

typedef struct _JsonDomIndexNode {
    JsonDomSList* index_list;
    JsonDomKey key;
    JsonDomIndexNode* next;
} JsonDomIndexNode;


typedef struct _JsonDomCollection {
    JsonDomIndexNode index_head;
    char* name; 
} JsonDomCollection;

static int compare_string_keys(void* lh, void* rh, void* usrptr) {
    JsonDomKey* idxkey = usrptr;

    const JsonDomNode* lhnode = json_dom_node_get_member(lh, *idxkey);
    const JsonDomNode* rhnode = json_dom_node_get_member(rh, *idxkey);

    JsonDomNodeType lhtype = json_dom_node_get_type(lhnode);
    JsonDomNodeType rhtype = json_dom_node_get_type(rhnode);


    if(rhtype != lhtype) {
        if(rhtype < lhtype) {
            return 1;
        }
        return -1;
    }

    const char* str1 = json_dom_node_get_string(lhnode);
    const char* str2 = json_dom_node_get_string(rhnode);
    char tc1, tc2, tc3;

    while(tc1 && !tc3)
    {
        tc1 = *str1++;
        tc2 = *str2++;
        tc3 = tc1 ^ tc2;
    }

    return tc1 - tc2;
    //return 0;
    /*

       return strcmp(json_dom_node_get_string(lhnode),
       json_dom_node_get_string(rhnode));

    /*
    switch(lhtype) {
    case JSON_DOM_NODE_TYPE_INT:
    {
    int lhp = json_dom_node_get_int_member(lh, *idxkey);
    int rhp = json_dom_node_get_int_member(rh, *idxkey);

    if(lhp == rhp) {
    return 0;
    }
    else if(lhp < rhp) {
    return -1;
    }
    else {
    return 1;
    }
    }
    break;
    case JSON_DOM_NODE_TYPE_STRING:
    {
    }
    break;
    }
    return 0;
    */
}
/*
   static JsonDomIndex* json_dom_index_new(JsonDomKey key) 
   {
   JsonDomAllocator* alloc = json_dom_node_get_allocator();
   JsonDomIndex* index = json_dom_allocator_alloc_chunk(alloc);
   index->index_list = json_dom_slist_new();
   index->key = key;
   return index;
   }
   */

JsonDomCollection* json_dom_collection_new(const char* name)
{
    JsonDomCollection* self = malloc(sizeof(JsonDomCollection));
    int name_length = strlen(name);
    self->name = malloc(name_length);
    strcpy(self->name, name);

    JsonDomKey idkey = json_dom_node_key_new("_id");
    self->index_head.index_list = json_dom_slist_new();
    self->index_head.key = idkey;
    self->index_head.next = 0;

    return self;
}

void json_dom_collection_insert_str(JsonDomCollection* self, const char* str) 
{
    JsonDomNode* dom_node = json_dom_parse(str);
    if(dom_node) {
        JsonDomIndexNode* index_iter = &self->index_head;
        while(index_iter) {
            if(json_dom_node_has_member(dom_node, index_iter->key)) {
                //json_dom_slist_append(index_iter->index_list, dom_node);
                json_dom_slist_insert(index_iter->index_list, dom_node, compare_string_keys, &index_iter->key);
            }
            index_iter = index_iter->next;
        }
    }
    
}


void json_dom_collection_print(JsonDomCollection* self)
{
    const JsonDomSListNode* iter = json_dom_slist_begin(self->index_head.index_list);
    while(iter != json_dom_slist_end(self->index_head.index_list)) {
        JsonStringBuilder builder = json_string_builder_new();
        json_dom_node_stringify(iter->payload, &builder);
        printf("%s\n", builder.head);
        free(builder.head);

        iter = json_dom_slist_node_next(iter);
    }
}

