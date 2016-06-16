#pragma once

#include "rcommon.h"

#ifdef _DEBUG

#define MAX_MEMORYCHECK			64

void ADD_COUNT_MEMORY(int count);
void SUB_COUNT_MEMORY(int count);

#define NEW_STRING				ADD_COUNT_MEMORY(1);
#define DEL_STRING				SUB_COUNT_MEMORY(1);
#define NEW_IPLIMAGE			ADD_COUNT_MEMORY(2);
#define DEL_IPLIMAGE			SUB_COUNT_MEMORY(2);
#define NEW_PIXIMAGE			ADD_COUNT_MEMORY(3);
#define DEL_PIXIMAGE			SUB_COUNT_MEMORY(3);
#define NEW_CVSEQ				ADD_COUNT_MEMORY(4);
#define DEL_CVSEQ				SUB_COUNT_MEMORY(4);
#define NEW_STORAGE				ADD_COUNT_MEMORY(5);
#define DEL_STORAGE				SUB_COUNT_MEMORY(5);
#define NEW_API					ADD_COUNT_MEMORY(6);
#define DEL_API					SUB_COUNT_MEMORY(6);
#define NEW_PIXA				ADD_COUNT_MEMORY(7);
#define DEL_PIXA				SUB_COUNT_MEMORY(7);
#define NEW_BOXA				ADD_COUNT_MEMORY(8);
#define DEL_BOXA				SUB_COUNT_MEMORY(8);




#define     ADDR_COMPARE(op)					\
  BOOL inline operator op (ADDR &one, const ADDR &two) {	\
    return (one.aLong op two.aLong); };



#else  _DEBUG
#define NEW_STRING
#define DEL_STRING
#define NEW_IPLIMAGE
#define DEL_IPLIMAGE
#define NEW_PIXIMAGE
#define DEL_PIXIMAGE
#define NEW_CVSEQ
#define DEL_CVSEQ
#define NEW_STORAGE
#define DEL_STORAGE
#define NEW_API
#define DEL_API
#define NEW_PIXA
#define DEL_PIXA
#define NEW_BOXA
#define DEL_BOXA
#endif _DEBUG
