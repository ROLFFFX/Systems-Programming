//
//  my_malloc-driver.c
//  Lab1: Malloc
//  This file is for testing your code only
//  You will not turn it in
//

#include "my_malloc.h"
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
// Chunk Size Unittesting
// uint32_t update_chunk_size(uint32_t size);
// void check_size_update(){
//     uint32_t size_1 = update_chunk_size(20); //32
//     uint32_t size_2 = update_chunk_size(8);  //16
//     uint32_t size_3 = update_chunk_size(9);  //24
//     uint32_t size_4 = update_chunk_size(108);//120
//     uint32_t size_5 = update_chunk_size(1);  //16

//     printf("test 1: %s\n", size_1==32 ? "Passed" : "Failed");
//     printf("test 2: %s\n", size_2==16 ? "Passed" : "Failed");
//     printf("test 3: %s\n", size_3==24 ? "Passed" : "Failed");
//     printf("test 4: %s\n", size_4==120 ? "Passed" : "Failed");
//     printf("test 5: %s\n", size_5==16 ? "Passed" : "Failed");

// }
void print_free_list();
void print_free_list(){
    FreeListNode L = free_list_begin();
    printf("Start of Free List----------------------------\n");
    if(L == NULL)
    {
        printf("NULL\n");
        return;
    }
    int count = 0;
    int* size2;
    while(L != NULL){
        size2 = (int*)((char*)L + 8);
        printf("Node[%d]: address: %p size2:%d\n", count, L,*size2);
        count++;
        L = L -> flink;
    }
    printf("End of Free List------------------------------\n\n\n");
}

// Test 1: Basic Malloc
// void check_basic_malloc(){
//     printf("Test 1 Start--------------------------\n");

//     void *chunk1 = my_malloc(100);
//     void *chunk2 = my_malloc(100);
//     printf("Test 1 - a: Basic Malloc: %s\n", ((chunk1 != NULL) && (chunk2 != NULL) && (chunk2 - chunk1 == 112)) ? "Passed" : "Failed");

//     void *chunk3 = my_malloc(1);
//     void *chunk4 = my_malloc(1);
//     printf("Test 1 - b: Basic Malloc: %s\n", ((chunk3 != NULL) && (chunk4 != NULL) && (chunk4 - chunk3 == 16)) ? "Passed" : "Failed");

//     void *chunk5 = my_malloc(100);
//     void *chunk6 = my_malloc(100);
//     printf("Test 1 - c: Basic Malloc: %s\n", ((chunk5 != NULL) && (chunk6 != NULL) && (chunk6 - chunk5 == 112)) ? "Passed" : "Failed");

//     void *chunk7 = my_malloc(8184);
//     void *chunk8 = my_malloc(8184);
//     printf("Test 1 - d: Basic Malloc: %s\n", ((chunk7 != NULL) && (chunk8 != NULL) && (chunk8 - chunk7 == 8192)) ? "Passed" : "Failed");

//     void *chunk9 = my_malloc(10000);
//     printf("Test 1 Done--------------------------\n");
// }

// Test 2: Basic Free
void check_basic_free(){
    void *chunk1 = my_malloc(100);
    void *chunk2 = my_malloc(10);
    void *chunk3 = my_malloc(1);
    my_free(chunk1);
    my_free(chunk2);
    my_free(chunk3);
    print_free_list();

    void *chunk4 = my_malloc(100);
    void *chunk5 = my_malloc(10);
    void *chunk6 = my_malloc(1);
    my_free(chunk6);
    my_free(chunk5);
    my_free(chunk4);
    print_free_list();
    // Here you might want to check some internal state of your malloc library to verify the chunk was freed.
    // printf("Test 2: Basic Free: %s\n", (free_list_begin == 112) ? "Passed" : "Failed");
}

// // Test 3: Reclaim
// void check_reclaim(){
//     void *chunk1 = my_malloc(100);
//     my_free(chunk1);
//     void *chunk2 = my_malloc(100);
//     printf("Test 3: Reclaim: %s\n", (chunk1 == chunk2) ? "Passed" : "Failed");
// }

// Test 4: Fragmentation
void check_fragmentation(){
    void *chunk1 = my_malloc(32);
    void *chunk2 = my_malloc(100);
    my_free(chunk1);
    print_free_list();
    void *chunk3 = my_malloc(24);
    print_free_list();
    
    // printf("Test 4: Fragmentation: %s\n", (*(uint32_t *)chunk2 - *(uint32_t *)chunk1 == 16) ? "Passed" : "Failed");
}

// Test 5: Coalesce
void check_coalesce(){
    void *chunk1 = my_malloc(8);
    void *chunk2 = my_malloc(16);
    void *chunk3 = my_malloc(24);
    
    my_free(chunk1);
    my_free(chunk2);
    my_free(chunk3);
    print_free_list();
    coalesce_free_list();
    print_free_list();
    
    // printf("Test 5: Coalesce: %s\n", /* condition to check chunks are coalesced */ ? "Passed" : "Failed");
}

// // Test 6: Large Allocation
// void check_large_allocation(){
//     void *chunk = my_malloc(9000);
//     printf("Test 6: Large Allocation: %s\n", (chunk != NULL) ? "Passed" : "Failed");
// }

