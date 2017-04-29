// transaction.cpp: implementation of the transaction class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "transaction.h"
#include <string.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

transaction::transaction(char* _time, int _qte, float _cours)
{
	time= new char[32];
	strcpy(time,_time);
	qte=_qte;
	cours=_cours;

}

transaction::~transaction()
{
	delete[] time;
}


ostream& operator<<(ostream &os, const transaction& t)
{
	cout<<"A "<<t.time<<" heures "<<t.qte<<" actions au cours de "<<t.cours<<" Euros"<<endl;
	return os;
}
