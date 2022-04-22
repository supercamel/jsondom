#ifndef JSON_DOM_COLLECTION_H
#define JSON_DOM_COLLECTION_H

#include "dom_node.h"
#include "slist.h"

typedef struct _JsonDomCollectionRange {
    JsonDomSListNode* first;
    JsonDomSListNode* last;
} JsonDomCollectionRange;


typedef struct _JsonDomCollection JsonDomCollection;

JsonDomCollection* json_dom_collection_new(const char* name);

void json_dom_collection_create_index(JsonDomCollection* self, const char* key);
void json_dom_collection_append(JsonDomCollection* self, JsonDomNode* node);
void json_dom_collection_append_str(JsonDomCollection* self, const char* json_str);

/**
 * Travels along the primary index and returns the first document that matches the field 'key' with value 'keynode'
 */
const JsonDomNode* json_dom_collection_find_linear(JsonDomCollection* self, const char* key, JsonDomNode* keynode);

/**
 * 
 */
const JsonDomNode* json_dom_collection_find_one_int(JsonDomCollection* self, const char* key, int value);
const JsonDomNode* json_dom_collection_find_one_uint(JsonDomCollection* self, const char* key, uint64_t value);
const JsonDomNode* json_dom_collection_find_one_double(JsonDomCollection* self, const char* key, double value);
const JsonDomNode* json_dom_collection_find_one_string(JsonDomCollection* self, const char* key, const char* value);
unsigned int json_dom_collection_length(JsonDomCollection* self);


void json_dom_collection_print(JsonDomCollection* self);


#endif
