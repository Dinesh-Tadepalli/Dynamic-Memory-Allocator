/******************************************************************************
 * @file: stress.c
 *
 * Main file that stresses and benchmarks the memory allocator.
 *
 * @creator: Bujji Setty (selagamsetty@wisc.edu)
 * @modified: 7/31/22 Bujji Setty (selagamsetty@wisc.edu)
 *
 * DO NOT MODIFY THE CONTENTS OF THIS FILE
 *
 *****************************************************************************/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mem.h"
#include "header.h"
#include "util.h"

#define HEAP_SIZE       1048576
#define NUM_REQUESTS    1000000
#define INIT_VAL           0xcc
#define SEED         0xdeadbeef

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
    srand(SEED);
   
    char *p[NUM_REQUESTS];
    clock_t start, end;
    start = clock();
    Mem_Init(chp, HEAP_SIZE);
    for (size_t i = 0; i < NUM_REQUESTS; i++) {
        p[i] = NULL;
    }

    for (size_t i = 0; i < NUM_REQUESTS; i++) {
        p[i] = Mem_Alloc(15);
    }
    for (size_t i = 0; i < NUM_REQUESTS; i++) {
        if (i % 2 == 0) {
            Mem_Free(p[i]);
            p[i] = NULL;
        }
    }
    for (size_t i = 0; i < NUM_REQUESTS; i++) {
        if (i%2 == 0) {
            p[i] = Mem_Alloc(1);
        }
    }
    for (size_t i = 0; i < NUM_REQUESTS; i++) {
        Mem_Free(p[i]);
        p[i] = NULL;
    }
    for (size_t i = 0; i < NUM_REQUESTS-1; i++) {
        p[i+1] = Mem_Alloc(i+1);
    }
    for (size_t i = 0; i < NUM_REQUESTS; i++) {
        if (i % 2 == 0) {
            Mem_Free(p[i]);
            p[i] = NULL;
        }
    }
    p[2] = Mem_Alloc(256);
    p[4] = Mem_Alloc(512);
    p[6] = Mem_Alloc(1024);
    p[8] = Mem_Alloc(2048);
    for (size_t i = 0; i < NUM_REQUESTS; i++) {
        Mem_Free(p[i]);
        p[i] = NULL;
    }
    for (size_t i = 0; i < NUM_REQUESTS; i++) {
        p[i] = Mem_Alloc(rand() % ((HEAP_SIZE/4) + 1));
    }
    for (size_t i = 0; i < NUM_REQUESTS; i++) {
        Mem_Free(p[i]);
        p[i] = NULL;
    }

    end = clock();
    double elapsed = (double) (end - start) / CLOCKS_PER_SEC;
    printf("Took %f seconds!\n", elapsed);
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
