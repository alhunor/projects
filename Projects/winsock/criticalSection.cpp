#include "criticalSection.h"
#include <assert.h> 


bool criticalSection::init()
{
// Initialize the critical section one time only.
	
	initialised = InitializeCriticalSectionAndSpinCount(&cs, 0x00000400);
	return initialised;
}

void criticalSection::destroy()
{
	// Release resources used by the critical section object.
	if (initialised) DeleteCriticalSection(&cs);
}


void criticalSection::enter()
{
	assert(initialised);
	// Request ownership of the critical section.
	EnterCriticalSection(&cs);
}

void criticalSection::leave()
{
	assert(initialised);
	// Release ownership of the critical section.
	LeaveCriticalSection(&cs);
}