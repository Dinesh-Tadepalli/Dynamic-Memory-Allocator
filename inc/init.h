/******************************************************************************
 * @file: init.h
 *
 * Header file for functions that initialize and cleanup a heap space for P3 testing.
 *
 * @creator: Bujji Setty (selagamsetty@wisc.edu)
 * @modified: 8/5/22 Bujji Setty (selagamsetty@wis.edu)
 *
 *****************************************************************************/
#include <stdbool.h>
#include <stddef.h>

#ifndef __INIT_H__
#define __INIT_H__

typedef struct ArgParse_Results {
    bool pro;  // flag to check if prologue block was set correctly
    bool epi;  // flag to check if epilogue block was set correctly
    bool foot; // flag to check if footers are set on every block
} Flags;

void *Initialize_Heap_Space(int argc, char *argv[], char init_val, size_t size, Flags *F);

void Check_Prologue();

void Check_Epilogue();

void Check_Footers();

void Tear_Down_Heap_Space();

#endif // __INIT_H__


