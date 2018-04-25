#include <iostream>
#include <conio.h>
#include "exported.h"

using namespace std;



void main()
{
	HMODULE H = LoadLibrary("exportingtoPython.dll");
	typedef int (*funcPtr)(int, int);
	funcPtr sum;

	if (H != NULL)
	{
		sum = (funcPtr) GetProcAddress(H, "sum");
	}
	else
	{
		cout << "Error loading DLL." << endl;
		goto _exit;
	}
	
	cout << sum(5, 10) << endl;
	FreeLibrary(H);

_exit:
	printf("Press any key to exit...");
	_getch();
}