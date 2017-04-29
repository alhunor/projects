// transactionHistory.cpp: implementation of the transactionHistory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <string.h>
#include <iostream.h>
#include "history.h"
#include "transaction.h"
#include "transactionHistory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

transactionHistory::transactionHistory()
{
	time=new char[32];
}

transactionHistory::~transactionHistory()
{
	delete[] time;
}


//////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////

bool transactionHistory::store (int row, int col, char* value)
{
	if (row>1)
	{
		if (col==1)
			strcpy(time,value);

		if (col==2)
			cours=atof(value);

		if (col==3)
		{
			qte=atoi(value);
			newTrans=new transaction(time,qte,cours);
			transactions.insert(newTrans);
		}
	}
	return true;
}


void transactionHistory::add(transaction* t)
{
	transactions.insert(t);
}


void transactionHistory::listNew()
{
	using namespace std;
	set<transaction*>::iterator i;
	for (i=transactions.begin();i!=transactions.end();i++)
		cout<<**i;
		

}