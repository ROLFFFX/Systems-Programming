// THIS CODE WAS MY OWN WORK , IT WAS WRITTEN WITHOUT CONSULTING ANY
// SOURCES OUTSIDE OF THOSE APPROVED BY THE INSTRUCTOR . Yuxuan Shi, yshi373, 2599711
//
//  my_malloc.c
//  Lab1: Malloc
//

#include "my_malloc.h"
#include <unistd.h>
// #include <stdio.h>

#define MINIMUMCHUNKSIZE 16
#define MAGICNUM 1018

MyErrorNo my_errno=MYNOERROR;
FreeListNode head = NULL;

uint32_t update_chunk_size(uint32_t size);
void *find_chunk(uint32_t size);
void insert_node(FreeListNode N);
void remove_node(FreeListNode N);
void split_chunk(void* chunk_found, uint32_t size, uint32_t chunk_size);
FreeListNode free_list_begin();

void *my_malloc(uint32_t size){
    if (size == 0){ return NULL; }
    size = update_chunk_size(size);             //update chunk size to include header and padding
    void* new_chunk = find_chunk(size);         //find suitable chunk in memory that can fit in user request
    if (my_errno == MYENOMEM){ return NULL; }   //sbrk() failed cuz heap runs out of memory
    return (char*)new_chunk + 8;                //return address of new_chunk assigned skipping header
}
      
void my_free(void *ptr){
    my_errno = MYNOERROR;
    if (ptr == NULL) {  //check ptr for null before magic num check avoids dereferencing a null ptr
        my_errno = MYBADFREEPTR;
        return;
    }

    //retrieve header content
    uint32_t* free_size = (uint32_t*)((char*)ptr - 8);
    uint32_t* magic_num = (uint32_t*)((char*)ptr - 4);

    if (*magic_num != MAGICNUM){ //handle case: magic number mismatch
        my_errno = MYBADFREEPTR;
        return;
    }

    FreeListNode node_to_be_freed;
    node_to_be_freed = (FreeListNode)((char*)ptr - 8);  //jump ahead of header, get full node
    node_to_be_freed->size = *free_size;
    node_to_be_freed->flink = NULL;
    insert_node(node_to_be_freed);
}

FreeListNode free_list_begin(){
    return head;
}

void coalesce_free_list(){
    FreeListNode free_list_node = head;
    if (free_list_node == NULL){ return; }
    while (free_list_node != NULL) {
        if ((void *)free_list_node + free_list_node->size == free_list_node->flink) {   //if current size + current addr equals to addr for next node, two ndoes are neighbors
            //merging (by skipping over next node)
            free_list_node->size += free_list_node->flink->size;
            free_list_node->flink = free_list_node->flink->flink;
        } else {    //wrapping udpate free_list_node so that it handles case where multiple chunks are all neighbors
            free_list_node = free_list_node->flink;
        }
    }
}

// Helper functions---------------------------------------------
// Return nearest multiple of 8(padding added) + header (8 byte)
uint32_t update_chunk_size(uint32_t size){
    size+=7;
    size-=(size%8);
    return size+8;
}

// Find memory chunk that could fit in 'size'
void* find_chunk(uint32_t size){
    void* chunk_found = NULL;
    FreeListNode free_list_node = free_list_begin();    //pointer to head of free list
    uint32_t chunk_size;                                //bookkeeper which will store size of chunk found

    while(free_list_node != NULL){
        if (size <= free_list_node -> size){            //when current chunk could fit in user request
            chunk_found = free_list_node;               //store chunk address
            chunk_size = free_list_node -> size;        //store chunk size and check later to see if splitting is needed
            remove_node(free_list_node);                //update freelist
            break;
        }
        free_list_node = free_list_node -> flink;
    }

    if (chunk_found == NULL) {          //nothing on free list could fit user reqeust
        void* new_page = NULL;
        if (size <= 8192) {             //traversal failed simply because free list is full: get a new page and update bookkeeper
            new_page = sbrk(8192);          
            chunk_size = 8192;
        } else {                        //traversal failed since it asks for more than 8192
            new_page = sbrk(size);
            chunk_size = size;
        }
        if (new_page==(void*)-1) {      //set error channel. sbrk failed cuz heap runs out of memory
            my_errno = MYENOMEM;        
            return new_page;
        }
        chunk_found = new_page;         //chunk_found and is now a whole new page
    }
    
    //write header file
    *(uint32_t *)chunk_found = chunk_size;   
    *(uint32_t *)((char *)chunk_found + 4) = MAGICNUM;

    //if padding allows for one more chunk.
    if ((chunk_size - size) >= MINIMUMCHUNKSIZE){ 
        split_chunk(chunk_found, size, chunk_size); 
        *(uint32_t*)chunk_found = size;  //rewrite chunk header (size)
    } 
    return chunk_found;
}

// Insert node using first-fit strategy
void insert_node(FreeListNode node_to_be_inserted) {
    FreeListNode free_list_node = free_list_begin();
    FreeListNode previous_node = NULL;
    
    if (free_list_node == NULL) { // If free list is empty, initialize free list with node to be inserted
        head = node_to_be_inserted;
        return;
    }

    while (free_list_node != NULL && free_list_node < node_to_be_inserted) {
        previous_node = free_list_node;
        free_list_node = free_list_node->flink;
    }

    node_to_be_inserted->flink = free_list_node;

    if (previous_node == NULL) { // If inserting at the head of the list
        head = node_to_be_inserted;
    } else {
        previous_node->flink = node_to_be_inserted;
    }
}

// Remove given node from free list
void remove_node(FreeListNode node_to_be_removed){
    FreeListNode free_list_node = free_list_begin();
    if (free_list_node == node_to_be_removed) {     //head is to be removed
        head = free_list_node -> flink;
        return;
    }
    
    while (free_list_node -> flink != NULL) {       //traverse free list, locate node and connect prev and next
        if (free_list_node -> flink == node_to_be_removed) {
            free_list_node -> flink = free_list_node -> flink -> flink;
            return;
        }
        free_list_node = free_list_node -> flink;
    }
}

void split_chunk(void* chunk_found, uint32_t size, uint32_t chunk_size){
    FreeListNode splitted_node = NULL;
    // Initialize for new node and then add new node to free list
    char* splitted_node_start = (char*)chunk_found + size; 
    splitted_node = (FreeListNode)splitted_node_start;
    splitted_node -> size = chunk_size - size;
    splitted_node -> flink = NULL;
    insert_node(splitted_node);
}
