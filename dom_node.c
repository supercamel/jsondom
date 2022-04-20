#include <string.h>
#include <stdlib.h>

#include <time.h>

#include "dom_node.h"
#include "allocator.h"
#include "stringbuilder.h"



JsonDomAllocator* alloc = 0;

JsonDomAllocator* json_dom_node_get_allocator() {
    return alloc;
}

typedef struct _JsonDomHead {
    void* first;
    void* last;
    unsigned int length;
} JsonDomHead;


typedef union _JsonDomNodeValue {
    int i;
    uint64_t u;
    double d;
    char* str;
    bool b;
    struct _JsonDomHead head;
} JsonDomNodeValue;

typedef struct _JsonDomNode {
    JsonDomNodeType type;
    JsonDomNodeValue value;
} JsonDomNode;


typedef struct _JsonDomValueNode {
    JsonDomNode node;
    void* next;
    void* prev;
} JsonDomValueNode;

typedef struct _JsonDomMemberNode {
    JsonDomNode node;
    JsonDomKey key;
    void* next;
    void* prev;
} JsonDomMemberNode;


/**
 * FNV1a hash algorithm
 */
/*
uint32_t fnv1a_str(const char *str)
{
    unsigned char *s = (unsigned char *)str;
    uint32_t hval = 0x811c9dc5;
    const uint32_t FNV_32_PRIME = 0x01000193;

    while (*s) {
        hval ^= (uint32_t)*s++;
        hval *= FNV_32_PRIME;
    }

    return hval;
}
*/

static bool compare_keys(JsonDomKey k1, JsonDomKey k2) 
{
    //if(k1.hash == k2.hash) {
        if(strcmp(k1.value, k2.value) == 0) {
            return true;
        }
    //}
    return false;
}

static void free_object(JsonDomNode* self) 
{
    switch(self->type) {
        case JSON_DOM_NODE_TYPE_OBJECT:
            {
                if(self->value.head.first != 0) {
                    JsonDomMemberNode* iter = self->value.head.first;
                    while(iter) {
                        JsonDomMemberNode* next = iter->next;
                        free_object(&iter->node);
                        json_dom_allocator_free(alloc, iter->key.value);
                        iter = next;
                    }
                }
            }
            break;
        case JSON_DOM_NODE_TYPE_ARRAY:
            {
                if(self->value.head.first != 0) {
                    JsonDomValueNode* iter = self->value.head.first;
                    while(iter) {
                        JsonDomValueNode* next = iter->next;
                        free_object(&iter->node);
                        iter = next;
                    }
                }
            }
            break;
        case JSON_DOM_NODE_TYPE_STRING:
            {
                json_dom_allocator_free(alloc, self->value.str);

            }
            break;
    }
}

void json_dom_node_delete(JsonDomNode* self)
{
    free_object(self);
    json_dom_allocator_free(alloc, self);
}


void json_dom_node_initialise()
{
    srand(time(0));
    alloc = json_dom_allocator_new(96, 5);
}

JsonDomKey json_dom_node_key_new(const char* keyval)
{
    JsonDomKey key;
    key.value = (char*)json_dom_allocator_alloc_chunk(alloc);

    const char* cptr = keyval;
    unsigned int count = 0;
    //uint32_t hval = 0x811c9dc5;
    //const uint32_t FNV_32_PRIME = 0x01000193;

    while(*cptr != '\0') {
        char c = *cptr++;
        key.value[count++] = c;
        //hval ^= (uint32_t)c;
        //hval *= FNV_32_PRIME;

        if(count > 62) {
            key.value[63] = '\0';
            JsonStringBuilder builder = json_string_builder_new();
            json_string_builder_append(&builder, key.value);
            json_dom_allocator_free(alloc, key.value);

            while(*cptr != '\0') {
                char c = *cptr++;
                //hval ^= (uint32_t)c;
                //hval *= FNV_32_PRIME;

                json_string_builder_append_char(&builder, c);

            }
            key.value = json_string_builder_get(&builder);
            break;
        }
    }

    //key.hash = hval;
    key.length = cptr - keyval;
    key.value[key.length] = '\0';
    return key;
}

/**
 * here we unescape the string while copying it into a new memory location AND calculating the FNV1a hash
 * all in a single pass
 */
