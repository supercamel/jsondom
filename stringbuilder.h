#ifndef JSON_STRING_BUILDER_H
#define JSON_STRING_BUILDER_H

#include <stdbool.h>

typedef struct _JsonStringBuilder {
    char* head;
    char* ptr;
    unsigned int reserved;
    char* canary;
} JsonStringBuilder;



JsonStringBuilder json_string_builder_new();

void json_string_builder_append(JsonStringBuilder* self, const char* str);
void json_string_builder_append_escape(JsonStringBuilder* self, const char* str);
void json_string_builder_append_char(JsonStringBuilder* self, char c);
void json_string_builder_append_int(JsonStringBuilder* self, int i);
void json_string_builder_append_uint(JsonStringBuilder* self, unsigned int i);
void json_string_builder_append_double(JsonStringBuilder* self, double d);
void json_string_builder_append_bool(JsonStringBuilder* self, bool b);
void json_string_builder_append_null(JsonStringBuilder* self);

char* json_string_builder_get(JsonStringBuilder* self);


void json_string_unescape(const char* src, char* dest, unsigned int* limit);
char json_string_unescape_next(const char** src);


#endif


