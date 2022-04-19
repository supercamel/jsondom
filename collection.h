#ifndef JSON_DOM_COLLECTION_H
#define JSON_DOM_COLLECTION_H

#include "dom_node.h"
#include "slist.h"

typedef struct _JsonDomCollection JsonDomCollection;

JsonDomCollection* json_dom_collection_new(const char* name);

void json_dom_collection_create_index(JsonDomCollection* self, JsonDomKey key);
void json_dom_collection_insert_str(JsonDomCollection* self, const char* json_str);

void json_dom_collection_print(JsonDomCollection* self);


#endif
