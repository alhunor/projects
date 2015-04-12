#ifndef _MyError_H_
#define _MyError_H_

#include <string>

using namespace std;

class myError: public exception{
// this class is a placeholder for further developments, e.g if we want to concatenate error messages in a throw cascade
public:
	myError(const std::string& s);
	myError(const char* c);
	const char* what() {return msg.c_str();}
	std::string msg;
};

#endif