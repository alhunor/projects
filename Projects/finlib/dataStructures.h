#ifndef DataStructures_H_
#define DataStructures_H_

//#include <boost/shared_ptr.hpp>
#include <map>
#include <vector>
#include <string>
#include <boost/variant.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include "currency.h"
#include "myStuff/Matrix.h"


typedef std::vector<std::pair<std::string, VariantArray> > ResultT;

/*
class variant_type : public boost::static_visitor<variantType>
{
	variantType operator()(double d) const { return NUMERIC; }
	variantType operator()(std::string s) const { return STRING; }
};


struct MyVariant : public BoostVariantT
{
public:
	variantType tipus()
	{
		return boost::apply_visitor(variant_type(), *this);
		// vagy: return this->which();
	}
	MyVariant& operator=(double d);
	MyVariant& operator=(const std::string& s);
};

MyVariant& MyVariant::operator=(const std::string& s)
{
	BoostVariantT::operator=(s);
	return *this;
}

MyVariant& MyVariant::operator=(double d)
{
	BoostVariantT::operator=(d);
	return *this;
}
*/

typedef struct 
{
	time_t time;
	float bid, ask;
} timeStampedBidAskT;


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

/*
struct resultT
{
	MotherOfAllBaseObjects* m;
	int rows, cols;
	int h; // handle
};*/



#endif