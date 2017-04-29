// transactionHistory.h: interface for the transactionHistory class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _TRANSACTIONHISTORY_H_INCLUDED_
#define _TRANSACTIONHISTORY_H_INCLUDED_

#include "history.h"
#include "transaction.h"
#include <set>

class transactionHistory : public history  
{
public:
	transactionHistory();
	virtual ~transactionHistory();
	bool store (int row, int col, char* value);
	void add(transaction* t);
	void listNew();
	inline void zap() {transactions.clear();}
protected:
	transaction* newTrans;
	char* time;
	int qte;
	float cours;
	std::set<transaction*> transactions;
};

#endif // _TRANSACTIONHISTORY_H_INCLUDED_
