#ifndef JSON_DOM_NODE_H
#define JSON_DOM_NODE_H

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>


#include "allocator.h"
#include "stringbuilder.h"

typedef int JsonDomBool;

enum {
    JSON_DOM_NODE_TYPE_INT,
    JSON_DOM_NODE_TYPE_UINT,
    JSON_DOM_NODE_TYPE_DOUBLE,
    JSON_DOM_NODE_TYPE_STRING,
    JSON_DOM_NODE_TYPE_BOOL,
    JSON_DOM_NODE_TYPE_OBJECT,
    JSON_DOM_NODE_TYPE_ARRAY,
    JSON_DOM_NODE_TYPE_NULL,
    JSON_DOM_NODE_TYPE_INVALID
} typedef JsonDomNodeType;

typedef struct _JsonDomNode JsonDomNode;


JsonDomAllocator* json_dom_node_get_allocator();
void json_dom_node_initialise();
JsonDomNode* json_dom_node_new();
void json_dom_node_delete(JsonDomNode* self);

void json_dom_node_set_object(JsonDomNode* self);
void json_dom_node_set_array(JsonDomNode* self);
void json_dom_node_set_int(JsonDomNode* self, int value);
void json_dom_node_set_uint(JsonDomNode* self, uint64_t value);
void json_dom_node_set_bool(JsonDomNode* self, bool value);
void json_dom_node_set_double(JsonDomNode* self, double value);
void json_dom_node_set_string(JsonDomNode* self, const char* value);
void json_dom_node_set_null(JsonDomNode* self);
unsigned int json_dom_node_set_string_escaped(JsonDomNode* self, const char* value);

JsonDomNodeType json_dom_node_get_type(const JsonDomNode* self);
int json_dom_node_get_int(const JsonDomNode* self);
uint64_t json_dom_node_get_uint(const JsonDomNode* self);
bool json_dom_node_get_bool(const JsonDomNode* self);
double json_dom_node_get_double(const JsonDomNode* self);
const char* json_dom_node_get_string(const JsonDomNode* self);




void            json_dom_node_set_int_member(JsonDomNode* self, const char* key, int value);
int             json_dom_node_get_int_member(const JsonDomNode* self, const char* key);
void            json_dom_node_set_uint_member(JsonDomNode* self, const char* key, uint64_t value);
uint64_t        json_dom_node_get_uint_member(const JsonDomNode* self, const char* key);
void            json_dom_node_set_bool_member(JsonDomNode* self, const char* key, bool value);
bool            json_dom_node_get_bool_member(const JsonDomNode* self, const char* key);
void            json_dom_node_set_string_member(JsonDomNode* self, const char* key, const char* value);
unsigned int    json_dom_node_set_string_member_escaped(JsonDomNode* self, const char* key, const char* value);
const char*     json_dom_node_get_string_member(const JsonDomNode* self, const char* key);
void            json_dom_node_set_double_member(JsonDomNode* self, const char* key, double value);
double          json_dom_node_get_double_member(const JsonDomNode* self, const char* key);
void            json_dom_node_set_member(JsonDomNode* self, const char* key, JsonDomNode* other);
const JsonDomNode* json_dom_node_get_member(const JsonDomNode* self, const char* key);
bool            json_dom_node_has_member(const JsonDomNode* self, const char* key);

void json_dom_node_push_back(JsonDomNode* self, JsonDomNode* other);
void json_dom_node_pop_back(JsonDomNode* self);
const JsonDomNode* json_dom_node_get_index(const JsonDomNode* self, unsigned int idx);
unsigned int json_dom_node_length(const JsonDomNode* self);

char* json_dom_node_stringify(const JsonDomNode* self, JsonStringBuilder* builder);

#endif


