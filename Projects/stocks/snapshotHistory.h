// snapshotHistory.h: interface for the snapshotHistory class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _SNAPSHOTHISTORY_H_INCLUDED_
#define _SNAPSHOTHISTORY_H_INCLUDED_

#include "history.h"
#include "snapshot.h"
#include <vector>

class snapshotHistory : public history  
{
public:
	snapshotHistory();
	virtual ~snapshotHistory();
	bool store (int row, int col, char* value);
	void add(snapshot* s);
	inline void zap() {snapshots.clear();}
	bool toFile(int file);
	bool toDB(MYSQL*);
protected:
	snapshot* newSnapshot;
	time_t temps;
	char* company;
	float dernier; //en euros
	float variation; //pourcentage
	float ouverture;
	float plushaut;
	float plusbas;
	int volume;
	std::vector<snapshot*> snapshots;
};

#endif // _SNAPSHOTHISTORY_H_INCLUDED_
