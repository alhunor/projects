#include "MotherOfAllBaseObjects.h"

handleT handle; //global variable


bool handleT::exists(int h)
{
	it = handles.find(h);
	return it != handles.end();
}

void*& handleT::operator[](int h)
{
	it = handles.find(h);
	if (it == handles.end())
		throw "Invalid handle.";
	return it->second;
	//return handles[i];
}

void handleT::remove(int h)
{
	it = handles.find(h);
	if (it == handles.end())
		return;
	handles.erase(it);
}

int handleT::add(MotherOfAllBaseObjects* p)
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