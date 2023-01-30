#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mem.h"
#include "header.h"
#include "util.h"

#define HEAP_SIZE       2400
#define NUM_REQUESTS  100000
#define INIT_VAL        0xaa

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
    Mem_Init(chp, HEAP_SIZE);
   
    char *p[NUM_REQUESTS];
    for (size_t i = 0; i < NUM_REQUESTS; i++) {
        p[i] = NULL;
    }

    size_t successes = 0;
    for (size_t i = 0; i < NUM_REQUESTS; i++) {
        p[i] = Mem_Alloc(1);
        if (p[i] && p[i] >= chp && p[i] < (chp+HEAP_SIZE)) {
            successes++;
        }
    }
    for (size_t i = 0; i < NUM_REQUESTS; i++) {
        Mem_Free(p[i]);
        p[i] = NULL;
    }
    for (size_t i = 0; i < NUM_REQUESTS; i++) {
        p[i] = Mem_Alloc(9);
        if (p[i] && p[i] >= chp && p[i] < (chp+HEAP_SIZE)) {
            successes++;
        }
    }
    for (size_t i = 0; i < NUM_REQUESTS; i++) {
        Mem_Free(p[i]);
        p[i] = NULL;
    }
    printf("Successfully fulfilled %lu of %u requests!\n", 
            successes, 2*NUM_REQUESTS);
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
