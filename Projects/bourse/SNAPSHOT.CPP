// snapshot.cpp: implementation of the snapshot class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "snapshot.h"
#include "epoch.h"
#include <string.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

snapshot::snapshot(char* _company, float _dernier, float _variation, float _ouverture, float _plushaut, float _plusbas, int _volume):
				 dernier(_dernier),variation(_variation),ouverture(_ouverture), plushaut(_plushaut), plusbas(_plusbas),volume(_volume)
{
    temps=getEpoch();
	company=new char[strlen(_company)+1];
	strcpy(company,_company);
}

snapshot::~snapshot()
{
	delete[] company;
}

ostream& operator<<(ostream &os, const snapshot& t)
{
	
	cout<<t.company<<" ("<<t.variation<<"%) : "<<" dernier cours "<<t.dernier<<" Euros, volume "<<t.volume<<endl;
	return os;
}
