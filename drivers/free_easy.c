#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mem.h"
#include "header.h"
#include "util.h"

#define HEAP_SIZE 1600
#define INIT_VAL 0x88

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
   
    char *p = Mem_Alloc(15);
    p[ 0] = 't';
    p[ 1] = 'h';
    p[ 2] = 'i';
    p[ 3] = 's';
    p[ 4] = ' ';
    p[ 5] = 'i';
    p[ 6] = 's';
    p[ 7] = ' ';
    p[ 8] = 'f';
    p[ 9] = 'u';
    p[10] = 'n';
    p[11] = ' ';
    p[12] = 'f';
    p[13] = 'u';
    p[14] = 'n';
   
    if (Mem_Free(NULL)) {
        printf("[ERROR]: Mem_Free reported success on freeing NULL pointer!\n");
        exit(1);
    }
    if (Mem_Free(heap - 16)) {
        printf("[ERROR]: Mem_Free reported success on freeing pointer before heap!\n");
        exit(1);
    }
    if (Mem_Free(heap +alloc_size+ 16)) {
        printf("[ERROR]: Mem_Free reported success on freeing pointer after heap!\n");
        exit(1);
    }
    Header *hp = (Header *) chp;
    // hp now points to the first header
    *hp = (*hp) & (~0x1); // Clear LSB
    if(Mem_Free(p)) {
        printf("[ERROR]: Mem_Free reported success on freeing a block that wa already free!\n");
        exit(1);
    }
    *hp = (*hp) | 0x1; // Restore LSB
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
