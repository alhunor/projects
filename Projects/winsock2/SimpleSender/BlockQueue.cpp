
#include "blockQueue.h"
#include <memory.h>

int blockQueue::insert(const void* dataPtr, int size)
{
	int start;
	switch (state)
	{
	case Empty:
		//insert(0);
		if (nbTotalBlocks == 0)
		{
			if (grow() < 0)
			{
				// could not grow
				return -1;
			}
		}
		start = 0;
		memcpy(blockPtr + start, dataPtr, size);
		posInsert = 1;
		posRemove = 0;
		state = FullEmpty;
		break;
	case EmptyFull:
		start = 0;
		memcpy(blockPtr + start, dataPtr, size);
		posInsert = 1;
		if (posInsert == posRemove)
		{
			state = Full;
		} else state = FullEmptyFull;
		break;
	case EmptyFullEmpty:
		start = posInsert * blockSize;
		memcpy(blockPtr + start, dataPtr, size);
		++posInsert;
		if (posInsert == nbTotalBlocks)
		{
			state = EmptyFull;
		}
		break;
	case Full:
		if (grow() < 0)
		{
			// could not grow
			return -1;
		}
		state = FullEmpty;
		// no break, coninue with code for FullEmpty
	case FullEmpty:
		start = posInsert * blockSize;
		memcpy(blockPtr + start, dataPtr, size);
		++posInsert;
		if (posInsert == nbTotalBlocks)
		{
			state = Full;
		}
		break;
	case FullEmptyFull:
		start = posInsert * blockSize;
		memcpy(blockPtr + start, dataPtr, size);
		++posInsert;
		if (posInsert == posRemove)
		{
			state = Full;
		}
		break;
	default:
		throw "Datastructure has been corrupted";
	}
	++nbUsedBlocks;
	return state;
} // blockQueue::insert(const void* dataPtr, int size)

int blockQueue::remove(void* dataPtr, int size)
{
	int start;
	switch (state)
	{
	case Empty:
		return -1; // erroe, nothing to remove
		break;
	case EmptyFull:
		start = posRemove * blockSize;;
		memcpy(dataPtr, blockPtr + start, size);
		++posRemove;
		if (posRemove == nbTotalBlocks)
		{
			state = Empty;
		}
		break;
	case EmptyFullEmpty:
		start = posRemove * blockSize;
		memcpy(dataPtr, blockPtr + start, size);
		++posRemove;
		if (posInsert == posRemove)
		{
			state = Empty;
		}
		break;
	case Full: // Remove 1st element
		start = 0;
		memcpy(dataPtr, blockPtr + start, size);
		posRemove = 1;
		if (nbTotalBlocks == posRemove)
		{
			state = Empty;
		}
		else
		{
			state = EmptyFull;
		}
		break;
	case FullEmpty:
		start = 0;
		memcpy(dataPtr, blockPtr + start, size);
		posRemove = 1;
		if (posRemove == posInsert)
		{
			state = Empty;
		}
		else
		{
			state = EmptyFullEmpty;
		}
		break;
	case FullEmptyFull:
		start = posRemove * blockSize;
		memcpy(dataPtr, blockPtr + start, size);
		++posRemove;
		if (posRemove == nbTotalBlocks)
		{
			state = FullEmpty;
		}
		break;
	default:
		throw "Datastructure has been corrupted";
	}
	--nbUsedBlocks;
	return state;
} // int blockQueue::remove(void* dataPtr, int size)

int blockQueue::grow() // returns position of next free block
{
	if (state == Full)
	{
		void* newPtr = realloc(blockPtr, nbTotalBlocks * 2 * blockSize);
		if (newPtr != NULL)
		{
			blockPtr = (char*)newPtr;
		}
		else return -1;
		posInsert = nbTotalBlocks;
		nbTotalBlocks *= 2;
		state = FullEmpty;
		return posInsert;
	}

	if (state == Empty)
	{
		nbTotalBlocks = 2;
		blockPtr = (char*)malloc(nbTotalBlocks * blockSize);
		if (blockPtr == NULL) return -1;
		posInsert = 0;
		return posInsert;
	}
	return -1; // Error, should never reach this line
} //int blockQueue::grow()

bool blockQueueTest()
{
	blockQueue bq(4,2);
	int state;

	int a[32], tmp;

	for (int i = 0; i < 32; ++i) a[i] = i;

	state = bq.insert(&a[0], 4); // E->FE
	if (state != blockQueue::FullEmpty) return false;

	state = bq.remove(&tmp, 4); //FE->E
	if (state != blockQueue::Empty || tmp!=0) return false;

	state = bq.insert(&a[0], 4);// E->FE, already tested
	state = bq.insert(&a[1], 4);// FE->F
	if (state != blockQueue::Full ) return false;

	state = bq.remove(&tmp, 4); //F->EF
	if (state != blockQueue::EmptyFull || tmp != 0) return false;

	state = bq.insert(&a[0], 4); // EF ->F
	if (state != blockQueue::Full) return false;

	state = bq.insert(&a[2], 4); //F->FE
	if (state != blockQueue::FullEmpty) return false;

	state = bq.remove(&tmp, 4); //FE->EFE
	if (state != blockQueue::EmptyFullEmpty || tmp != 0) return false;

	state = bq.remove(&tmp, 4); //EFE->EF
	if (state != blockQueue::EmptyFullEmpty || tmp != 1) return false;

	state = bq.insert(&a[3], 4); //EFE->EF
	if (state != blockQueue::EmptyFull) return false;


	state = bq.remove(&tmp, 4); //EF->EF
	if (state != blockQueue::EmptyFull || tmp!=2 ) return false;

	state = bq.remove(&tmp, 4); //EF->E
	if (state != blockQueue::Empty || tmp != 3) return false;


	state = bq.insert(&a[0], 4);// E->FE, already tested
	state = bq.insert(&a[1], 4);// FE->FE
	state = bq.insert(&a[2], 4);// FE->FE
	state = bq.insert(&a[3], 4);// FE->F
	state = bq.remove(&tmp, 4); //F->EF
	state = bq.remove(&tmp, 4); //EF->EF
	if (state != blockQueue::EmptyFull || tmp != 1) return false;

	state = bq.insert(&a[0], 4); // EF->FEF
	if (state != blockQueue::FullEmptyFull) return false;

	state = bq.insert(&a[1], 4);// FE->FE
	if (state != blockQueue::Full) return false;

	state = bq.remove(&tmp, 4); //F->EF
	state = bq.remove(&tmp, 4); //EF->EF
	state = bq.insert(&a[0], 4);// EF->FEF
	state = bq.remove(&tmp, 4); //FEF->EFF
	if (state != blockQueue::FullEmptyFull || tmp != 2) return false;

	state = bq.remove(&tmp, 4); //FEF->FF
	if (state != blockQueue::FullEmpty || tmp != 3) return false;

	return true;
} // bool blockQueueTest()