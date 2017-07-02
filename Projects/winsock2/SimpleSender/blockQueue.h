#pragma once
#include <malloc.h>
#include <memory.h>
//#define WIN32_LEAN_AND_MEAN
//#include <windows.h>
#include "myStuff/fileSystems.h"

//#include "mutex.h"

bool blockQueueTest(); // returns true if all is well



class writer
{
public:
	writer() = delete;
	writer(const writer& w) = delete;
	writer(int _pageSize, int nbInitialPages) : pageSize(_pageSize), nbPages(nbInitialPages) {}

	virtual ~writer() {};
	virtual bool open(const char* name) =0;
	virtual void close() =0;

	virtual void writePage(int pos, const void* source, int size) = 0;
	virtual void readPage(int posRemove, void* dataPtr, int size) = 0;

	virtual int grow() =0;
	int size() { return nbPages; }
protected:
	int pageSize, nbPages;
};

class memWriter : public writer
{
public:
	memWriter() = delete;
	memWriter(const memWriter& w) = delete;
	memWriter& operator=(memWriter&) = delete;

	memWriter(int pageSize, int nbInitialPages) : blockPtr(NULL), writer(pageSize, nbInitialPages) { }
	virtual ~memWriter() { close(); };
	virtual bool open(const char* name = NULL)
	{
		if (blockPtr) close();
		grow();
		return true;
	} //virtual bool open(const char* name = NULL)

	virtual void close()
	{
		if (blockPtr == NULL) return;

		free(blockPtr);
		blockPtr = NULL;
	} // virtual void close()

	virtual void writePage(int pos, const void* source, int size)
	{
		memcpy(blockPtr + pos*pageSize, source, size);
	}

	virtual void readPage(int posRemove, void* dataPtr, int size)
	{
		memcpy(dataPtr, blockPtr + posRemove*pageSize, size);
	}

	virtual int grow()
	{
		if (blockPtr == NULL) // first call
		{
			blockPtr = (char*)malloc(nbPages * pageSize);
			if (blockPtr == NULL) return -1;
		}
		else
		{	// grow already allocated space
			void* newPtr = realloc(blockPtr, nbPages * 2 * pageSize);
			if (newPtr != NULL)
			{
				blockPtr = (char*)newPtr;
				nbPages *= 2;
			}
			else return -1;
		}
		return nbPages;
	} // virtual int grow()
protected:
	char* blockPtr;
};

class fileWriter : public writer
{
public:
	fileWriter() = delete;
	fileWriter(const memWriter& w) = delete;
	fileWriter& operator=(fileWriter&) = delete;

	fileWriter(int pageSize, int nbInitialPages) : writer(pageSize, nbInitialPages) { }
	virtual ~fileWriter() { close(); };


	virtual bool open(const char* name)
	{
		if (mmf.isOpen()) close();

		bool b = mmf.create(name, pageSize*nbPages);
		//grow();
		return b;
	} //virtual bool open(const char* name = NULL)


	virtual void close()
	{
		mmf.close();
	} // virtual void close()

	virtual void writePage(int pos, const void* source, int size)
	{
		mmf.write(pos*pageSize, (const char*)source, size);
		//memcpy(ptr + pos*pageSize, source, size);
	}

	virtual void readPage(int pos, void* dataPtr, int size)
	{
		//memcpy(dataPtr, ptr + posRemove*pageSize, size);
		mmf.read(pos*pageSize, (char*) dataPtr, size);
	}


protected:
	memoryMappedFile mmf;
};


typedef enum { Empty = 0, Full, EmptyFull, EmptyFullEmpty, FullEmptyFull, FullEmpty } BQState;

template <class WRITER>
class blockQueue
{
public:
	blockQueue(int _blockSize, int nbBlocks = 2) : nbUsedBlocks(0), nbTotalBlocks(0), state(Empty), w(_blockSize, nbBlocks)
	{
		//blockPtr = (char*)malloc(nbTotalBlocks * blockSize);
	}

	int insert(const void* dataPtr, int size); // inserts an item. Returns 0 if successful, negative error code otherwise

