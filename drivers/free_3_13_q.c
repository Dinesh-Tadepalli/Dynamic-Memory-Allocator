#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mem.h"
#include "header.h"
#include "util.h"

#define HEAP_SIZE 1600
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
    Mem_Init(chp, HEAP_SIZE);
   
    char *p = Mem_Alloc(13);
    char *q = Mem_Alloc(14);
    char *r = Mem_Alloc(15);
    p[ 0] = 'T';
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
    p[12] = '!';
    if (!Mem_Free(q)) {
        printf("[INFO ]: Heap start is at address %p\n", chp);
        printf("[ERROR]: Mem_Free reported failure on valid pointer q!\n");
        exit(1);
    }
    Header *hp = (Header *)p; hp--;
    Header *hq = (Header *)q; hq--;
    Header *hr = (Header *)r; hr--;

    if (*hp != 33) {
        printf("[INFO ]: Heap start is at address %p\n", chp);
        printf("[ERROR]: Mem_Free changed p when freeing q!\n");
        exit(1);
    }
    if (*hq != 32) {
        printf("[INFO ]: Heap start is at address %p\n", chp);
        printf("[ERROR]: Mem_Free incorrectly set block q to free!\n");
        exit(1);
    }
    if (*hr != 33) {
        printf("[INFO ]: Heap start is at address %p\n", chp);
        printf("[ERROR]: Mem_Free changed r when freeing q!\n");
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
