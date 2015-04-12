#include "ExcelConversions.h"
//#include "FileSystems.h"
#include <xlw\xlw.h>

using namespace xlw;

handleT handle; //global variable

/////////////   labelValue      ///////////////////////////////////////////////

XlfOper labelValue::toXlfOper()
{
	XlfOper	ret(data.size(), 2);
	double d;
	std::string* s;
	int i=0;

	for ( std::map<std::string, valueT>::iterator it = data.begin(); it!=data.end(); ++it)
	{
		ret.SetElement(i,0, it->first);
		valueT v = it->second;
		if (v.isString)
		{
			s = (std::string*)(v.p);
			ret.SetElement(i, 1, *s);
		} else
		{
			d = *(double*)(it->second.p);
			ret.SetElement(i, 1, d);		
		}
		++i;
	} // for
	return ret;
} // XlfOper labelValue::toXlfOper()

std::string& labelValue::getStr(std::string name)
{	
	std::map<std::string, valueT>::iterator it = data.find(name);
	if (it==data.end())
		throw "Unknown parameter name.";
	std::string* s = (std::string*)(it->second.p);
	return *s;
}


void* labelValue::getPointer(std::string name)
{ // similar to getObj, but returns pointer to data, or null  if not found
	std::map<std::string, valueT>::iterator it = data.find(name);
	if (it==data.end())
		return NULL;
	return it->second.p;
}


double labelValue::getNum(std::string name)
{	
	std::map<std::string, valueT>::iterator it = data.find(name);
	if (it==data.end())
		throw "Unknown parameter name.";
	double d = *(double*)(it->second.p);
	return d;
}

int labelValue::getHandle(std::string name)
{	// Similar to getNum, but return -1 if handle is not found
	std::map<std::string, valueT>::iterator it = data.find(name);
	if (it==data.end())
		return -1;
	double d = *(double*)(it->second.p);
	return (int)d;
}


bool labelValue::setNum(std::string name, double value)
{	
	std::map<std::string, valueT>::iterator it = data.find(name);
	if (it==data.end())
		return false;
	*(double*)(it->second.p) = value;
	return true;
}


void* labelValue::getObj(std::string name)
{	
	std::map<std::string, valueT>::iterator it = data.find(name);
	if (it==data.end())
		throw "Unknown parameter name.";
	double d = *(double*)(it->second.p);
	return handle[(int)d];
}


void labelValue::add(std::string name, std::string value)
{	
	valueT v;
	v.isString = true;
	v.p = new std::string(value);
	data[name]=v;
}

void labelValue::add(std::string name, double value)
{	
	valueT v;
	v.isString = false;
	v.p = new double(value);
	data[name]=v;
}

void labelValue::clear()
{	
	
	for (std::map<std::string, valueT>::iterator it = data.begin(); it!=data.end(); ++it)
	{
		delete it->second.p;
	}
	data.clear();
}

void labelValue::add(XlfOper xl)
{
	char *tmp;
	std::string name, value;

	if  (xl.columns()!=2)
		throw "Xloper should have exacly rwo columns.";

	for ( int i = 0; i < xl.rows(); i++)
	{
		tmp = xl(i, 0).AsString();
		name =tmp;
		if (xl(i, 1).IsString())
		{
			tmp = xl(i, 1).AsString();
			value = tmp;
			add(name, value);
		} else
		{
			double d = xl(i, 1).AsDouble();
			add(name, d);
		} // if
	} // for
} // void labelValue::add(XlfOper xl)

