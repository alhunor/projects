#ifndef MotherOfAllBaseObjects_H_
#define MotherOfAllBaseObjects_H_


class MotherOfAllBaseObjects
{
public:
	virtual ~MotherOfAllBaseObjects() {}
	MotherOfAllBaseObjects() {}
	virtual const char* name()=0;
};

#endif;