// // Test 7: Magic Number
void check_magic_number(){
    // uint32_t* magic_num = (uint32_t*)((char*)chunk1 - 4);
    // printf("magic num: %u. \n", *magic_num);

    // freeing one pointer repeatedly
    // void* chunk1 = my_malloc(8);
    // my_free(chunk1);
    // void* chunk2 = my_malloc(8);
    // my_free(chunk1);

    void* chunk1 = my_malloc(8);
    my_free(chunk1);
    // my_free(chunk1+1);
    my_free(NULL);

    print_free_list();
    printf("EXIT STATUS: %d\n", my_errno);

}

// // Test 8: Manual SBRK()
// void check_manual_sbrk(){
//     void *chunk1 = sbrk(0);
//     sbrk(8192);
//     void *chunk2 = sbrk(0);
//     printf("Test 8: Manual SBRK(): %s\n", (*(uint32_t *)chunk2 - *(uint32_t *)chunk1 == 8192) ? "Passed" : "Failed");
// }

void print_chunk_info(void *ptr) {
    if (ptr == NULL) {
        printf("Pointer is null.\n");
        return;
    }

    // Retrieve header content
    uint32_t *chunk_size = (uint32_t *)((char *)ptr - 8);
    uint32_t *magic_num = (uint32_t *)((char *)ptr - 4);

    // Print chunk size and magic number
    printf("Chunk size: %u\n", *chunk_size);
    printf("Magic number: %u\n", *magic_num);
}


int main(int argc, const char * argv[]){
    
    // check_size_update();
    // check_basic_malloc();
    check_basic_free();
    // check_reclaim();
    // check_fragmentation();
    // check_coalesce();
    // check_large_allocation();
    // check_magic_number();
    // check_manual_sbrk();


    return 0;
}



// Test Messages

// Test 1: Basic Malloc
// Test Failed: b' \x[20 chars]t\n\nDistance between allocation 0 and 1 is 16[542 chars]\n\n' != b' \x[20 chars]t\n\n' : Basic Test Failed -- Chunks are not being allocated to the correct relative positions in the heap
// Five or six sequential malloc() of various size, follow by measuring the addresses
// Printing out sizes of chunks that is returned.

// Test 2: Basic Free
// Test Failed: b' \x08Start of Free Test\n\nFirst node at: 0x0\[1567 chars]\n\n' != b' \x08' : Free Test Failed -- Chunks are not being free'd correctly
// Malloc a bunch of things and free them in various orders, and then print free list to see if they are placed in the correct order
// Even freed in reverse order, the free list should be of that order

// Test 3: Reclaim
// Test Failed: b' \x08Start of Reclaim Test\n\nFirst node at: 0[562 chars]\n\n' != b' \x08' : Reclaim Test Failed -- Chunks are not being reused correctly
// Do 6 different malloc and free them all and then malloc again to see if they are still in same place

// Test 4: Fragmentation
// Test Failed: b' \x[37 chars]t\n\nSize Recorded: 64\nFirst node at: 0x40\nS[63 chars]\n\n' != b' \x[37 chars]t\n\n' : Fragmentation Test Failed -- Small chunks (< 16 bytes) are being split from chunks
// One case: if you got chunk on free list of size 48 and the chunk size is 40
// Allocate malloc(32) which will generate 40 size chunk and free it (40 on free list)
// Malloc again for 40 and see if it gives the entire 40 bytes

// Test 5: Coalesce
// Test Failed: b' \x[42 chars]t: 0x40\nSize of free chunk: 16\nNext node at:[699 chars]\n\n' != b' \x[42 chars]t: 0x50\nSize of free chunk: 32\nNext node at:[1005 chars]\n\n' : Coalesce Test Failed -- Chunks are not being merged correctly
// Test Failed: b' \x[37 chars]t\n\nSize Recorded: 64\nFirst node at: 0x40\nS[63 chars]\n\n' != b' \x[37 chars]t\n\n' : Fragmentation Test Failed -- Small chunks (< 16 bytes) are being split from chunks
// Any consecutive chunk in memory should be merged to a large chunk

// Test 6: Large Allocation
// Test Failed: b' \x[31 chars]t\n\nSize Recorded: 10016\nFirst node at: 0x0\[205 chars]\n\n' != b' \x[31 chars]t\n\n' : Large Allocation Test Failed -- Large chunks (>= 8192 bytes) are not allocated correctly.
// malloc( > 8192): also test 8192 - 8

// Test 7: Magic Number
// Test Failed: b' \x[287 chars]t: 0x0\nSize of free chunk: 24\nNext node at: [104 chars]\n\n' != b' \x[287 chars]t: 0x18\nSize of free chunk: 88\nNext node at:[104 chars]\n\n' : Magic Number Test Failed -- Magic number is not being correctly checked by my_free
// Free an address that shouldn't be freed
// ERRNO set to bad pointer

// Test 8: Manual SBRK()
// Test Failed: b' \x[31 chars]t\n\nSize Recorded: 10016\nFirst node at: 0x0\[205 chars]\n\n' != b' \x[31 chars]t\n\n' : Large Allocation Test Failed -- Large chunks (>= 8192 bytes) are not allocated correctly.
