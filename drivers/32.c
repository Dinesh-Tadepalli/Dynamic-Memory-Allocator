
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mem.h"
#include "header.h"
#include "init.h"
#include "util.h"

#define HEAP_SIZE          2400
#define INIT_VAL           0x00

int main(int argc, char *argv[]) {
    Flags F;
    char *chp = Initialize_Heap_Space(argc, argv, INIT_VAL, HEAP_SIZE, &F);

    // Begin actual test stuff
    Mem_Init(chp, HEAP_SIZE);

    if (F.pro)  Check_Prologue();
    if (F.epi)  Check_Epilogue();
    if (F.foot) Check_Footers();
    char *p[75];
    Header *hp;
    for (size_t i = 0; i <75; i++){
        p[i] = Mem_Alloc(9);
        if (p[i] == NULL){
            printf("[ERROR]: Mem_Alloc reported failure when should've succeeded!\n");
            exit(1);
        }
        if ((size_t)p[i]%16){
            printf("[ERROR]: Mem_Alloc returned non-aligned pointer=%p!\n", p[i]);
            exit(1);
        }
        hp = (Header *)(p[i] - sizeof(Header));
        if(*hp != 33){
            printf("[ERROR]: Mem_Alloc wrote incorrect header value to %p!\n", hp);
            exit(1);
        }
        if (F.pro)  Check_Prologue();
        if (F.epi)  Check_Epilogue();
        if (F.foot) Check_Footers();
    }
    if (Mem_Free(chp + HEAP_SIZE)){
        printf("[ERROR]: Mem_Free of pointer after last footer should not succeed!\n");
        exit(1);
    }
    if (F.pro)  Check_Prologue();
    if (F.epi)  Check_Epilogue();
    if (F.foot) Check_Footers();

    Tear_Down_Heap_Space();
    return 0;
}
