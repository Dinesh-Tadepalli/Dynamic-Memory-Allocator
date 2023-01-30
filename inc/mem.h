/******************************************************************************
 * @file: mem.h
 *
 * Header file for a memory allocator.
 *
 * @creator: Bujji Setty (selagamsetty@wisc.edu)
 * @modified: 7/29/22 Bujji Setty (selagamsetty@wis.edu)
 *
 * DO NOT MODIFY THE CONTENTS OF THIS FILE
 *
 *****************************************************************************/

#ifndef __MEM_H__
#define __MEM_H__

#include "header.h"

#define MIN_BLOCK_SIZE 16

typedef Header Footer;
typedef unsigned short Payload;

void  Mem_Init(void *Heap_Start, size_t Heap_Size); 
void *Mem_Alloc(Payload payload);
int   Mem_Free(void *ptr);

#endif // __MEM_H__
