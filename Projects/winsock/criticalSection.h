#ifndef criticalSection_H_H
#define criticalSection_H_H

#include <windows.h>

class criticalSection
{
public:
	criticalSection() : initialised(false) {};
	void enter();
	void leave();
	~criticalSection() { destroy(); }
	bool init();

private:
	void destroy();
	CRITICAL_SECTION cs;
	BOOL initialised;
};


class autoLocker: public criticalSection
{
public:
	autoLocker() { criticalSection::init(); criticalSection::enter(); };
	~autoLocker() { criticalSection::leave(); }
};

#endif