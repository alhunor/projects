// snapshot.h: interface for the snapshot class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _SNAPSHOT_H_INCLUDED_
#define _SNAPSHOT_H_INCLUDED_

#include <time.h>

class snapshot  
{
friend ostream& operator<<(ostream &os, const snapshot& t);


public:
	snapshot(char* _company, float _dernier, float _variation, float _ouverture, float _plushaut, float _plusbas, int _volume);
	virtual ~snapshot();
	char* company;
	float dernier; //en euros
	float variation; //pourcentage
	float ouverture;
	float plushaut;
	float plusbas;
	int volume;
protected:
	time_t temps;
};

#endif // _SNAPSHOT_H_INCLUDED_
    