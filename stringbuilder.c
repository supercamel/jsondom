#include <stdlib.h>
#include <stdio.h>
#include "stringbuilder.h"
#include "dom_node.h"

char* itoa(int value, char* result) {
    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= 10;
        *ptr++ = "9876543210123456789" [9 + (tmp_value - value * 10)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';

    char* ret = ptr;
    ptr--;

    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return ret;
}

char* utoa(unsigned int value, char* result) {
    char* ptr = result, *ptr1 = result, tmp_char;
    unsigned int tmp_value;

    do {
        tmp_value = value;
        value /= 10;
        *ptr++ = "0123456789" [tmp_value - value * 10];
    } while (value);

    char* ret = ptr;
    ptr--;

    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return ret;
}

JsonStringBuilder json_string_builder_new()
{
    JsonStringBuilder self;
    self.head = malloc(128);
    self.ptr = self.head;
    self.reserved= 128;
    self.canary = &self.head[self.reserved-20];
    *self.canary = '\0';
    return self;
}

static inline void check_canary(JsonStringBuilder* self) 
{
    if(self->canary[0] != 0) {
        size_t ptr_dist = self->ptr - self->head;
        self->head = realloc(self->head, self->reserved+128);
        self->ptr = self->head+ptr_dist;
        self->reserved += 128;
        self->canary = &(self->head)[self->reserved-20];
        *self->canary = '\0';
    }
}

void json_string_builder_append(JsonStringBuilder* self, const char* str)
{
    while(*str) {
        while(*str && (!self->canary[0])) {
            *(self->ptr)++ = *str++;
        }

        check_canary(self);
    }
}

void json_string_builder_append_escape(JsonStringBuilder* self, const char* str)
{
    while(*str) {
        while(*str && (!self->canary[0])) {
            switch(*str) {
                case '\"':
                case '\\':
                case '\n':
                case '\t':
                case '\b':
                case '\f':
                case '\r':
                    *(self->ptr)++ = '\\';
            }
            *(self->ptr)++ = *str++;
        }

        check_canary(self);
    }
}


void json_string_builder_append_char(JsonStringBuilder* self, char c)
{
    *(self->ptr)++ = c;
    check_canary(self);
}

void json_string_builder_append_int(JsonStringBuilder* self, int i)
{
    self->ptr = itoa(i, self->ptr);
    check_canary(self);
}

void json_string_builder_append_uint(JsonStringBuilder* self, unsigned int i)
{
    self->ptr = utoa(i, self->ptr);
    check_canary(self);
}

void json_string_builder_append_double(JsonStringBuilder* self, double d)
{
    int n = sprintf(self->ptr, "%f", d);
    self->ptr += n;
    check_canary(self);
}

void json_string_builder_append_bool(JsonStringBuilder* self, bool b) 
{
    if(b) {
        *(self->ptr)++ = 't'; 
        *(self->ptr)++ = 'r'; 
        *(self->ptr)++ = 'u'; 
        *(self->ptr)++ = 'e'; 
    }
    else {
        *(self->ptr)++ = 'f'; 
        *(self->ptr)++ = 'a'; 
        *(self->ptr)++ = 'l'; 
        *(self->ptr)++ = 's'; 
        *(self->ptr)++ = 'e'; 
    }

    check_canary(self);
}

void json_string_builder_append_null(JsonStringBuilder* self)
{
    *(self->ptr)++ = 'n'; 
    *(self->ptr)++ = 'u'; 
    *(self->ptr)++ = 'l'; 
    *(self->ptr)++ = 'l'; 
    check_canary(self);
}


char* json_string_builder_get(JsonStringBuilder* self) 
{
    *(self->ptr+1) = '\0';
    return self->head;
}

/*
   case '\"':
   case '\\':
   case '\n':
   case '\t':
   case '\b':
   case '\f':
   case '\r':
   */


inline char json_string_unescape_next(const char** src)
{
    char ret;
    const char* ptr = *src;
    if(*ptr == '\\') {
        ptr++;

        switch(*ptr) {
            case '\\': 
                ret = '\\';
                break;
            case 'n':
                ret = '\n';
                break;
            case 't':
                ret = '\t';
                break;
            case 'b':
                ret = '\b';
                break;
            case 'f':
                ret = '\f';
                break;
            case 'r':
                ret = '\r';
                break;
            case '"':
                ret = '"';
                break;
            default:
                {
                    printf("Unknown escape sequence \\%c\n", *ptr);
                    ret = *ptr;
                }
        }
    }
    else {
        ret = *ptr;
    }

    ptr++;
    *src = ptr;
    return ret;
}


inline void json_string_unescape(const char* src, char* dest, unsigned int* n)
{
    unsigned int limit = *n;
    *n = 0;
    const char* scanptr = src;

    while((*scanptr != '\0') && (*scanptr != '"')) {
        *dest++ = json_string_unescape_next(&scanptr);

        (*n)++;
        if(*n > limit) {
            break;
        }
    }
    *dest = '\0';
}


