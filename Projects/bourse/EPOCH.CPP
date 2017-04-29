#include "stdAfx.h"
#include "epoch.h"
#include <time.h>

static time_t epoch;
static bool inited=false;

time_t getEpoch()
{
	return epoch;
}

void newEpoch()
{
	inited=true;
	time(&epoch);
}


