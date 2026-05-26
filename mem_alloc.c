#include<stdio.h>
#include <unistd.h>
#include "mem_alloc.h"

typedef struct header{
    int size;
    struct header * next;
} header_t;

static header_t base = {0, &base};
static header_t *freeb = &base;
static header_t base_used = {0, NULL};
static header_t *used_list = NULL;
static void add_to_free_list(header_t * block)
{
    header_t *temp = freeb;
    if(freeb == &base)
    {
        #ifdef DEBUG
        printf("freeb and base are same\n");
        #endif
        base.next = block;
         #ifdef DEBUG
        printf("base %p now points to %p\n", &base, base.next);
         #endif
        block->next = &base;
        freeb = block;
        #ifdef DEBUG
        printf("freeb is no %p and points to %p\n", freeb, freeb->next);
        #endif
        return;
    }

    while(temp && !(block > temp && block < temp->next))
    {
        if(temp >= temp->next && (block > temp || block < temp->next))
        {
            break;
        }
        temp = temp->next;
    }
    

    if(block < temp->next)
    {
        if(block + block->size == temp->next)
        {
            block->size += temp->next->size;
            block->next = temp->next->next;
        }
        else
        {
            block->next = temp->next;
        }
        
        if(temp->next == freeb)
        {
            freeb = block;
        }
    }

    if(block > temp)
    {
        if(temp + temp->size == block)
        {
            temp->size += block->size;
            temp->next = block->next;
        }
        else
        {
            temp->next = block;
        }
    }

}

static header_t * more_mem(size_t num_units) {
    #ifdef DEBUG
    printf("IN more mem\n");
    #endif
    void *vp;
    if(num_units < MIN_ALLOCATION_SIZE)
    {
        num_units = MIN_ALLOCATION_SIZE / sizeof(header_t);
    }
    if((vp = sbrk(num_units * sizeof(header_t))) != (void *)(-1))
    {
        header_t * tmp = (header_t *)(vp);
        tmp->size = num_units;
        add_to_free_list(tmp);
        // freeb->next = tmp;
        return freeb;
    }
    return NULL;

}

int remove_from_used_list(header_t *block)
{
    header_t *temp = &base_used;
    header_t *prev = &base_used;
    #ifdef DEBUG
    printf("before the loop\n");
    #endif
    while(temp && temp != block)
    {
        #ifdef DEBUG
        printf("in the loop\n");
        #endif
        prev = temp;
        temp = temp->next;
    }
    #ifdef DEBUG
    printf("out of the loop\n");
    #endif
    if(!temp)
    {
        return 1;
    }
    prev->next = temp->next;
    return 0;
}

void free_mem(void *block)
{
    header_t *block_to_free = (header_t *)(block - 1);
    if(remove_from_used_list(block_to_free))
    {
        #ifdef DEBUG
        printf("failed to remove %p from used list", block_to_free);
        #endif
    }
    add_to_free_list(block_to_free);
}

void * mem_alloc(size_t alloc)
{
    size_t num_units = (alloc + sizeof(header_t) -1)/(sizeof(header_t)) + 1;
    header_t *temp;
    header_t *prev = freeb;
    #ifdef DEBUG
    printf("base is %p\n", &base);
    #endif
    for(temp = freeb->next; ; prev=temp, temp = temp->next)
    {
        #ifdef DEBUG
        printf("freeb->next is at the start is %p\n", temp);
        #endif
        
        if(temp->size >= num_units)
        {
            #ifdef DEBUG
            printf("size of node %d and size of units %zu\n", temp->size, num_units);
            #endif
            if(temp->size == num_units)
            {
                #ifdef DEBUG
                printf("exactly equal\n");
                #endif
                prev->next = temp->next;
                freeb = prev;
            }
            else
            {
                #ifdef DEBUG
                printf("temp size of %p is %d\n", temp, temp->size);
                #endif
                temp->size -= num_units;
                #ifdef DEBUG
                printf("new temp size is %d\n", temp->size);
                #endif
                freeb = temp;
                temp += temp->size;
                temp->size = num_units;
                #ifdef DEBUG
                printf("allocated block size of %p is %d\n", temp, temp->size);
                #endif
            }
            // freeb = prev;
            #ifdef DEBUG
            printf("new freeb size of %p is %d\n", freeb, freeb->size);
            printf("freebe next is %p\n", freeb->next);
            #endif
            temp->next = base_used.next;
            base_used.next = temp;
            return (void *)temp + 1;
        }
        if(temp == freeb)
        {
            #ifdef DEBUG
            printf("%zu\n", num_units);
            #endif
            temp = more_mem(num_units);
            if(temp == NULL)
            {
                return NULL;
            }
        }
        #ifdef DEBUG
        printf("prev is %p\n", prev);
        printf("freeb is %p\n", temp);
        #endif
    }
    return NULL;
}