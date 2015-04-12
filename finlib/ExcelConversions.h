#ifndef EXCELCONVERSIONS_H_
#define EXCELCONVERSIONS_H_

#include "dataStructures.h"
#include "MotherOfAllBaseObjects.h"
//#include "FileSystems.h"
#include "xlw/xlw.h"

extern handleT handle;
/////////////   labelValue      ///////////////////////////////////////////////

struct valueT
{
	bool isString;
	void* p;
};

class labelValue
{
public:
	labelValue() { }
	~labelValue(){ clear();}
	void add(std::string name, std::string value);// {	data[name]=new std::string(value);}
	void add(std::string name, double value);
	void add(xlw::XlfOper xl);
	std::string& getStr(std::string name);
	double getNum(std::string name);
	bool setNum(std::string name, double value);
	void* getObj(std::string name);
	void* getPointer(std::string name);
	int getHandle(std::string name); // return -1 if label is not present

	void clear();

	xlw::XlfOper toXlfOper();
protected:
	std::map<std::string, valueT> data;
};



#endif