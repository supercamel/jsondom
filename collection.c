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
    const char* key;
    JsonDomIndexNode* next;
} JsonDomIndexNode;


typedef struct _JsonDomCollection {
    JsonDomIndexNode index_head;
    char* name; 
    uint64_t id_count;
} JsonDomCollection;


static inline int compare_keys(void* lh, void* rh, void* usrptr) {
    const char* idxkey = usrptr;

    if(rh == 0) {
        return 0;
    }
    const JsonDomNode* lhnode = json_dom_node_get_member(lh, idxkey);
    const JsonDomNode* rhnode = json_dom_node_get_member(rh, idxkey);

    int lhtype = json_dom_node_get_type(lhnode);
    int typedif = lhtype - json_dom_node_get_type(rhnode);
    if(typedif == 0) {
        if(lhtype == JSON_DOM_NODE_TYPE_INT) {
            return json_dom_node_get_int(lhnode) - json_dom_node_get_int(rhnode);
        }
        else if(lhtype == JSON_DOM_NODE_TYPE_UINT) {
            uint64_t lh = json_dom_node_get_uint(lhnode);
            uint64_t rh = json_dom_node_get_uint(rhnode);
            if(lh > rh) {
                return 1;
            }
            else if(lh < rh) {
                return -1;
            }
            else {
                return 0;
            }
        }
        else if(lhtype == JSON_DOM_NODE_TYPE_DOUBLE) {
            return json_dom_node_get_double(lhnode) - json_dom_node_get_double(rhnode);
        }
        else if(lhtype == JSON_DOM_NODE_TYPE_STRING) {
            const char* str1 = json_dom_node_get_string(lhnode);
            const char* str2 = json_dom_node_get_string(rhnode);

            return strcmp(str1, str2);
        }
        else {
            return 0;
        }
    }
    else {
        return typedif;
    }
}

JsonDomCollection* json_dom_collection_new(const char* name)
{
    JsonDomCollection* self = malloc(sizeof(JsonDomCollection));
    int name_length = strlen(name);
    self->name = malloc(name_length);
    strcpy(self->name, name);

    self->index_head.index_list = json_dom_slist_new();
    self->index_head.key = strdup("_id");
    self->index_head.next = 0;
    self->id_count = 0;

    return self;
}

void json_dom_collection_create_index(JsonDomCollection* self, const char* key) 
{
    // create the index object
    JsonDomIndexNode* index_node = malloc(sizeof(JsonDomIndexNode));
    index_node->next = 0;
    index_node->key = strdup(key);
    index_node->index_list = json_dom_slist_new();

    // add index to the list
    JsonDomIndexNode* index_iter = &self->index_head;
    while(index_iter->next) {
        index_iter = index_iter->next;
    }

    index_iter->next = index_node;

    // construct the index list
    JsonDomSListNode* iter = json_dom_slist_begin(self->index_head.index_list);
    while(iter) {
        JsonDomNode* dom_node = iter->payload;
        if(json_dom_node_has_member(dom_node, key)) {
            json_dom_slist_insert(index_node->index_list, dom_node, compare_keys, key);
        }
        iter = json_dom_slist_node_next(iter);
    }
}


void json_dom_collection_append(JsonDomCollection* self, JsonDomNode* dom_node) 
{
    json_dom_node_set_uint_member(dom_node, "_id", self->id_count++);
    if(dom_node) {
        json_dom_slist_prepend(self->index_head.index_list, dom_node);
        JsonDomIndexNode* index_iter = self->index_head.next;
        while(index_iter != 0) {
            if(json_dom_node_has_member(dom_node, index_iter->key)) {
                json_dom_slist_insert(index_iter->index_list, dom_node, compare_keys, index_iter->key);
            }
            index_iter = index_iter->next;
        }
    }
}

void json_dom_collection_append_str(JsonDomCollection* self, const char* str) 
{
    JsonDomNode* dom_node = json_dom_parse(str);
    json_dom_node_set_uint_member(dom_node, "_id", self->id_count++);
    if(dom_node) {
        JsonDomIndexNode* index_iter = &self->index_head;
        while(index_iter) {
            if(json_dom_node_has_member(dom_node, index_iter->key)) {
                json_dom_slist_insert(index_iter->index_list, dom_node, compare_keys, index_iter->key);
            }
            index_iter = index_iter->next;
        }
    }
}

