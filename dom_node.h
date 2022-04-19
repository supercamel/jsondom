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

typedef struct _JsonDomKey {
    uint32_t hash;
    char* value;
    unsigned int length;
} JsonDomKey;



JsonDomKey json_dom_node_key_new(const char* key);

/**
 * Parses and de-escapes a string until either null or an unescaped double quote is found,
 * then turns it into a JsonDomKey
 */
JsonDomKey json_dom_node_key_new_escaped(const char* key);
void json_dom_node_key_free(JsonDomKey key);

JsonDomAllocator* json_dom_node_get_allocator();
void json_dom_node_initialise();
JsonDomNode* json_dom_node_new();
void json_dom_node_delete(JsonDomNode* self);

void json_dom_node_set_object(JsonDomNode* self);
void json_dom_node_set_array(JsonDomNode* self);
void json_dom_node_set_int(JsonDomNode* self, int value);
void json_dom_node_set_uint(JsonDomNode* self, unsigned int value);
void json_dom_node_set_bool(JsonDomNode* self, bool value);
void json_dom_node_set_double(JsonDomNode* self, double value);
void json_dom_node_set_string(JsonDomNode* self, const char* value);
void json_dom_node_set_null(JsonDomNode* self);
unsigned int json_dom_node_set_string_escaped(JsonDomNode* self, const char* value);

JsonDomNodeType json_dom_node_get_type(const JsonDomNode* self);
int json_dom_node_get_int(const JsonDomNode* self);
unsigned int json_dom_node_get_uint(const JsonDomNode* self);
bool json_dom_node_get_bool(const JsonDomNode* self);
double json_dom_node_get_double(const JsonDomNode* self);
const char* json_dom_node_get_string(const JsonDomNode* self);




void            json_dom_node_set_int_member(JsonDomNode* self, JsonDomKey key, int value);
int             json_dom_node_get_int_member(const JsonDomNode* self, JsonDomKey key);
void            json_dom_node_set_uint_member(JsonDomNode* self, JsonDomKey key, unsigned int value);
unsigned int    json_dom_node_get_uint_member(const JsonDomNode* self, JsonDomKey key);
void            json_dom_node_set_bool_member(JsonDomNode* self, JsonDomKey key, bool value);
bool            json_dom_node_get_bool_member(const JsonDomNode* self, JsonDomKey key);
void            json_dom_node_set_string_member(JsonDomNode* self, JsonDomKey key, const char* value);
unsigned int    json_dom_node_set_string_member_escaped(JsonDomNode* self, JsonDomKey key, const char* value);
const char*     json_dom_node_get_string_member(const JsonDomNode* self, JsonDomKey key);
void            json_dom_node_set_double_member(JsonDomNode* self, JsonDomKey key, double value);
double          json_dom_node_get_double_member(const JsonDomNode* self, JsonDomKey key);
void            json_dom_node_set_member(JsonDomNode* self, JsonDomKey key, JsonDomNode* other);
const JsonDomNode* json_dom_node_get_member(const JsonDomNode* self, JsonDomKey key);
bool            json_dom_node_has_member(const JsonDomNode* self, JsonDomKey key);

void json_dom_node_push_back(JsonDomNode* self, JsonDomNode* other);
void json_dom_node_pop_back(JsonDomNode* self);
const JsonDomNode* json_dom_node_get_index(const JsonDomNode* self, unsigned int idx);

char* json_dom_node_stringify(const JsonDomNode* self, JsonStringBuilder* builder);

#endif