	int remove(void* dataPtr, int size); // removes an item. Returns 0 if successful, negative error code otherwise

	int size() const { return nbUsedBlocks; }

protected:
	WRITER w;
	BQState state;
	int grow(); // returns position of next free block or -1 if error
	

	//int blockSize;
	//char* blockPtr;
	int nbUsedBlocks, nbTotalBlocks;
	int posInsert, posRemove; // positions where insert and removal is happening
	blockQueue() = delete;
	blockQueue(const blockQueue&) = delete;
	blockQueue& operator=(blockQueue&) = delete;
};

template <class WRITER>
int blockQueue<WRITER>::insert(const void* dataPtr, int size)
{
	//	int start;
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
		//		start = 0;
		//		memcpy(blockPtr + start, dataPtr, size);
		w.writePage(0, dataPtr, size);
		posInsert = 1;
		posRemove = 0;
		state = FullEmpty;
		break;
	case EmptyFull:
		w.writePage(0, dataPtr, size);
			//start = 0;
			//memcpy(blockPtr + start, dataPtr, size);
		posInsert = 1;
		if (posInsert == posRemove)
		{
			state = Full;
		}
		else state = FullEmptyFull;
		break;
	case EmptyFullEmpty:
		//start = posInsert * blockSize;
		//memcpy(blockPtr + start, dataPtr, size);
		w.writePage(posInsert, dataPtr, size);
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
		//		start = posInsert * blockSize;
		//		memcpy(blockPtr + start, dataPtr, size);
		w.writePage(posInsert, dataPtr, size);
		++posInsert;
		if (posInsert == nbTotalBlocks)
		{
			state = Full;
		}
		break;
	case FullEmptyFull:
		//		start = posInsert * blockSize;
		//		memcpy(blockPtr + start, dataPtr, size);
		w.writePage(posInsert, dataPtr, size);
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

template <class WRITER>
int blockQueue<WRITER>::remove(void* dataPtr, int size)
{
//	int start;
	switch (state)
	{
	case Empty:
		return -1; // erroe, nothing to remove
		break;
	case EmptyFull:
		//start = posRemove * blockSize;
		//memcpy(dataPtr, blockPtr + start, size);
		w.readPage(posRemove, dataPtr, size);
		++posRemove;
		if (posRemove == nbTotalBlocks)
		{
			state = Empty;
		}
		break;
	case EmptyFullEmpty:
		//start = posRemove * blockSize;
		//memcpy(dataPtr, blockPtr + start, size);
		w.readPage(posRemove, dataPtr, size);
		++posRemove;
		if (posInsert == posRemove)
		{
			state = Empty;
		}
		break;
	case Full: // Remove 1st element
		//start = 0;
		//memcpy(dataPtr, blockPtr + start, size);
		w.readPage(0, dataPtr, size);
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
		//start = 0;
		//memcpy(dataPtr, blockPtr + start, size);
		w.readPage(0, dataPtr, size);
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
		//start = posRemove * blockSize;
		//memcpy(dataPtr, blockPtr + start, size);
		w.readPage(posRemove, dataPtr, size);
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

template <class WRITER>
int blockQueue<WRITER>::grow() // returns position of next free block
{
	
	if (w.grow() < 0) return -1;

	nbTotalBlocks = w.size();

	if (state == Full)
	{
		/*		void* newPtr = realloc(blockPtr, nbTotalBlocks * 2 * blockSize);
		if (newPtr != NULL)
		{
		blockPtr = (char*)newPtr;
		} else return -1;*/
		posInsert = nbTotalBlocks;
		nbTotalBlocks *= 2;
		state = FullEmpty;
		return posInsert;
	}

	if (state == Empty)
	{
		//nbTotalBlocks = 2;
		//blockPtr = (char*)malloc(nbTotalBlocks * blockSize);
		//if (blockPtr == NULL) return -1;

		posInsert = 0;
		return posInsert;
	}
	return -1; // Error, should never reach this line
} //int blockQueue::grow()
