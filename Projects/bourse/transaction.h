// transaction.h: interface for the transaction class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _TRANSACTION_H_INCLUDED_
#define _TRANSACTION_H_INCLUDED_


class transaction  
{
friend ostream& operator<<(ostream &os, const transaction& t);


public:
	transaction(char* _time, int _qte, float _cours);
	virtual ~transaction();
//	transaction& operator=(const transaction&) {};
protected:
	char* time;
	int qte;
	float cours;
};

#endif // _TRANSACTION_H_INCLUDED_
