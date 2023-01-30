#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mem.h"
#include "header.h"
#include "util.h"

#define HEAP_SIZE 1600
#define INIT_VAL 0x22

int main() {
    void *heap = NULL;
    size_t alloc_size = 0;
    alloc_size += MIN_BLOCK_SIZE;     // Prologue 
    alloc_size += sizeof(Header);     // First header
    alloc_size += HEAP_SIZE;          // Heap space
    alloc_size += sizeof(Header);     // Epilogue 
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
    Mem_Init(chp, HEAP_SIZE);
    
    Header *hp = (Header *) chp;
    // hp now points to the first header

    Header block_size = HEAP_SIZE;
    block_size &= (~0x1);
    // Ignore whether block is allocated at first
    Header actual = ((*hp) & (~0x1));
    if (actual != block_size) {
        printf("[INFO ]: Heap start is at address %p\n", chp);
        printf("[ERROR]: Incorrectly set value of first header (=%lu), should be %lu!\n",
                (unsigned long)actual,
                (unsigned long)block_size);
        exit(1);
    }
    // Now test that block is free
    if ((*hp) & (0x1)) {
        printf("[INFO ]: Heap start is at address %p\n", chp);
        printf("[ERROR]: First header must be marked as free!\n");
        exit(1);
    }
    /*
    chp = (char *) heap;
    chp += MIN_BLOCK_SIZE; // prologue
    chp += sizeof(Header); // first header
    while ((unsigned long) chp % 16) 
        chp++;
    chp -= sizeof(Header);
    Print_Heap_Blocks(chp, HEAP_SIZE);
    */
    free(heap);
    return 0;
}
