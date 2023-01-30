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
    if (!p) {
        printf("[INFO ]: Heap start is at address %p\n", chp);
        printf("[ERROR]: Returned p=null on 13 byte allocation request in 1600 byte heap!\n");
        exit(1);
    }
    if (!q) {
        printf("[INFO ]: Heap start is at address %p\n", chp);
        printf("[ERROR]: Returned q=null on 14 byte allocation request in 1600 byte heap!\n");
        exit(1);
    }
    if (!r) {
        printf("[INFO ]: Heap start is at address %p\n", chp);
        printf("[ERROR]: Returned r=null on 14 byte allocation request in 1600 byte heap!\n");
        exit(1);
    }
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
    chp += sizeof(Header);
    if (p != chp) {
        printf("[INFO ]: Heap start is at address %p\n", chp);
        printf("[ERROR]: p is incorrect, must use a first fit policy!\n");
        exit(1);
    }
    if ((unsigned long)p % (unsigned long)16) {
        printf("[ERROR]: p is a non-aligned pointer\n");
        exit(1);
    }
    if (q != (chp+32)) {
        printf("[INFO ]: Heap start is at address %p\n", chp);
        printf("[ERROR]: q is incorrect, must use a first fit policy!\n");
        exit(1);
    }
    if ((unsigned long)q % (unsigned long)16) {
        printf("[ERROR]: q is a non-aligned pointer\n");
        exit(1);
    }
    if (r != (chp+64)) {
        printf("[INFO ]: Heap start is at address %p\n", chp);
        printf("[ERROR]: r is incorrect, must use a first fit policy!\n");
        exit(1);
    }
    if ((unsigned long)r % (unsigned long)16) {
        printf("[ERROR]: r is a non-aligned pointer\n");
        exit(1);
    }
    Header *hp = (Header *) chp;
    hp--;
    // hp now points to the first header

    if ((unsigned long)(*hp) != 33) {
        printf("[ERROR]: Allocated block p must be 32 bytes and have LSB set!\n");
        exit(1);
    }
    chp = chp + ((unsigned long)(*hp)&(~0x1));
    // chp now points to q's user pointer
    hp = (Header *) chp;
    hp--;
    // hp now points to the header for q
    if ((unsigned long)(*hp) != 33) {
        printf("[ERROR]: Allocated block q must be 32 bytes and have LSB set!\n");
        exit(1);
    }
    chp = chp + ((unsigned long)(*hp)&(~0x1));
    // chp now points to r's user pointer
    hp = (Header *) chp;
    hp--;
    // hp now points to the header for r
    if ((unsigned long)(*hp) != 33) {
        printf("[ERROR]: Allocated block r must be 32 bytes and have LSB set!\n");
        exit(1);
    }
    chp = chp + ((unsigned long)(*hp)&(~0x1));
    // chp now points to next block
    hp = (Header *) chp;
    hp--;
    // hp now points to the header for the split block
    Header block_size = HEAP_SIZE - 96;
    block_size &= (~0x1);

    Header actual = ((*hp) & (~0x1));
    if (actual != block_size) {
        printf("[ERROR]: Incorrectly set header of split block (=%lu), should be %lu!\n",
                (unsigned long)actual,
                (unsigned long)block_size);
        exit(1);
    }
    // Now test that block is free
    if ((*hp) & (0x1)) {
        printf("[ERROR]: Block following the split bust be marked as free!\n");
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
