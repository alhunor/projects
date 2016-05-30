#ifndef MotherOfAllBaseObjects_H_
#define MotherOfAllBaseObjects_H_

#include <map>


class MotherOfAllBaseObjects
{
public:
	virtual ~MotherOfAllBaseObjects() {}
	MotherOfAllBaseObjects() {}
	virtual const char* name()=0;
};


class handleT
{
public:
	handleT() { n = 0; }
	~handleT() { clear(); }

	int get() { handles[n] = NULL; return n++; }
	int add(MotherOfAllBaseObjects* p);
	void*& operator[](int i);
	void remove(int h);
	int nbHandles() const { return n; }
	bool exists(int h);
	void clear();

private:
	handleT(handleT const&) = delete;         // Don't Implement
	void operator=(handleT const&) = delete; // Don't implement
	std::map<int, void*> handles;
	std::map<int, void*>::iterator it;
	int n;
};

extern handleT handle;


#endif;