JsonDomKey json_dom_node_key_new_escaped(const char* keyval)
{
    JsonDomKey key;
    key.value = (char*)json_dom_allocator_alloc_chunk(alloc);

    const char* cptr = keyval;
    unsigned int count = 0;
    uint32_t hval = 0x811c9dc5;
    const uint32_t FNV_32_PRIME = 0x01000193;

    while(*cptr != '\0') {
        char c = json_string_unescape_next(&cptr);
        key.value[count++] = c;
        hval ^= (uint32_t)c;
        hval *= FNV_32_PRIME;

        if(*cptr == '"') {
            key.value[count++] = '\0';
            break;
        }

        if(count > 62) {
            key.value[63] = '\0';
            // the key is too big to fit into fast memory
            // fall back to using a StringBuilder string
            JsonStringBuilder builder = json_string_builder_new();
            json_string_builder_append(&builder, key.value);
            json_dom_allocator_free(alloc, key.value);

            while(*cptr != '\0') {
                char c = json_string_unescape_next(&cptr);
                hval ^= (uint32_t)c;
                hval *= FNV_32_PRIME;

                json_string_builder_append_char(&builder, c);
                if(*cptr == '"') {
                    break;
                }
            }
            key.value = json_string_builder_get(&builder);
            break;
        }
    }

    key.hash = hval;
    key.length = cptr - keyval;
    return key;
}

void json_dom_node_key_free(JsonDomKey key)
{
    json_dom_allocator_free(alloc, key.value);
}


JsonDomNode* json_dom_node_new() 
{
    JsonDomNode* node = (JsonDomNode*)json_dom_allocator_alloc_chunk(alloc);
    node->type = JSON_DOM_NODE_TYPE_NULL;
}

void json_dom_node_set_object(JsonDomNode* self)
{
    free_object(self);
    self->type = JSON_DOM_NODE_TYPE_OBJECT;
    self->value.head.first = 0;
    self->value.head.last = 0;
}

void json_dom_node_set_array(JsonDomNode* self)
{
    free_object(self);
    self->type = JSON_DOM_NODE_TYPE_ARRAY;
    self->value.head.first = 0;
    self->value.head.last = 0;
}

void json_dom_node_set_int(JsonDomNode* self, int value)
{
    free_object(self);
    self->type = JSON_DOM_NODE_TYPE_INT;
    self->value.i = value;
}

void json_dom_node_set_uint(JsonDomNode* self, uint64_t value)
{
    free_object(self);
    self->type = JSON_DOM_NODE_TYPE_UINT;
    self->value.u = value;
}

void json_dom_node_set_double(JsonDomNode* self, double value)
{
    free_object(self);
    self->type = JSON_DOM_NODE_TYPE_DOUBLE;
    self->value.d = value;
}

void json_dom_node_set_bool(JsonDomNode* self, bool value)
{
    free_object(self);
    self->type = JSON_DOM_NODE_TYPE_BOOL;
    self->value.b = value;
}

void json_dom_node_set_string(JsonDomNode* self, const char* value) 
{
    free_object(self);
    self->type = JSON_DOM_NODE_TYPE_STRING;
    self->value.str = json_dom_allocator_alloc(alloc, strlen(value));
    strcpy(self->value.str, value);
}

void json_dom_node_set_null(JsonDomNode* self)
{
    free_object(self);
    self->type = JSON_DOM_NODE_TYPE_NULL;
}

unsigned int json_dom_node_set_string_escaped(JsonDomNode* self, const char* value)
{
    free_object(self);
    self->type = JSON_DOM_NODE_TYPE_STRING;
    self->value.str = json_dom_allocator_alloc(alloc, strlen(value+1));
    unsigned int len = 0;
    while((*value != '\0') && (*value != '"')) {
        char c = json_string_unescape_next(&value);
        self->value.str[len++] = c;
    }
    self->value.str[len] = '\0';
    return len;
}

JsonDomNodeType json_dom_node_get_type(const JsonDomNode* self)
{
    return self->type;
}

int json_dom_node_get_int(const JsonDomNode* self)
{
    return self->value.i;
}

uint64_t json_dom_node_get_uint(const JsonDomNode* self)
{
    return self->value.u;
}

bool json_dom_node_get_bool(const JsonDomNode* self)
{
    return self->value.b;
}

