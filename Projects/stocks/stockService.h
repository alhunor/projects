// _Stocks.h: interface for the C_Stocks class.
//
//////////////////////////////////////////////////////////////////////

#ifndef STOCKSERVICE_H
#define STOCKSERVICE_H


#include <windows.h>
#include "NTService.h"

class stockService : public CNTService
{

	HANDLE	m_hStop;
public:
	stockService(char* sname, char* dname):CNTService(sname,dname) {}
	virtual void Run(DWORD, LPTSTR *);
	virtual void Stop();
};

#endif // STOCKSERVICE_H
