#include "slist.h"
#include <stdio.h>
#include <stdlib.h>


static int random_level() {
    float r = (float)rand()/RAND_MAX;
    int lvl = 0;
    while (r < SLIST_P && lvl < SLIST_MAX_LEVELS)
    {
        lvl++;
        r = (float)rand()/RAND_MAX;
    }
    return lvl;
}


const JsonDomSListNode* json_dom_slist_node_next(const JsonDomSListNode* self) 
{
    return self->next[0];
}

JsonDomSListNode* json_dom_slist_node_new(JsonDomAllocator* alloc, void* payload)
{
    JsonDomSListNode* node = json_dom_allocator_alloc_chunk(alloc);
    node->payload = payload;

    for(int i = 0; i < SLIST_MAX_LEVELS+1; i++) {
        node->next[i] = 0;
    }
    return node;
}

JsonDomSList* json_dom_slist_new()
{
    JsonDomAllocator* alloc = json_dom_allocator_new(512, 3);
    JsonDomSList* slist = json_dom_allocator_alloc(alloc, sizeof(JsonDomSList));
    slist->alloc = alloc;
    slist->head = json_dom_slist_node_new(alloc, 0);

    for(int i = 0; i < SLIST_MAX_LEVELS+1; i++) {
        slist->head->next[i] = 0;
    }

    slist->length = 0;
    return slist;
}


void json_dom_slist_insert(JsonDomSList* self, void* payload, int (*compare)(void*, void*, void*), void* usrptr)
{
    // create the new node
    int rlevel = random_level();

    JsonDomSListNode* node = json_dom_slist_node_new(self->alloc, payload);

    // figure out where to insert it
    JsonDomSListNode* iter = self->head;

    for(int i = 0; i < SLIST_MAX_LEVELS+1; i++) {
        node->next[i] = 0;
    }

    JsonDomSListNode* update[SLIST_MAX_LEVELS+1];

    for(int level = SLIST_MAX_LEVELS; level >= 0; level--)
    {
        // if the new payload is less than the next
        while((iter->next[level] != 0) && 
                (compare(payload, iter->next[level]->payload, usrptr) < 0)) {
            // skip along
            iter = iter->next[level];
            //printf("%i ", level);
        }
        update[level] = iter;
    }
    //printf("\n");

    for(int i = 0; i <= rlevel; i++) {
        node->next[i] = update[i]->next[i];
        update[i]->next[i] = node;
    }

    self->length++;
}


const JsonDomSListNode* 
json_dom_slist_find_first(JsonDomSList* self, void* data, int (*compare)(void*, void*, void*), void* usrptr) {

    // find the first instance of the value
    JsonDomSListNode* iter = self->head;
    int level = SLIST_MAX_LEVELS-1;

    for(int level = SLIST_MAX_LEVELS-1; level >= 0; level--)
    {
        // if the new payload is less than the next
        while(iter->next[level]->payload && (compare(data, iter->next[level]->payload, usrptr) < 0)) {
            // skip along
            iter = iter->next[level];
        }
    }

    return iter->next[0];
}

const JsonDomSListNode* 
json_dom_slist_find_last(JsonDomSList* self, void* data, int (*compare)(void*, void*, void*), void* usrptr) {

    // find the first instance of the value
    JsonDomSListNode* iter = self->head;
    int level = SLIST_MAX_LEVELS-1;

    for(int level = SLIST_MAX_LEVELS-1; level >= 0; level--)
    {
        // if the new payload is less than the next
        while(iter->next[level]->payload && (compare(data, iter->next[level]->payload, usrptr) <= 0)) {
            // skip along
            iter = iter->next[level];
        }
    }

    return iter;
}

const JsonDomSListNode* json_dom_slist_begin(JsonDomSList* self)
{
    return self->head->next[0];
}

void json_dom_slist_delete_first_instance(
        JsonDomSList* self, void* data, int (*compare)(void*, void*, void*), void* usrptr)
{
    JsonDomSListNode* iter = self->head;
    int level = SLIST_MAX_LEVELS-1;

    for(int level = SLIST_MAX_LEVELS-1; level >= 0; level--)
    {
        if(iter->next[level]->payload) {
            // if the data is less than the next
            int cmp = compare(data, iter->next[level]->payload, usrptr);
            while(cmp < 0) {
                // skip along
                iter = iter->next[level];
                cmp = compare(data, iter->next[level]->payload, usrptr);
            }

            // this iter skips over the node, so reduce the length since the node is getting beamed
            if(cmp == 0) {
                JsonDomSListNode* nextnext = iter->next[level]->next[level];
                iter->next[level] = nextnext;
            }
        }
    }
}