double json_dom_node_get_double(const JsonDomNode* self)
{
    return self->value.d;
}

const char* json_dom_node_get_string(const JsonDomNode* self)
{
    return self->value.str;
}


static void json_dom_node_append_member(JsonDomNode* self, JsonDomMemberNode* end, JsonDomMemberNode* member) 
{
    member->prev = end;
    member->next = 0;

    if(end) {
        end->next = member;
    }
    else {
        self->value.head.first = member;
    }
}

void json_dom_node_set_int_member(JsonDomNode* self, JsonDomKey key, int value)
{
    JsonDomMemberNode* iter = self->value.head.first;
    JsonDomMemberNode* end_iter = 0;
    while(iter != 0) {
        if(compare_keys(iter->key, key)) {
            json_dom_node_set_int(&iter->node, value);
            return;
        }

        end_iter = iter;
        iter = iter->next;
    }

    JsonDomMemberNode* membernode  = json_dom_allocator_alloc_chunk(alloc);
    membernode->key = key;
    json_dom_node_set_int(&membernode->node, value);

    json_dom_node_append_member(self, end_iter, membernode); 

    self->value.head.length++;
}

int json_dom_node_get_int_member(const JsonDomNode* self, JsonDomKey key)
{
    JsonDomMemberNode* iter = self->value.head.first;
    while(iter != 0) {
        if(compare_keys(iter->key, key)) {
            return iter->node.value.i;
        }
        iter = iter->next;
    }
    return 0;
}

void json_dom_node_set_uint_member(JsonDomNode* self, JsonDomKey key, uint64_t value)
{
    JsonDomMemberNode* iter = self->value.head.first;
    JsonDomMemberNode* end_iter = 0;
    while(iter != 0) {
        if(compare_keys(iter->key, key)) {
            json_dom_node_set_uint(&iter->node, value);
            return;
        }

        end_iter = iter;
        iter = iter->next;
    }

    JsonDomMemberNode* membernode  = json_dom_allocator_alloc_chunk(alloc);
    membernode->key = key;
    json_dom_node_set_uint(&membernode->node, value);

    json_dom_node_append_member(self, end_iter, membernode); 

    self->value.head.length++;
}

uint64_t json_dom_node_get_uint_member(const JsonDomNode* self, JsonDomKey key)
{
    JsonDomMemberNode* iter = self->value.head.first;
    while(iter != 0) {
        if(compare_keys(iter->key, key)) {
            return iter->node.value.u;
        }
        iter = iter->next;
    }
    return 0;
}

void json_dom_node_set_double_member(JsonDomNode* self, JsonDomKey key, double value)
{
    JsonDomMemberNode* iter = self->value.head.first;
    JsonDomMemberNode* end_iter = 0;
    while(iter != 0) {
        if(compare_keys(iter->key, key)) {
            json_dom_node_set_double(&iter->node, value);
            return;
        }

        end_iter = iter;
        iter = iter->next;
    }

    JsonDomMemberNode* membernode  = json_dom_allocator_alloc_chunk(alloc);
    membernode->key = key;
    json_dom_node_set_double(&membernode->node, value);

    json_dom_node_append_member(self, end_iter, membernode); 

    self->value.head.length++;
}

double json_dom_node_get_double_member(const JsonDomNode* self, JsonDomKey key)
{
    JsonDomMemberNode* iter = self->value.head.first;
    while(iter != 0) {
        if(compare_keys(iter->key, key)) {
            return iter->node.value.d;
        }
        iter = iter->next;
    }
    return 0;
}


void json_dom_node_set_bool_member(JsonDomNode* self, JsonDomKey key, bool value)
{
    JsonDomMemberNode* iter = self->value.head.first;
    JsonDomMemberNode* end_iter = 0;
    while(iter != 0) {
        if(compare_keys(iter->key, key)) {
            json_dom_node_set_bool(&iter->node, value);
            return;
        }

        end_iter = iter;
        iter = iter->next;
    }

    JsonDomMemberNode* membernode  = json_dom_allocator_alloc_chunk(alloc);
    membernode->key = key;
    json_dom_node_set_bool(&membernode->node, value);

    json_dom_node_append_member(self, end_iter, membernode); 

    self->value.head.length++;
}

