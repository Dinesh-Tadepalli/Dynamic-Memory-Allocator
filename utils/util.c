#include <stdbool.h>
#include <stdio.h>
#include "mem.h"
#include "util.h"

void __Print_Block(Header *hp) {
    Payload p = *hp & (~0x1);
    bool allocated = (*hp & 0x1) == 0x1;
    if (allocated)
        printf("\t\t[%p | %5u | A]\n", hp, p);
    else 
        printf("\t\t[%p | %5u | F]\n", hp, p);
}

void Print_Heap_Blocks(void *start, size_t size) {
    char *chp = (char *) start;
    Header *heap_end = (Header *)(chp + size); 
    Header *heap_start = (Header *)(chp);
    size_t total_bytes = 2*sizeof(Header) + 2*MIN_BLOCK_SIZE - 1 + size;
    printf("/********************* START OF HEAP (>=%p) ***********************\\\n\n", heap_start);
    printf("\t\t  Heap is %lu (=0x%lx) bytes\n", total_bytes, total_bytes);
    printf("\t\t  Sizeof(Header) is %5lu\n", sizeof(Header));

    printf("\t\t  First non-prologue header should be at %p\n\n", chp);

    printf("\t[Header Address | Block Size | Block Designation]\n\n");

    chp -= MIN_BLOCK_SIZE;
    // Test if prologue block exists
    Header *hp = (Header *) chp;
    if (*hp == 17) {
        printf("\t\t[%p | %5u | A] (Prologue)\n", hp, 16);
    } else {
        printf("\t\t[%p | %4lu+ | P]\n", heap_start, 
                (size_t)16);
    }

    chp += MIN_BLOCK_SIZE; // skip prologue
    hp = (Header *) chp;
    while(hp < (heap_end - 1)) {
        __Print_Block(hp);
        chp += (*hp & (~0x1));
        hp = (Header *) chp;
    }
    chp = (char *)start;
    chp += size;
    hp = (Header *) chp;
    if (*hp == 0x1) {
        // test if epilogue is set
        printf("\t\t[%p | %5u | A] (Epilogue)\n", hp, 0);
    } else {
        printf("\t\t[%p | %5lu | P]\n", hp, 
                sizeof(Header));
    }

    printf("\n\\********************* END OF HEAP (<%p) ************************/\n", heap_end);
    return;
}
