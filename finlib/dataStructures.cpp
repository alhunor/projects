#include "dataStructures.h"
#include "yieldCurve.h"
#include <fstream>

handleT handle; //global variable

std::map <Currency, std::vector<holiday> > holidays::dates;


bool handleT::exists(int h)
{
	it = handles.find(h);
	return it != handles.end();
}

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


std::string& labelValue::getStr(std::string name)
{
	it = data.find(name);
	if (it == data.end())
		throw "Missing parameter: "+ name;
	std::string* s = (std::string*)(it->second.p);
	return *s;
}


Currency labelValue::getCurrency(std::string name)
{
	it = data.find(name);
	if (it == data.end())
		throw "Missing parameter: " + name;
	std::string* s = (std::string*)(it->second.p);
	return toCurrency(s->c_str());
}

FXPair labelValue::getFXPair(std::string name)
{
	it = data.find(name);
	if (it == data.end())
		throw "Missing parameter: " + name;
	std::string* s = (std::string*)(it->second.p);
	FXPair fx;
	if (s->size()!=6) throw "Unknown FX rate: " + *s;
	fx.fgn = toCurrency(s->substr(0, 3).c_str());
	fx.dom = toCurrency(s->substr(3).c_str());
	return fx;
}



void* labelValue::getPointer(std::string name)
{ // similar to getObj, but returns pointer to data, or null  if not found
	it = data.find(name);
	if (it == data.end())
		return NULL;
	return it->second.p;
}


double labelValue::getNum(std::string name, double defaultValue)
{
	it = data.find(name);
	if (it == data.end())
	{
		if (defaultValue==0) throw "Missing parameter:" + name;
		return defaultValue;
	}
		
	double d = *(double*)((it)->second.p);
	return d;
}


int labelValue::getHandle(std::string name)
{	// Similar to getNum, but return -1 if handle is not found
	it = data.find(name);
	if (it == data.end())
		return -1;
	double d = *(double*)(it->second.p);
	return (int)d;
}


bool labelValue::setNum(std::string name, double value)
{
	it = data.find(name);
	if (it == data.end())
		return false;
	*(double*)(it->second.p) = value;
	return true;
}


void* labelValue::getObj(std::string name)
{
	it = data.find(name);
	if (it == data.end())
		throw "Missing parameter: " + name;
	double d = *(double*)(it->second.p);
	return handle[(int)d];
}


void labelValue::add(std::string name, std::string value)
{
	valueT v;
	v.isString = true;
	v.p = new std::string(value);
	data[name] = v;
}

void labelValue::add(std::string name, double value)
{
	valueT v;
	v.isString = false;
	v.p = new double(value);
	data[name] = v;
}

void labelValue::clear()
{

	for (it = data.begin(); it != data.end(); ++it)
	{
		delete it->second.p;
	}
	data.clear();
	it = data.end();
}



