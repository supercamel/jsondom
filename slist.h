#ifndef JSON_DOM_SLIST_H
#define JSON_DOM_SLIST_H

#include "dom_node.h"

#define SLIST_MAX_LEVELS 5
#define SLIST_P 0.18


/**
 * Skip List
 *
 * This is an indexed double linked skip list.
 *
 * Skip lists are like linked lists only they have multiple links to other nodes. 
 * The additional links skip over nodes which enables the list to be traversed quickly. 
 *
 * This implementation is indexed, meaning the number of nodes that each hop skips is known and this makes
 * it possible to quickly skip to a node by its index (like an array). Though much faster than a linked list, 
 * there is still some overhead. 
 *
 * Skip lists can be ordered or unordered. Ordered skip lists are quite fast to add and delete data, 
 * fast to search and extremely fast to traverse a range of data. 
 *
 * The following functions are for ordered lists
 *
    JsonDomNode* node = json_dom_parse("{\"idx\":40}");
    JsonDomKey key = json_dom_node_key_new("idx");

    json_dom_slist_insert(slist, node, compare, &key);

 * json_dom_slist_insert - inserts data into an ordered list
 * json_dom_slist_find_first - finds the first instance of some data
 * json_dom_slist_find_last - finds the last instance of some data
 * json_dom_slist_delete_first_instance - deletes the first instance of something
 *
 * Unordered skip lists are fast to append data to and alright for accessing by index, though lots of 
 * index access can add up quickly. 
 *
 * json_dom_slist_append - adds a node to the end of the list. will break ordering (if that matters)
 * json_dom_slist_get_index - gets a node by index
 * json_dom_slist_delete_index - deletes a node by index
 *
 * DO NOT MIX AND MATCH ORDERED AND UNORDERED FUNCTIONS. Pick one. 
 */

typedef struct _JsonDomSListNode JsonDomSListNode;

/**
 * Skip list node
 */
typedef struct _JsonDomSListNode {
    //JsonDomNode* node;
    void* payload;
    JsonDomSListNode* next[SLIST_MAX_LEVELS+1];
    JsonDomSListNode* prev[SLIST_MAX_LEVELS+1];
    int width[SLIST_MAX_LEVELS+1];
} JsonDomSListNode;


/**
 * Skip list struct
 */
typedef struct _JsonDomSList {
    JsonDomSListNode* head;
    JsonDomSListNode* tail;
    unsigned int length;
} JsonDomSList;


/**
 * json_dom_slist_node_next: gets the next node in the list
 * Returns: (transfer none): the next node
 */
const JsonDomSListNode* json_dom_slist_node_next(JsonDomSListNode* self);

/**
 * json_dom_slist_node_prev: gets the previous node in the list
 * Returns: (transfer none): the prior node
 */
const JsonDomSListNode* json_dom_slist_node_prev(JsonDomSListNode* self);

/**
 * json_dom_slist_new: create a new skip list
 * Returns: (transfer full): A new skip list
 */
JsonDomSList* json_dom_slist_new();

/**
 * json_dom_slist_insert: inserts a new element into an ordered skip list. 
 * @payload: a pointer to the object to insert
 * @compare: a function to compare the skiplist values
 * @usrptr: a pointer to some user data that is passed to the compare function
 */
void json_dom_slist_insert(JsonDomSList* self, void* payload, int (*compare)(void*, void*, void*), void* usrptr);

/**
 * json_dom_slist_append: appends an element to the end of an unordered skip list
 * @payload: a pointer to the object to insert
 */
void json_dom_slist_append(JsonDomSList* self, void* payload);

/**
 * json_dom_slist_begin: returns the first node
 * Returns: (transfer none): the first node of the skip list
 */
const JsonDomSListNode* json_dom_slist_begin(JsonDomSList* self);

/**
 * json_dom_slist_end: returns the tail node which is one node past the last node with content
 * Returns: (transfer none): the tail node of the skip list
 */
const JsonDomSListNode* json_dom_slist_end(JsonDomSList* self);

/**
 * json_dom_slist_get_index: gets node payload pointer by index, like an array. 
 * Returns: (transfer none): the node data
 */
void* json_dom_slist_get_index(JsonDomSList* self, int pos);

/**
 * json_dom_slist_find_first: finds the first instance of the 'data' in the list. must be an ordered list.
 * @data: the data to find the first instance of
 * @compare: a compare function to determine which payload data is bigger than the other
 * @usrptr: a pointer to user data that is passed to the compare function. can be NULL
 * Returns: (transfer none): the node containing the first instance, or 0
 */
const JsonDomSListNode* json_dom_slist_find_first(
        JsonDomSList* self, void* data, int (*compare)(void*, void*, void*), void* usrptr);

/**
 * json_dom_slist_find_last: finds the last instance of the 'data' in the list. must be an ordered list.
 * @data: the data to find the last instance of
 * @compare: a compare function to determine which payload data is bigger than the other
 * @usrptr: a pointer to user data that is passed to the compare function. can be NULL
 * Returns: (transfer none): the node containing the first instance or 0
 */
const JsonDomSListNode* json_dom_slist_find_last(
        JsonDomSList* self, void* data, int (*compare)(void*, void*, void*), void* usrptr);

/**
 * json_dom_slist_delete_first_instance: finds and deletes the first instance of some data. 
 *      the list must be ordered
 * @data: the data to find and delete
 * @compare: the compare function
 * @usrptr: user data to pass to the compare function
 */
void json_dom_slist_delete_first_instance(
        JsonDomSList* self, void* data, int (*compare)(void*, void*, void*), void* usrptr);

/**
 * json_dom_slist_delete_index: deletes a node by index number
 * @pos: the position of the node to delete
 */
void json_dom_slist_delete_index(JsonDomSList* self, int pos);


#endif


