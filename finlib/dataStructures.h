#ifndef DataStructures_H_
#define DataStructures_H_

//#include <boost/shared_ptr.hpp>
#include <map>
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
	void clear();

private:
    handleT(handleT const&);         // Don't Implement
    void operator=(handleT const&); // Don't implement
	std::map<int, void*> handles;
	std::map<int, void*>::iterator it;
	int n;
};




#endif