#include <stdio.h>
#include <string.h>
#include "allocator.h"
#include "dom_node.h"
#include "stringbuilder.h"
#include "parser.h"
#include "slist.h"
#include "collection.h"

#include <time.h>

int main() {
    json_dom_node_initialise();

    JsonDomCollection* collection = json_dom_collection_new("test");

    clock_t start, end;
    start = clock();

    char json[128];
    for(int i = 0; i < 1000000; i++) {
        sprintf(json, "{\"_id\":\"%i\"}", i);
        json_dom_collection_insert_str(collection, json);
        printf("%i\n", i);
    }
    end = clock();
    double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC; 
    printf("%f\n", cpu_time_used);


    //json_dom_collection_print(collection);
}



/*
   int main() {
   char* mystr = "100";
   char* endptr;

   double result = 0;

   clock_t start, end;

   start = clock();


// ~0.497s
for(int i = 0; i < 10000000; i++) {
result = cheap_strdof(mystr, &endptr);

//result = strtod(mystr, &endptr);
if((result < 99) || (result > 101)) {
printf("failed. %f\n", result);
}
}
end = clock();
double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC; 
printf("%f\n", cpu_time_used);


}
*/


/*
   int main() {

   clock_t start, end;

   start = clock();

   JsonStringBuilder* strbuilder = json_string_builder_new();

   for(int i = 0; i < 1000000; i++) {

   json_string_builder_append(strbuilder, "{\"name\":");
   json_string_builder_append(strbuilder, "\"dindu\",\"age\":");
   json_string_builder_append_int(strbuilder, 123);

   json_string_builder_append(strbuilder, ",\"is_gay\":");
   json_string_builder_append_bool(strbuilder, true);
   json_string_builder_append(strbuilder, "}\n");

//printf("%s\n", json_string_builder_get(strbuilder));
}

json_string_builder_delete(strbuilder);

end = clock();
double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC; 
printf("%f\n", cpu_time_used);

return 0;
}


/*
int main() {
JsonDomAllocatorPage* page = json_dom_allocator_page_new(1024);

void* ptrs[1024];

while(1) {
int n_allocs = rand()%1024;

for(int i = 0; i < n_allocs; i++) {
void* ptr = json_dom_allocator_page_alloc(page);
memset(ptr, 0, 64);
ptrs[i] = ptr;
}

for(int i = 0; i < n_allocs; i++) {
json_dom_allocator_page_free(page, ptrs[i]);
}
}
}

*/

