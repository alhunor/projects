#ifndef _IPC_H
#define _IPC_H
#include <queue>


typedef int MUTEX;


MUTEX createMutex(char* name, bool owned);
void removeMutex(MUTEX m);

void getMutex(MUTEX m);
void releaseMutex(MUTEX m);

using namespace std;

template <typename T>
class mqueue // mutex protected wrapper around STL:queue
{
public:
	mqueue();
	~mqueue();
	mqueue(char* name);
	T* pop_front();
	void push(T* t);
	void clear();// removes every element from queue
protected:
	MUTEX m;
	queue<T*>* q;
};


template <typename T>
mqueue<T>::mqueue(char* name)
{
	m=createMutex(name, true); // creates a mutex it owns
	q = new queue<T*>();
	releaseMutex(m);
}

template <typename T>
mqueue<T>::~mqueue()
{
	getMutex(m); 
	delete q;
	//releaseMutex(m);
	removeMutex(m);
}

template <typename T>
T* mqueue<T>::pop_front()
{
	getMutex(m);
	if (!q->empty())
	{
		T* r =q->front();
		q->pop();
		return r;
	} else return NULL;
	releaseMutex(m);
}

template <typename T>
void mqueue<T>::push(T* t)
{
	getMutex(m);
	q->push(t);
	releaseMutex(m);
}

template <typename T>
void mqueue<T>::clear()
{ // removes every element from queue

	getMutex(m);
	while (	!q->empty() )
	{
		q->pop();
	}
	releaseMutex(m);
}

#endif
