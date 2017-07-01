#pragma once
#include "malloc.h"
//#include "mutex.h"

bool blockQueueTest(); // returns true if all is well

class blockQueue
{
public:
	typedef enum { Empty = 0, Full, EmptyFull, EmptyFullEmpty, FullEmptyFull, FullEmpty } State;

	blockQueue(int _blockSize, int nbBlocks = 2) :blockSize(_blockSize), nbUsedBlocks(0), nbTotalBlocks(nbBlocks), state(Empty)
	{
		blockPtr = (char*)malloc(nbTotalBlocks * blockSize);
	}

	int insert(const void* dataPtr, int size); // inserts an item. Returns 0 if successful, negative error code otherwise

	int remove(void* dataPtr, int size); // removes an item. Returns 0 if successful, negative error code otherwise

	int nb() const { return nbUsedBlocks; }

protected:
	State state;
	int grow(); // returns position of next free block or -1 if error
	

	int blockSize;
	char* blockPtr;
	int nbUsedBlocks, nbTotalBlocks;
	int posInsert, posRemove; // positions where insert and removal is happening
	blockQueue() = delete;
	blockQueue(const blockQueue&) = delete;
	blockQueue& operator=(blockQueue&) = delete;
};