/*
   int compare(void* lh, void* rh, void* usrptr) {
   JsonDomKey* idxkey = usrptr;

   JsonDomNode* lhnode = json_dom_node_get_member(lh, *idxkey);
   JsonDomNode* rhnode = json_dom_node_get_member(rh, *idxkey);

   JsonDomNodeType lhtype = json_dom_node_get_type(lhnode);
   JsonDomNodeType rhtype = json_dom_node_get_type(rhnode);


   if(rhtype != lhtype) {
   if(rhtype < lhtype) {
   return 1;
   }
   return -1;
   }


   switch(lhtype) {
   case JSON_DOM_NODE_TYPE_INT:
   {
   int lhp = json_dom_node_get_int_member(lh, *idxkey);
   int rhp = json_dom_node_get_int_member(rh, *idxkey);

   if(lhp == rhp) {
   return 0;
   }
   else if(lhp > rhp) {
   return -1;
   }
   else {
   return 1;
   }
   }
   break;
   case JSON_DOM_NODE_TYPE_STRING:
   {
   return strcmp(json_dom_node_get_string_member(lh, *idxkey),
   json_dom_node_get_string_member(rh, *idxkey));
   }
   break;
   }
   return 0;
   }

   int main() {
   clock_t start, end;

   json_dom_node_initialise();

   JsonDomSList* slist = json_dom_slist_new();

   start = clock();
   JsonDomNode* arr = json_dom_node_new();
   JsonDomNode* str = json_dom_node_new();
   JsonDomNode* dnode = json_dom_node_new();

   JsonDomKey idxkey = json_dom_node_key_new("idx");
   for(int i = 0; i < 10; i++) {
   JsonDomKey arrkey = json_dom_node_key_new("arr");

   JsonDomNode* node = json_dom_node_new();
   json_dom_node_set_object(node);

   json_dom_node_set_int_member(node, idxkey, i);

   json_dom_node_set_array(arr);

   json_dom_node_set_string(str, "Hello world!");
   json_dom_node_set_double(dnode, 153.12);

json_dom_node_push_back(arr, str);
json_dom_node_push_back(arr, dnode);

json_dom_node_set_member(node, arrkey, arr);

json_dom_slist_append(slist, node);
}

end = clock();
double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC; 
printf("%f\n", cpu_time_used);

JsonDomSListNode* iter = json_dom_slist_begin(slist);
while(iter != json_dom_slist_end(slist)) {
    JsonDomNode* n = iter->payload;
    printf("%i ", json_dom_node_get_int_member(n, idxkey));
    iter = json_dom_slist_node_next(iter);
}
printf("\n");


JsonDomKey key = json_dom_node_key_new("idx");
JsonDomNode* node = json_dom_parse("{\"idx\":4}");
json_dom_slist_delete_first_instance(slist, node, compare, &key);

node = json_dom_parse("{\"idx\":40}");

json_dom_slist_insert(slist, node, compare, &key);

iter = json_dom_slist_begin(slist);
while(iter != json_dom_slist_end(slist)) {
    JsonDomNode* n = iter->payload;
    printf("%i ", json_dom_node_get_int_member(n, idxkey));
    iter = json_dom_slist_node_next(iter);
}
printf("\n");


JsonDomSListNode* first = json_dom_slist_find_last(slist, node, compare, &key);
first = first->prev[0];
JsonDomNode* firstnode = (JsonDomNode*)first->payload;

printf("%i\n", json_dom_node_get_int_member(firstnode, key));

start = clock();
//for(int i = 0; i < 1000000; i++) {
//json_dom_slist_get_index(slist, 900000);
//}
end = clock();
cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC; 
printf("%f\n", cpu_time_used);


//}
//json_dom_slist_delete_index(slist, 29);
//json_dom_slist_print_fast(slist);


//printf("%i\n", json_dom_slist_get_index(slist, 97));

/*
   JsonDomNode* node = json_dom_node_new();
   JsonDomNode* arr = json_dom_node_new();
   JsonDomNode* str = json_dom_node_new();
   JsonDomNode* dnode = json_dom_node_new();

   for(int i = 0; i < 1000000; i++) {
   JsonDomKey idxkey = json_dom_node_key_new("idx");
   JsonDomKey arrkey = json_dom_node_key_new("arr");


//const char* str = "{\"key\":5.2,\"arr\":[0, 1, 2,3,4]}";
//const char* str = "{\"key\":5.2,\"arr\":\"[0, 1, 2,3,4]\"}";

//node = json_dom_parse(str);
json_dom_node_set_object(node);

json_dom_node_set_int_member(node, idxkey, 100);

json_dom_node_set_array(arr);

json_dom_node_set_string(str, "Hello world!");
json_dom_node_set_double(dnode, 153.12);

json_dom_node_push_back(arr, str);
json_dom_node_push_back(arr, dnode);

json_dom_node_set_member(node, arrkey, arr);
}

end = clock();
double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC; 

printf("%f\n", cpu_time_used);

JsonStringBuilder builder = json_string_builder_new();
printf("%s\n", json_dom_node_stringify(node, &builder));

char *line = NULL;
size_t len = 0;
ssize_t lineSize = 0;
lineSize = getline(&line, &len, stdin);
free(line);
/*
char buf[256];
unsigned int len;
json_string_unescape(str, buf, &len);
printf("%s %u\n", buf, len);
/*
JsonDomNode* subobj = json_dom_node_new();
json_dom_node_set_array(subobj);

JsonDomNode* element1 = json_dom_node_new();
json_dom_node_set_int(element1, 100);
json_dom_node_push_back(subobj, element1);

json_dom_node_set_double(element1, 12.54234);
json_dom_node_push_back(subobj, element1);

json_dom_node_pop_back(subobj);

JsonStringBuilder* builder = json_string_builder_new();
json_dom_node_stringify(subobj, builder);
printf("%s\n", json_string_builder_get(builder));
json_string_builder_delete(builder);


end = clock();
double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC; 
printf("%f\n", cpu_time_used);
// printf("int member: %i\n", json_dom_node_get_int_member(node, "test"));
//printf("str member: %s\n", json_dom_node_get_string_member(node, "str"));
return 0;
}
*/

