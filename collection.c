#include <string.h>
#include <stdlib.h>
#include "collection.h"
#include "slist.h"
#include "dom_node.h"

typedef struct _JsonDomCollection {
    JsonDomSList* primary_list;
    char* name; 
} JsonDomCollection;


JsonDomCollection* json_dom_collection_new(const char* name)
{
    JsonDomCollection* self = malloc(sizeof(JsonDomCollection));
    int name_length = strlen(name);
    self->name = malloc(name_length);
    strcpy(self->name, name);

    self->primary_list = json_dom_slist_new();
}

void json_dom_collection_insert_str(JsonDomCollection* self, const char* str) 
{

}


