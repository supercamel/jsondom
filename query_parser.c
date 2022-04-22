#include "query_parser.h"
#include "query_instructions.h"
#include "parser.h"

const char* json_dom_query_parse(const char* query, const char** error)
{
    JsonDomNode* query_node = json_dom_parse(query);
    if(query_node == 0) {
        if(*error) {
            strcpy(*error, "Query parse error!");
        }
        return 0;
    }

    if(json_dom_node_get_type(query_node) != JSON_DOM_NODE_TYPE_OBJECT) {
        if(*error) {
            strcpy(*error, "Query parse error. Not an object.");
        }
        return 0;
    }

    unsigned int length = json_dom_node_length(query_node);
    if(length > 1) {
        if(*error) {
            strcpy(*error, "Query root node has more than 1 entry.");
        }
        return 0;
    }

    JsonStringBuilder builder = json_string_builder_new();
    if(length == 0) {
        json_string_builder_append_char(&builder, QI_SPLAT);
        json_string_builder_append_char(&builder, QI_NULL); 
        return json_string_builder_get(&builder);
    }
}


