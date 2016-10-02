#include "dataStructures.h"
#include "yieldCurve.h"
#include <fstream>
#include "mystuff\motherOfAllBaseObjects.h"

std::map <Currency, std::vector<holiday> > holidays::dates;


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
	
	if (s->size()!=6) throw "Unknown FX rate: " + *s;
	Currency fgn = toCurrency(s->substr(0, 3).c_str());
	Currency dom = toCurrency(s->substr(3).c_str());
	return FXPair(fgn,dom);
}


void* labelValue::getPointer(std::string name)
{ // similar to getObj, but returns pointer to data, or null if not found
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
