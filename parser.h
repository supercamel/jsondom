#ifndef JSON_DOM_PARSER_H
#define JSON_DOM_PARSER_H

#include "dom_node.h"

const char* json_dom_parse_chug(const char* str);
JsonDomNode* json_dom_parse_array(const char** str);
JsonDomNode* json_dom_parse_object(const char** str);

JsonDomNode* json_dom_parse(const char* str);

#endif


