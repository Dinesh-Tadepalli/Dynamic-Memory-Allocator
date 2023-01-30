#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mem.h"
#include "header.h"
#include "util.h"

#define HEAP_SIZE 96
#define INIT_VAL 0x66

int main() {
    void *heap = NULL;    
    size_t alloc_size = 0;
    alloc_size += MIN_BLOCK_SIZE; // Prologue 
    alloc_size += sizeof(Header); // First header
    alloc_size += HEAP_SIZE;      // Heap space
    alloc_size += sizeof(Header); // Epilogue 
    alloc_size += MIN_BLOCK_SIZE - 1; // Padding to align heap space 
    while (!heap) 
        heap = malloc(alloc_size); 

    assert(heap);
    // Init heap to random value
    memset(heap, INIT_VAL, alloc_size);

    char *chp = (char *) heap;
    chp += MIN_BLOCK_SIZE; // prologue
    chp += sizeof(Header); // first header
    while ((unsigned long) chp % 16) 
        chp++;
    // chp now points to the address the first user pointer would have

    chp -= sizeof(Header);

    // Begin actual test stuff
    Mem_Init(chp, HEAP_SIZE); // pointer to first header
    Header *hp = (Header *) chp;
    if (!(*hp == HEAP_SIZE)){
        // check first header set correctly
        printf("[ERROR]: First header %p was set to %lu, but should be %d!\n",
                hp, (size_t)*hp, HEAP_SIZE);
        exit(1);
    }
    //Print_Heap_Blocks(chp, HEAP_SIZE);

    char *p = Mem_Alloc(9);
    if (p == NULL) {
        printf("[ERROR]: Mem_Alloc returned NULL pointer p when heap had space!\n");
        exit(1);
    }
    if ((size_t)p % 16 != 0) {
        printf("[ERROR]: Mem_Alloc return non-aligned pointer for p=%p!\n",p);
        exit(1);
    }
    hp = (Header *)(p - sizeof(Header));
    if (!(*hp == 33)) {
        // Check header of first alloc set correctly
        printf("[ERROR]: Allocated header %p was set to %lu, but should be %d!\n",
                hp, (size_t)*hp, 33);
        exit(1);
    }
    hp = (Header *)(p + 32 - sizeof(Header));
    if (!(*hp == 64)) { 
        // Check header of split block set correctly
        printf("[ERROR]: Split block header %p was set to %lu, but should be %d!\n",
                hp, (size_t)*hp, 64);
        exit(1);
    }
    //Print_Heap_Blocks(chp, HEAP_SIZE);

    char *q = Mem_Alloc(9);
    if (q == NULL) {
        printf("[ERROR]: Mem_Alloc returned NULL pointer q when heap had space!\n");
        exit(1);
    }
    if ((size_t)q % 16 != 0) {
        printf("[ERROR]: Mem_Alloc return non-aligned pointer for q=%p!\n",q);
        exit(1);
    }
    hp = (Header *)(q - sizeof(Header));
    if (!(*hp == 33)) {
        // Check header of second alloc set correctly
        printf("[ERROR]: Second allocated header %p was set to %lu, but should be %d!\n",
                hp, (size_t)*hp, 33);
        exit(1);
    }
    hp = (Header *)(q + 32 - sizeof(Header));
    if (!(*hp == 32)) {
        // Check header of split block set correctly
        printf("[ERROR]: Split block header %p was set to %lu, but should be %d!\n",
                hp, (size_t)*hp, 32);
        exit(1);
    }
    //Print_Heap_Blocks(chp, HEAP_SIZE);

    char *r = Mem_Alloc(9);
    if (r == NULL) {
        printf("[ERROR]: Mem_Alloc returned NULL pointer r when heap had space!\n");
        exit(1);
    }
    if ((size_t)r % 16 != 0) {
        printf("[ERROR]: Mem_Alloc return non-aligned pointer for r=%p!\n",r);
        exit(1);
    }
    hp = (Header *)(r - sizeof(Header));
    if(!(*hp == (Header)33)) {
        // Check header of third alloc set correctly
        printf("[ERROR]: Third allocated header %p was set to %lu, but should be %d!\n",
                hp, (size_t)*hp, 33);
        exit(1);
    }
    //Print_Heap_Blocks(chp, HEAP_SIZE);

    p[ 0] = 'T';
    q[ 0] = 'h';
    r[ 0] = 'i';

    // Case 1: Free a block that has allocated block above and allocated block below
    int free_success = Mem_Free(q);
    if(!free_success) {
        printf("[ERROR]: Free q for Case 1 failed when it should've succeeded!\n");
        exit(1);
    }

    hp = (Header *)(q - sizeof(Header));
    if(*hp != 32) { 
        // Check header of q freed correctly
        printf("[ERROR]: Second freed header %p was set to %lu, but should be %d!\n",
                hp, (size_t)*hp, 32);
        exit(1);
    }
    hp = (Header *)(q + 32 - sizeof(Header));
    if(*hp != 33) { 
        // Check header of r is unchanged
        printf("[ERROR]: Third allocated header %p was changed to %lu, but should've stayed %d!\n",
                hp, (size_t)*hp, 33);
        exit(1);
    }
    //Print_Heap_Blocks(chp, HEAP_SIZE);

    // Case 2: Free a block that has free block above and allocated block below
    free_success = Mem_Free(r);
    if (!free_success) {
        printf("[ERROR]: Free r for Case 2 failed when it should've succeeded!\n");
        exit(1);
    }
    hp = (Header *)(q - sizeof(Header));
    if(*hp != 64) { 
        // Check header of q coalesced with r correctly
        printf("[ERROR]: Second freed header %p was set to %lu, but should be %d!\n",
                hp, (size_t)*hp, 64);
        exit(1);
    }
    //Print_Heap_Blocks(chp, HEAP_SIZE);

    // Case 3: Free a block that has allocateed block above and free block below
    free_success = Mem_Free(p);
    if (!free_success) {
        printf("[ERROR]: Free p for Case 3 failed when it should've succeeded!\n");
        exit(1);
    }
    hp = (Header *)(p - sizeof(Header));
    if (*hp != HEAP_SIZE) { 
        // Check header of p coalesced with q&r correctly
        printf("[ERROR]: First freed header %p was set to %lu, but should be %d!\n",
                hp, (size_t)*hp, HEAP_SIZE);
        exit(1);
    }
    //Print_Heap_Blocks(chp, HEAP_SIZE);

    // Reset for Case 4
    p = Mem_Alloc(9);
    q = Mem_Alloc(9);
    r = Mem_Alloc(9);
    Mem_Free(p);
    Mem_Free(r);
    //Print_Heap_Blocks(chp, HEAP_SIZE);

    // Case 4: Free a block that has free block above and free block below
    free_success = Mem_Free(q);
    if (!free_success) {
        printf("[ERROR]: Free q for Case 4 failed when it should've succeeded!\n");
        exit(1);
    }
    hp = (Header *)(p - sizeof(Header));
    if(*hp != HEAP_SIZE) { 
        // check first header set correctly
        printf("[ERROR]: First freed header %p was set to %lu, but should be %d!\n",
                hp, (size_t)*hp, HEAP_SIZE);
        exit(1);
    }
    //Print_Heap_Blocks(chp, HEAP_SIZE);

    free(heap);
    return 0;
}