bool json_dom_node_get_bool_member(const JsonDomNode* self, JsonDomKey key)
{
    JsonDomMemberNode* iter = self->value.head.first;
    while(iter != 0) {
        if(compare_keys(iter->key, key)) {
            return iter->node.value.b;
        }
        iter = iter->next;
    }
    return false;
}

void json_dom_node_set_string_member(JsonDomNode* self, JsonDomKey key, const char* value)
{
    JsonDomMemberNode* iter = self->value.head.first;
    JsonDomMemberNode* end_iter = 0;
    while(iter != 0) {
        if(compare_keys(iter->key, key)) {
            json_dom_node_set_string(&iter->node, value);
            return;
        }

        end_iter = iter;
        iter = iter->next;
    }

    JsonDomMemberNode* membernode  = json_dom_allocator_alloc_chunk(alloc);
    membernode->key = key;
    json_dom_node_set_string(&membernode->node, value);

    json_dom_node_append_member(self, end_iter, membernode); 

    self->value.head.length++;
}


unsigned int json_dom_node_set_string_member_escaped(JsonDomNode* self, JsonDomKey key, const char* value)
{
    unsigned int len = 0;
    JsonDomMemberNode* iter = self->value.head.first;
    JsonDomMemberNode* end_iter = 0;
    while(iter != 0) {
        if(compare_keys(iter->key, key)) {
            return json_dom_node_set_string_escaped(&iter->node, value);
        }

        end_iter = iter;
        iter = iter->next;
    }

    JsonDomMemberNode* membernode  = json_dom_allocator_alloc_chunk(alloc);
    membernode->key = key;
    len = json_dom_node_set_string_escaped(&membernode->node, value);

    json_dom_node_append_member(self, end_iter, membernode); 

    self->value.head.length++;
    return len;
}

const char* json_dom_node_get_string_member(const JsonDomNode* self, JsonDomKey key)
{
    JsonDomMemberNode* iter = self->value.head.first;
    while(iter != 0) {
        if(compare_keys(iter->key, key)) {
            return iter->node.value.str;
        }
        iter = iter->next;
    }
    return false;
}

const JsonDomNode* json_dom_node_get_member(const JsonDomNode* self, JsonDomKey key) 
{
    JsonDomMemberNode* iter = self->value.head.first;
    while(iter != 0) {
        if(compare_keys(iter->key, key)) {
            return &iter->node;
        }
        iter = iter->next;
    }
    return 0;
}

bool json_dom_node_has_member(const JsonDomNode* self, JsonDomKey key)
{
    JsonDomMemberNode* iter = self->value.head.first;
    while(iter != 0) {
        if(compare_keys(iter->key, key)) {
            return true;
        }
        iter = iter->next;
    }
    return false;
}

void json_dom_node_set_null_member(JsonDomNode* self, JsonDomKey key) 
{
    unsigned int len = 0;
    JsonDomMemberNode* iter = self->value.head.first;
    JsonDomMemberNode* end_iter = 0;
    while(iter != 0) {
        if(compare_keys(iter->key, key)) {
            json_dom_node_set_null(&iter->node);
            return;
        }

        end_iter = iter;
        iter = iter->next;
    }

    JsonDomMemberNode* membernode  = json_dom_allocator_alloc_chunk(alloc);
    membernode->key = key;
    json_dom_node_set_null(&membernode->node);

    json_dom_node_append_member(self, end_iter, membernode); 
    self->value.head.length++;
}

void json_dom_node_set_member(JsonDomNode* self, JsonDomKey key, JsonDomNode* other)
{
    JsonDomMemberNode* iter = self->value.head.first;
    JsonDomMemberNode* end_iter = 0;
    while(iter != 0) {
        if(compare_keys(iter->key, key)) {
            json_dom_node_set_object(&iter->node);
            iter->node.type = other->type;
            iter->node.value = other->value;
            other->value.head.first = 0;
            other->type = JSON_DOM_NODE_TYPE_NULL;
            return;
        }

        end_iter = iter;
        iter = iter->next;
    }

    JsonDomMemberNode* membernode  = json_dom_allocator_alloc_chunk(alloc);
    membernode->key = key;
    membernode->node.type = other->type;
    membernode->node.value = other->value;
    other->value.head.first = 0;
    other->type = JSON_DOM_NODE_TYPE_NULL;

    json_dom_node_append_member(self, end_iter, membernode); 
    self->value.head.length++;
}

