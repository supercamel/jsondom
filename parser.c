#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "parser.h"


typedef struct _ParserNumericResult {
    double d;
    int i;
    unsigned int u;
    int valid;
} ParserNumericResult;




static inline int fast_atoi(char* pstr, char** end)
{
    int res = 0, sign = 1;
    unsigned int diff;
    if(*pstr =='-')
    {
        sign = -1;
        pstr++;

        diff = *pstr - '0';
        if(diff >= 10) {
            *end = pstr;
            return 0;
        }
    }

    diff = *pstr++ - '0';
    while (diff < 10) {
        res = res * 10 + diff;
        diff = *pstr++ - '0';
    }
    *end = pstr-1;
    return res * sign;
}


/**
 * cheap_strtod is cheap because it's much faster than strtod for parsing  doubles within a certain range
 * outside of that, it falls back to using strtod to get a result
 *
 * this is about 8x faster for values less than 1 billion and less than 9 decimal places, and without an exponent
 * i estimate this covers the vast majority of cases
 *
 * but we gotta work on this. there are genuine fast double parses out there.
 */
static inline ParserNumericResult parse_numeric(const char* p, const char** end)
{
    ParserNumericResult pnr;
    pnr.valid = 1;

    const char* t = p;
    int sign = 1;
    unsigned int diff;

    if(*t == '-') {
        sign = -1;
        t++;

        diff = *t - '0';
        if(diff >= 10) {
            *end = p;
            pnr.valid = 0;
            return pnr;
        }
    }

    int lh = 0;
    diff = *t++ - '0';
    while (diff < 10) {
        lh = lh * 10 + diff;
        diff = *t++ - '0';
    }
    t--;
    *end = t;

    if(p == t) {
        *end = p;
        pnr.valid = 0;
        return pnr;
    }
    else if((t - p) > 9) {
        pnr.d = strtod(p, (char**)end);
        pnr.valid = 2;
        return pnr;
    }

    if(*t == '.') {
        pnr.valid = 2;
        t++;
        *end = t;

        double rh  = 0;
        diff = *t++ - '0';
        while (diff < 10) {
            rh  = rh * 10 + diff;
            diff = *t++ - '0';
        }
        t--;

        int len = t - *end;
        if(len > 9) {
            pnr.d = strtod(p, (char**)end);
            return pnr;
        }

        *end = t;

        if((*t == 'e') || (*t == 'E')) {
            pnr.d = strtod(p, (char**)end);
            return pnr;
        }

        double pow = 1;
        for(int i = 0; i < len; i++) {
            pow *= 10; 
        }

        rh /= pow;

        pnr.d = (lh+rh) * sign;
        return pnr;
    }
    else if((*t == 'e') || (*t == 'E')) {
        pnr.d = strtod(p, (char**)end);
        pnr.valid = 2;
        return pnr;
    }
    else {
        pnr.i = lh*sign;
        return pnr;
    }
}


__attribute__((always_inline)) inline bool whitespace(char c) {
    switch(c) {
        case ' ':
        case '\t':
        case '\n':
            return true;
        default:
            return false;
    }
}

__attribute__((always_inline)) inline const char* chug(const char* str) {
    char c = *str;
    while(whitespace(c)) {
        c = *++str;
    }
    return str;
}

int parse_key(const char* str, char** key) {
    JsonStringBuilder builder = json_string_builder_new();
    char next;
    do {
        next = json_string_unescape_next(&str); 
        json_string_builder_append_char(&builder, next);
    } while(next != '"');

    *key = json_string_builder_get(&builder);
    return strlen(*key);
}

JsonDomNode* json_dom_parse_node(const char** pstr) 
{
    const char* str = *pstr;
    JsonDomNode* node = json_dom_node_new();
    switch(*str) {
        case '"':
            {
                str++;
                unsigned int len = json_dom_node_set_string_escaped(node, str);
                str += len;
                if(*str++ != '"') {
                    printf("expected closing quote after string\n");
                    return 0;
                }
            }
            break;
        case '[':
            {
                str++;
                node = json_dom_parse_array(&str);
            }
            break;
        case '{':
            {
                str++;
                node = json_dom_parse_object(&str);
            }
            break;
        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            {

                ParserNumericResult pnr = parse_numeric(str, &str);
                if(pnr.valid == 1) {
                    json_dom_node_set_int(node, pnr.i);
                }
                else if(pnr.valid == 2) {
                    json_dom_node_set_double(node, pnr.d);
                }
            }
            break;
        case 'n':
            {
                if(strncmp(str, "null", 4) == 0) {
                    json_dom_node_set_null(node);
                    str += 4;
                }
                else {
                    printf("unexpected token");
                    return 0;
                }
            }
            break;
        case 't':
            {
                if(strncmp(str, "true", 4) == 0) {
                    json_dom_node_set_bool(node, true);
                    str += 4;
                }
                else {
                    printf("unexpected token");
                    return 0;
                }
            }
            break;
        case 'f':
            {
                if(strncmp(str, "false", 5) == 0) {
                    json_dom_node_set_bool(node, false);
                    str += 5;
                }
                else {
                    printf("unexpected token");
                    return 0;
                }
            }
            break;
    }

    *pstr = str;
    return node;
}


inline JsonDomNode* json_dom_parse_array(const char** pstr)
{
    const char* str = *pstr;
    JsonDomNode* node = json_dom_node_new();
    json_dom_node_set_array(node);
    do {
        str = chug(str);
        JsonDomNode* value = json_dom_parse_node(&str);
        json_dom_node_push_back(node, value);

        str = chug(str);
        if(*str == ',') {
            str++;
            continue;
        }
    } while(*str != ']');
    str++;
    *pstr = str;
    return node;
}

JsonDomNode* json_dom_parse_object(const char** pstr)
{
    const char* str = *pstr;
    JsonDomNode* node = json_dom_node_new();

    json_dom_node_set_object(node);
    str = chug(str);
    while(*str != '}') {
        str = chug(str);
        if(*str != '"') {
            printf("expected \". got %c\n", *str);
            return 0;
        }    
        str++;

        char* key;
        int length = parse_key(str, &key);
        str += length;

        str = chug(str);

        if(*str++ != ':') {
            printf("expected :\n");
            return 0;
        }

        str = chug(str);
        JsonDomNode* member = json_dom_parse_node(&str);
        json_dom_node_set_member(node, key, member);

        str = chug(str);
        if(*str == ',') {
            str++;
            continue;
        }
    } 

    *pstr = str;
    return node;
}

JsonDomNode* json_dom_parse(const char* str) {
    str = chug(str);
    if(*str == '{') {
        str++;
        JsonDomNode* obj = json_dom_parse_object(&str);
        return obj;
    }
    return 0;
}

