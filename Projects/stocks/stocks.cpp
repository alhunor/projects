
#include "stdafx.h"
#include "stockService.h"


#ifndef _CONSOLE
	#include <stdlib.h>		// for declaration of __argv and __argc
#endif


#ifdef _CONSOLE
	int main( int argc, char ** argv ) {
#else
	int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int) {
	// non of the arguments is used when running on Win95
	int argc = __argc;
	char ** argv = __argv;
#endif	// _CONSOLE

	// create the service-object
	stockService stocks("StockServer","Stock Server");
	
	// RegisterService() checks the parameterlist for predefined switches
	// (such as -d or -i etc.; see NTService.h for possible switches) and
	// starts the services functionality.
	// You can use the return value from "RegisterService()"
	// as your exit-code.
	return stocks.RegisterService(argc, argv);
}