const JsonDomNode* json_dom_collection_find_linear(JsonDomCollection* self, const char* key, JsonDomNode* keynode)
{
    JsonDomIndexNode* index = &self->index_head;
    JsonDomSListNode* iter = json_dom_slist_begin(index->index_list);
    while(iter) {
        JsonDomNode* dom_node = iter->payload;
        if(json_dom_node_has_member(dom_node, key)) {
            JsonDomNode* node = json_dom_node_get_member(dom_node, key);
            if(json_dom_node_get_type(node) == json_dom_node_get_type(keynode)) {
                JsonDomNodeType type = json_dom_node_get_type(node);
                switch(type){
                    case JSON_DOM_NODE_TYPE_INT:
                        {
                            if(json_dom_node_get_int(node) == json_dom_node_get_int(keynode)) {
                                return dom_node;
                            }
                        }
                        break;
                }
            }
        }
        iter = json_dom_slist_node_next(iter);
    }

    return 0;
}

const JsonDomNode* json_dom_collection_find_one_int(JsonDomCollection* self, const char* key, int value)
{
    JsonDomNode* cmp = json_dom_node_new();
    json_dom_node_set_object(cmp);
    json_dom_node_set_int_member(cmp, key, value);


    JsonDomIndexNode* index = &self->index_head;
    while(index && (strcmp(index->key, key) != 0)) {
        index = index->next; 
    }
    if(index == 0) {
        // the key is not indexed, do a linear search
        const JsonDomNode* result = json_dom_collection_find_linear(self, key, cmp);
        json_dom_node_delete(cmp);
        return result;
    }

    JsonDomSListNode* result = json_dom_slist_find_first(index->index_list, cmp, compare_keys, key);
    if(result != 0) {
        return result->payload;
    }
    return 0;
}

const JsonDomNode* json_dom_collection_find_one_uint(JsonDomCollection* self, const char* key, uint64_t value)
{
    JsonDomNode* cmp = json_dom_node_new();
    json_dom_node_set_object(cmp);
    json_dom_node_set_uint_member(cmp, key, value);

    JsonDomIndexNode* index = &self->index_head;
    while(index && (strcmp(index->key, key) != 0)) {
        index = index->next; 
    }
    if(index == 0) {
        // the key is not indexed, do a linear search
        const JsonDomNode* result = json_dom_collection_find_linear(self, key, cmp);
        json_dom_node_delete(cmp);
        return result;
    }

    JsonDomSListNode* result = json_dom_slist_find_first(index->index_list, cmp, compare_keys, key);
    if(result != 0) {
        return result->payload;
    }
    return 0;
}

const JsonDomNode* json_dom_collection_find_one_double(JsonDomCollection* self, const char* key, double value)
{
    JsonDomNode* cmp = json_dom_node_new();
    json_dom_node_set_object(cmp);
    json_dom_node_set_double_member(cmp, key, value);

    JsonDomIndexNode* index = &self->index_head;
    while(index && (strcmp(index->key, key) != 0)) {
        index = index->next; 
    }
    if(index == 0) {
        // the key is not indexed, do a linear search
        const JsonDomNode* result = json_dom_collection_find_linear(self, key, cmp);
        json_dom_node_delete(cmp);
        return result;
    }

    JsonDomSListNode* result = json_dom_slist_find_first(index->index_list, cmp, compare_keys, key);
    if(result != 0) {
        return result->payload;
    }
    return 0;
}

const JsonDomNode* json_dom_collection_find_one_string(JsonDomCollection* self, const char* key, const char* value)
{
    JsonDomNode* cmp = json_dom_node_new();
    json_dom_node_set_object(cmp);
    json_dom_node_set_string_member(cmp, key, value);

    JsonDomIndexNode* index = &self->index_head;
    while(index && (strcmp(index->key, key) != 0)) {
        index = index->next; 
    }
    if(index == 0) {
        // the key is not indexed, do a linear search
        const JsonDomNode* result = json_dom_collection_find_linear(self, key, cmp);
        json_dom_node_delete(cmp);
        return result;
    }

    JsonDomSListNode* result = json_dom_slist_find_first(index->index_list, cmp, compare_keys, key);
    if(result != 0) {
        return result->payload;
    }
    return 0;
}


unsigned int json_dom_collection_length(JsonDomCollection* self) 
{
    return self->index_head.index_list->length;
}

void json_dom_collection_print(JsonDomCollection* self)
{
    const JsonDomSListNode* iter = json_dom_slist_begin(self->index_head.index_list);
    while(iter != 0) {
        JsonStringBuilder builder = json_string_builder_new();
        json_dom_node_stringify(iter->payload, &builder);
        printf("%s\n", json_string_builder_get(&builder));
        free(builder.head);

        iter = json_dom_slist_node_next(iter);
    }
}

