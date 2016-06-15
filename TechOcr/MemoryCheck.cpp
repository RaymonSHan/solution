
#ifdef _DEBUG
#include "MemoryCheck.h"

MYINT MemoryCheck[MAX_MEMORYCHECK];

void ADD_COUNT_MEMORY(int count) {
	InterInc(&MemoryCheck[count]);
}
void SUB_COUNT_MEMORY(int count) {
	InterDec(&MemoryCheck[count]);
}











#endif _DEBUG