void json_dom_node_push_back(JsonDomNode* self, JsonDomNode* other) 
{
    JsonDomValueNode* vn = json_dom_allocator_alloc_chunk(alloc);
    //json_dom_node_set_object(&vn->node);
    vn->node.type = other->type;
    vn->node.value = other->value;
    other->value.head.first = 0;
    other->value.head.last = 0;
    other->type = JSON_DOM_NODE_TYPE_NULL;

    JsonDomValueNode* iter = self->value.head.last;

    vn->prev = iter;
    vn->next = 0;

    if(iter) {
        iter->next = vn;
    }

    if(self->value.head.first == 0) {
        self->value.head.first = vn;
    }

    self->value.head.last = vn;
    self->value.head.length++;
}

void json_dom_node_pop_back(JsonDomNode* self)
{
    if(self->value.head.last != 0) {
        JsonDomValueNode* vn = self->value.head.last;
        if(vn->prev) {
            ((JsonDomValueNode*)(vn->prev))->next = 0;
        }

        free_object(&vn->node);
        json_dom_allocator_free(alloc, vn);
        self->value.head.length--;
    }
}

void json_dom_node_push_front(JsonDomNode* self, JsonDomNode* other) 
{
    JsonDomValueNode* vn = json_dom_allocator_alloc_chunk(alloc);
    json_dom_node_set_object(&vn->node);
    vn->node.type = other->type;
    vn->node.value = other->value;
    other->value.head.first = 0;
    other->type = JSON_DOM_NODE_TYPE_NULL;

    JsonDomValueNode* iter = self->value.head.first;

    vn->next = iter;
    vn->prev = 0;

    if(iter) {
        iter->prev = vn;
    }

    self->value.head.first = vn;
    if(self->value.head.last == 0) {
        self->value.head.last = vn;
    }
    self->value.head.length++;
}


const JsonDomNode* json_dom_node_get_index(const JsonDomNode* self, unsigned int idx)
{
    JsonDomValueNode* iter = self->value.head.first;
    while(iter) {
        if(idx == 0) {
            return &iter->node;
        }
        idx--;
        iter = iter->next;
    }
    return 0;
}

char* json_dom_node_stringify(const JsonDomNode* self, JsonStringBuilder* builder)
{
    switch(self->type) {
        case JSON_DOM_NODE_TYPE_OBJECT:
            {
                json_string_builder_append(builder, "{\"");
                JsonDomMemberNode* iter = self->value.head.first;
                while(iter != 0) {
                    json_string_builder_append_escape(builder, iter->key.value);
                    json_string_builder_append(builder, "\":");
                    json_dom_node_stringify(&iter->node, builder);
                    iter = iter->next;
                    if(iter) {
                        json_string_builder_append(builder, ",\"");
                    }
                }           
                json_string_builder_append(builder, "}");
            }
            break;
        case JSON_DOM_NODE_TYPE_ARRAY:
            {
                json_string_builder_append(builder, "[");
                JsonDomValueNode* iter = self->value.head.first;
                while(iter) {
                    json_dom_node_stringify(&iter->node, builder);
                    iter = iter->next;
                    if(iter) {
                        json_string_builder_append(builder, ",");
                    }
                }
                json_string_builder_append(builder, "]");
            }
            break;
        case JSON_DOM_NODE_TYPE_STRING:
            {
                json_string_builder_append(builder, "\"");
                json_string_builder_append_escape(builder, self->value.str);
                json_string_builder_append(builder, "\"");
            }
            break;
        case JSON_DOM_NODE_TYPE_INT:
            {
                json_string_builder_append_int(builder, self->value.i);
            }
            break;
        case JSON_DOM_NODE_TYPE_UINT:
            {
                json_string_builder_append_uint(builder, self->value.u);
            }
            break;
        case JSON_DOM_NODE_TYPE_DOUBLE:
            {
                json_string_builder_append_double(builder, self->value.d);
            }
            break;
        case JSON_DOM_NODE_TYPE_BOOL:
            {
                json_string_builder_append_bool(builder, self->value.b);
            }
            break;
        case JSON_DOM_NODE_TYPE_NULL:
            {
                json_string_builder_append_null(builder);
            }
            break;
    } 
    return json_string_builder_get(builder);
}

