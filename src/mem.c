/******************************************************************************
 * @file: mem.c
 *****************************************************************************/

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "header.h"
#include "mem.h"

// Set this to 1 to enable dbgprintf  and dbgassert statements, make sure to 
// set it back to 0 before submitting!
#define DEBUG               0 
#define dbgprintf(...)      if (DEBUG) { printf(__VA_ARGS__); }
#define dbgassert(...)      if (DEBUG) { assert(__VA_ARGS__); }

/******************************************************************************
 * Helper functions
 *****************************************************************************/

// ADD ANY HELPER FUNCTIONS YOU MIGHT WRITE HERE 

/******************************************************************************
 * Global Variables
 *****************************************************************************/
void *heap_start;
size_t heap_size;
void * hList[1001]; 
void *first;
int offset;

/******************************************************************************
 * Memory Allocator functions
 *****************************************************************************/

/*
 * This function initializes the heap space for future calls to Mem_Alloc and 
 * Mem_Free. You may safely assume that this function is only called once in 
 * a given program.
 *
 * @param Heap_Start : A pointer to the start of the heap space
 * @param Heap_Size : The size the heap
 * @return void
 */
void Mem_Init(void *Heap_Start, size_t Heap_Size) {

    // Register the start of the heap to the global variable
    heap_start = Heap_Start;

    // Register the size of the heap to the global variable
    heap_size = Heap_Size;

    Header *startHeader = heap_start;
    *startHeader = heap_size;
  
    *startHeader = *startHeader & (~1); //mark as free
    *startHeader = *startHeader;
    hList[0] = startHeader;

    unsigned long addr = (unsigned long)heap_start;
    while (addr % 16 != 0) {
	addr++;
	offset++;
    }

    first = (void *)addr;

    Header *ptr1;
    hList[1] = first + 16 - 4;
    ptr1 = hList[1];
    *ptr1 = 0;

    Header *ptr2 = NULL;
    for (int i=2; i < 1001; i++) {
	hList[i] = hList[i-1] + 16;
	ptr2 = hList[i];
	*ptr2 = 0;
    }

    Header *prologue = heap_start - 16;
    *prologue = 16 | 1;
    Header *prologueFooter = heap_start - 4;
    *prologueFooter = *prologue;

    Header *epilogue = heap_start + heap_size;
    *epilogue = 1;

    return;
}

/*
 * This function  allows a user to request space on the heap. The type of param
 * payload is defined in mem.h and may not be changed. If param payload is ever
 * 0, this function should return NULL immediately. 
 *
 * @param payload : The number of bytes the user wants on the heap
 * @return A 16-byte aligned pointer to payload bytes on the heap on success,
 *         NULL on failure
 */
void* Mem_Alloc(Payload payload){

	if (payload == 0) {
		return NULL;
	}

	Header *hp = NULL;
	bool found = false;

	int index;

	//Search for header that fits payload
	for (int i=0; i < 1000; i++) {
		hp = hList[i];
		if (*hp == 0 || *hp >= 10000) {
			continue;
		}
		if (*hp & 1) {
			continue;
		} else {
			//Found header! But does it fit the payload?
			if (*hp - 4 - 4  >= payload) {
				hp = hList[i];
				found = true;
				index = i;
				
				break;
			}
		}
	}

	if (found == false) {
		return NULL;
	}

	void *ptr = hp;
	if (index == 0) {
		ptr = first - offset;
	} 

	int mult = (((payload + 4 + 4 - 1) / 16) + 1);
	int space = (mult)*16;
	Header *newHp = ptr + space;
	
	Header *ptr2 = hList[index + (mult)];

	//Should I add a header after this space? Or is there already a header?
	if ((*ptr2 & 1) || (ptr > (first + heap_size))) {

	} else {
	*newHp = *hp - space;
	}
	*hp = space | 1;


//Below code is for printing part of the heap space, just for debugging purposes
/*
	Header *ptr2;
	for (int i=0; i<20; i++) {
		printf("Address of %d: %p\n", i, hList[i]);
		ptr2 = hList[i];
		printf("Value: %u\n", *ptr2);

	}
*/

	if (hp == hList[0]) {
		return first;
	} else {
		return ptr+4;
	}
}


/*
 * This function  allows a user to tell the memory allocator that they finished
 * using space that they had requested on the heap.
 *
 * @param ptr: A pointer
 * @return 0 on error, 1 on success 
 */
int Mem_Free(void *ptr) {

// Again, below code is for depugging purposes.
/*
	Header *ptr2;
	for (int i=0; i<10; i++) {
		printf("Address of %d: %p\n", i, hList[i]);
		ptr2 = hList[i];
		printf("Value: %u\n", *ptr2);

	}
*/
	
	Header *prev = NULL;
	Header *current = (ptr - 4);
	Header *next = NULL;
	bool found = false;
	Header *p;
	int index = -1;

	if ( ((unsigned long)current < (unsigned long)heap_start) || (unsigned long)current > (((unsigned long)heap_start)+heap_size) ) {
		return 0;
	}

	if (!(*current & 1)) {
		return 0;
	}

	//Special case if I am freeing the first header
	if (ptr == hList[0]+4) {
	
		int j = 0;
		Header *ptr0 = hList[j+1];
		while (j < 1000) {
			ptr0 = hList[j+1];
			//Find a header after the header to be freed. Later
			//we'll check if that "next" header is allocated or free
			if ((*ptr0 <= 10000) && (*ptr0 != 0)) {
				next = hList[j+1];
				break;
			}
			j++;
		}

		if ( (!(*next & 1)) ) {
			*current = *current + *next - 1;
			memset(current+4, 0, ((*current)/4)+1 ); 
			return 1;
		} else {
			*current = *current - 1;
			memset(current+4, 0, ((*current)/4)+1 ); 
			return 1;
		}

	}

	//Case for if the header to be freed is not the first header
	for (int i=1; i<1000; i++) {
		p = hList[i-1];
		if ((*p <= 10000) && (*p != 0)) { 
			prev = hList[i-1];
		}
		if ((ptr == hList[i]+4)) {
			found = true;
			index = i;
			break;
		}
	}

	if (found == false) {
		return 0;
	}

	int j = index;
	Header *ptr0 = hList[j+1];
	while (j < 1000-index) {
		ptr0 = hList[j+1];
		if ((*ptr0 <= 10000) && (*ptr0 != 0)) {
			next = hList[j+1];
			break;
		}
		j++;
	}

	bool prevStatus = true;
	bool nextStatus = true;
	
	if (prev == 0) {
		prevStatus = false;
	}
	
	if (next == 0) {
		nextStatus = false;
	}

	if ( (prevStatus == true) && (nextStatus == true) && (!(*prev & 1)) && (!(*next & 1)) ) { //if prev and next are both free	
		*prev = *prev + *current + *next - 1;
		memset(prev+4, 0, ((*prev)/4)+1 ); 
		return 1;
	} else if ( (prevStatus == true) && (!(*prev & 1)) ) {
		*prev = *prev + *current - 1;
		memset(prev+4, 0, ((*prev)/4)+1 ); 
		return 1;
	} else if ( (nextStatus == true) && (!(*next & 1)) ) {
		*current = *current + *next - 1;
		memset(current+4, 0, ((*current)/4)+1 ); 
		return 1;
	} else {
		*current = *current - 1;
		memset(current+4, 0, ((*current)/4)+1 ); 

		return 1;
	}

    return 0;
}

