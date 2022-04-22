#ifndef JSON_DOM_QUERY_PARSER_H
#define JSON_DOM_QUERY_PARSER_H

#include <stdbool.h>

/**
 * Parses a query and returns bytecode
 * Returns byte-code used to match documents
 */
const char* json_dom_query_parse(const char* query, const char** error);


/**
 * Executes the bytecode
 */
bool json_dom_query_match(JsonDomNode* document, const char* bytecode);


#endif


