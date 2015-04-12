#include "dataStructures.h"

void*& handleT::operator[](int h)
{
	it=handles.find(h);
	if (it==handles.end())
		throw "Invalid handle.";
	return it->second;
	//return handles[i];
}

void handleT::remove(int h)
{
	it=handles.find(h);
	if (it==handles.end())
		return;
	handles.erase(it);
}

int handleT::add(void* p)
{
	int h = get();
	(*this)[h] = p;
	return h;
}


void handleT::clear()
{
	handles.clear();
	n = 0;
}