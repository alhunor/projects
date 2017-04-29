// history.h: interface for the history class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _HISTORY_H_INCLUDED_
#define _HISTORY_H_INCLUDED_

#include <windows.h>
#include "mysql\mysql.h"

class history  
{
public:
	history() : initialized(false) {}
	virtual bool store (int row, int col, char* value)=0;
	virtual void zap()=0; //Efface l'historique
	virtual ~history() {}
protected:
	bool initialized;
};

#endif _HISTORY_H_INCLUDED_
