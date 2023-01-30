#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "mem.h"
#include "init.h"

#define INFO 0

void *heap; // pointer to allocation start
void *fhp; // first header pointer
size_t init_size;

void *Initialize_Heap_Space(int argc, char *argv[], char init_val, size_t size, Flags *F) {
    if (F == NULL) {
        printf("[ERROR]: Flag pointer must be allocated either on stack or heap!\n");
        exit(1);
    }
    // by default, don't do any extra checking
    F->pro = false;
    F->epi = false;
    F->foot = false;
    size_t alloc_size = 0;
    alloc_size += MIN_BLOCK_SIZE;     // Prologue 
    alloc_size += sizeof(Header);     // First header
    alloc_size += size;               // Heap space
    alloc_size += sizeof(Header);     // Epilogue 
    alloc_size += MIN_BLOCK_SIZE - 1; // Padding to align heap space 
    heap = NULL;
    while (!heap) 
        heap = malloc(alloc_size); 
    init_size = size;
    assert(heap);
    // Init heap to random value
    memset(heap, init_val, alloc_size);

    char *chp = (char *) heap;
    chp += MIN_BLOCK_SIZE; // prologue
    chp += sizeof(Header); // first header
    while ((unsigned long) chp % 16) 
        chp++;
    // chp now points to the address the first user pointer would have

    chp -= sizeof(Header);
    fhp = chp;
    if (argc == 1) {
        // no need to set any flags, just return
        return chp;
    }
    if (argc > 4) {
        printf("[ERROR]: Too many command line arguments to %s!\n", argv[0]);
        printf("[INFO ]: usage: %s [OPTIONS]\n", argv[0]);
        printf("[INFO ]: \tOPTIONS:\n");
        printf("[INFO ]: \t  -p/P: check prologue block set correctly and maintained during test\n");
        printf("[INFO ]: \t  -e/E: check epilogue block set correctly and maintained during test\n");
        printf("[INFO ]: \t  -f/F: check footers of every block are set correctly and maintained during test\n");
        exit(1);
    }
    assert(argc >= 1 && argc <= 4);
    // now we need to set flags 
    for(int i = 1; i < argc; i++) {
        if (strncasecmp(argv[i], "-p", 2) == 0)
            F->pro = true;
        if (strncasecmp(argv[i], "-e", 2) == 0)
            F->epi = true;
        if (strncasecmp(argv[i], "-f", 2) == 0)
            F->foot = true;

    }
    return chp;
}

void Check_Prologue() {
    void *first_header = fhp;
    if (INFO) printf("%s [INFO ]: Checking prologue at %p\n", __FILE__, first_header-MIN_BLOCK_SIZE);
    char *pro_footer = first_header - sizeof(Footer);
    char *pro_header = first_header - MIN_BLOCK_SIZE;
    if (*(Header *)pro_header != 17) {
        printf("%s [ERROR]: Incorrectly set prologue header %p to %lu when should've been %lu\n",
                __FILE__, pro_header, (size_t)*(Header*)pro_header, (size_t)17);
        exit(1);
    }
    if (*(Footer *)pro_footer != 17) {
        printf("%s [ERROR]: Incorrectly set prologue footer %p to %lu when should've been %lu\n",
                __FILE__, pro_footer, (size_t)*(Header*)pro_footer, (size_t)17);
        exit(1);
    }
}

void Check_Epilogue() {
    void *first_header = fhp;
    size_t size = init_size;
    if (INFO) printf("%s [INFO ]: Checking epilogue at %p\n", __FILE__, first_header+size);
    char *epi = first_header + size;
    if (*(Header *)epi != 1) {
        printf("%s [ERROR]: Incorrectly set epilogue header %p to %lu when should've been %lu\n",
                __FILE__, epi, (size_t)*(Header*)epi, (size_t)1);
        exit(1);
    }
}

void Check_Footers() {
    Header *h = fhp;
    Footer *f;
    Header hv;
    while (h < (Header *)((char *)fhp + init_size)) {
        assert(h >= (Header *)fhp);
        if (INFO) printf("%s [INFO ]: Checking footer of header at %p\n", __FILE__, h);
        hv = *h;
        h = (void *)h + (*h & ~0x1);
        f = (void *)h - sizeof(Footer);
        if (INFO) printf("%s [INFO ]: Footer is at %p\n", __FILE__, f);
        if (*f != hv) {
            printf("%s [ERROR]: Incorrectly set footer %p to %lu when should've been %lu\n",
                __FILE__, f, (size_t)*f, (size_t)hv);
            exit(1);
        }
    }
}

void Tear_Down_Heap_Space() {
    assert(heap);
    free(heap);
}
