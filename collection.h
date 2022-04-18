#ifndef JSON_DOM_COLLECTION_H
#define JSON_DOM_COLLECTION_H


typedef struct _JsonDomCollection JsonDomCollection;

JsonDomCollection* json_dom_collection_new(const char* name);

void json_dom_collection_insert_str(JsonDomCollection* self, const char* json_str);



#endif
