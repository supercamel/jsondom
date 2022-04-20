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


static void print_levels(const JsonDomSList* self) {
    for(int i = 0; i < SLIST_MAX_LEVELS; i++) {
        printf("Level %i\n", i);
        JsonDomSListNode* iter = self->head;
        while(iter != self->tail) {
            printf("%i ", iter->width[i]);
            iter = iter->next[i];
        } 
        printf("\n");
    }
}


const JsonDomSListNode* json_dom_slist_node_next(const JsonDomSListNode* self) 
{
    return self->next[0];
}

const JsonDomSListNode* json_dom_slist_node_prev(const JsonDomSListNode* self)
{
    return self->prev[0];
}

JsonDomSListNode* json_dom_slist_node_new(JsonDomAllocator* alloc, void* payload)
{
    JsonDomSListNode* node = json_dom_allocator_alloc_chunk(alloc);
    /*
    node->next = json_dom_allocator_alloc_chunk(alloc);
    node->prev = json_dom_allocator_alloc_chunk(alloc);
    node->width = json_dom_allocator_alloc_chunk(alloc);
    */
    node->payload = payload;

    for(int i = 0; i < SLIST_MAX_LEVELS+1; i++) {
        node->next[i] = 0;
        node->prev[i] = 0;
        node->width[i] = 0;
    }
    return node;
}

JsonDomSList* json_dom_slist_new()
{
    JsonDomAllocator* alloc = json_dom_allocator_new(512, 3);
    JsonDomSList* slist = json_dom_allocator_alloc(alloc, sizeof(JsonDomSList));
    slist->alloc = alloc;
    slist->head = json_dom_slist_node_new(alloc, 0);
    slist->tail = json_dom_slist_node_new(alloc, 0);

    for(int i = 0; i < SLIST_MAX_LEVELS+1; i++) {
        slist->head->next[i] = slist->tail;
        slist->tail->prev[i] = slist->head;

        slist->tail->width[i] = 0;
        slist->head->width[i] = 0;
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
        node->next[i] = self->tail;
        node->prev[i] = self->head;
    }

    JsonDomSListNode* update[SLIST_MAX_LEVELS+1];

    for(int level = SLIST_MAX_LEVELS; level >= 0; level--)
    {
        // if the new payload is less than the next
        while((iter->next[level] != self->tail) && 
                (compare(payload, iter->next[level]->payload, usrptr) < 0)) {
            // skip along
            iter = iter->next[level];
            //printf("%i ", level);
        }
        iter->width[level]++;
        update[level] = iter;
    }
    //printf("\n");

    for(int i = 0; i <= rlevel; i++) {
        update[i]->prev[i] = node;
        node->next[i] = update[i]->next[i];
        update[i]->next[i] = node;
        node->prev[i] = update[i];
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




void json_dom_slist_append(JsonDomSList* self, void* payload)
{
    int rlevel = random_level();

    JsonDomSListNode* node = json_dom_slist_node_new(self->alloc, payload);

    for(int i = 0; i <= rlevel; i++) {
        self->tail->prev[i]->next[i] = node;
        node->prev[i] = self->tail->prev[i];
        self->tail->prev[i] = node;
        node->next[i] = self->tail;
    }

    for(int i = 0; i < SLIST_MAX_LEVELS; i++) {
        self->tail->prev[i]->width[i]++;
    }

    self->length++;
}

const JsonDomSListNode* json_dom_slist_begin(JsonDomSList* self)
{
    return self->head->next[0];
}

const JsonDomSListNode* json_dom_slist_end(JsonDomSList* self)
{
    return self->tail;
}

void* json_dom_slist_get_index(JsonDomSList* self, int idx)
{
    int level = SLIST_MAX_LEVELS-1;
    JsonDomSListNode* iter = self->head;
    int remaining = idx+1;

    while(remaining) {
        int width = iter->width[level];
        while(width > remaining) {
            level--;
            width = iter->width[level];    
        }

        remaining -= width;
        iter = iter->next[level];
    }

    return iter->payload;
}

void json_dom_slist_delete_index(JsonDomSList* self, int idx) 
{
    int level = SLIST_MAX_LEVELS-1;
    JsonDomSListNode* iter = self->head;
    int counter = 0;

    while(level >= 0) {
        int width = iter->width[level];

        if((counter+width) < idx) {
            counter += width;
            iter = iter->next[level];
        }
        else {
            iter->width[level]--; 

            if(counter+width == idx) {
                iter->width[level] += iter->next[level]->width[level];
                JsonDomSListNode* nextnext = iter->next[level]->next[level];
                iter->next[level] = nextnext;
                nextnext->prev[level] = iter;
            }

            level--;
        }
    }

    self->length--;
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
            iter->width[level]--;
            if(cmp == 0) {
                iter->width[level] += iter->next[level]->width[level];
                JsonDomSListNode* nextnext = iter->next[level]->next[level];
                iter->next[level] = nextnext;
                nextnext->prev[level] = iter;
            }
        }
    }
}




