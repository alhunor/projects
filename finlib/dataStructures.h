#ifndef DataStructures_H_
#define DataStructures_H_

//#include <boost/shared_ptr.hpp>
#include <map>
#include <vector>
#include <string>
#include "currency.h"

//#include <windows.h>



typedef struct 
{
	time_t time;
	float bid, ask;
} timeStampedBidAskT;


/////////////   Handles      ///////////////////////////////////

class handleT
{
public:
	handleT() {n=0;}

	int get() {handles[n]=NULL; return n++;}
	int add(void* p);
	void*& operator[](int i);
	void remove(int h);
	int nbHandles() const {return n;}
	bool exists(int h);
	void clear();

private:
    handleT(handleT const&);         // Don't Implement
    void operator=(handleT const&); // Don't implement
	std::map<int, void*> handles;
	std::map<int, void*>::iterator it;
	int n;
};



struct valueT
{
	bool isString;
	void* p;
};

class labelValue
{
public:
	typedef std::map<std::string, valueT>::iterator IT;
	labelValue() { }
	~labelValue() { clear(); }
	void add(std::string name, std::string value);// {	data[name]=new std::string(value);}
	void add(std::string name, double value);
	std::string& getStr(std::string name);
	double getNum(std::string name, double defaultValue=0);
	bool setNum(std::string name, double value);
	void* getObj(std::string name);
	void* getPointer(std::string name);
	int getHandle(std::string name); // return -1 if label is not present
	Currency getCurrency(std::string name);
	FXPair getFXPair(std::string name);

	void clear();
	std::map<std::string, valueT> data;
protected:
	IT it; // iterator set by lookup(name);
};




#endif