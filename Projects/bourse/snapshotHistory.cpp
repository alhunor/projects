// snapshotHistory.cpp: implementation of the snapshotHistory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <string.h>
#include <time.h>
#include <iostream.h>
#include <io.h>
#include "history.h"
#include "snapshot.h"
#include "snapshotHistory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

snapshotHistory::snapshotHistory()
{
	company=new char[32];
}

snapshotHistory::~snapshotHistory()
{
	delete[] company;
}


//////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////

struct snapshotRec
{
	char company[30];
	float dernier; //en euros
	float variation; //pourcentage
	float ouverture;
	float plushaut;
	float plusbas;
	int volume;
} recording[10];


bool snapshotHistory::toFile(int file)
{
	using namespace std;
	int nr=0;
	for (int i=0;i<snapshots.size();i++)
	{
		//cout<<snapshots[i]->company<<endl;
		strcpy(recording[nr].company,snapshots[i]->company);
		recording[nr].dernier=snapshots[i]->dernier;
		recording[nr].variation=snapshots[i]->variation;
		recording[nr].ouverture=snapshots[i]->ouverture;
		recording[nr].plushaut=snapshots[i]->plushaut;
		recording[nr].plusbas=snapshots[i]->plusbas;
		recording[nr].volume=snapshots[i]->volume;
		if (++nr==10)
		{
			if ( write(file,recording, sizeof(snapshotRec)*10)==-1)
				return false;
			nr=0;
		}

	};

	if (nr>1)
		if (write(file,recording, sizeof(snapshotRec)*(nr-1))==-1)
			return false;
	_commit(file);
	return true;
}


char* dtoa(double d, int count)
{
	int dec,sign;
	char* string=_fcvt(d,count,&dec,&sign);
//	cout<<string<<endl;
	int len=strlen(string);
	char *result=(char*)malloc(len+1+1+1);
	if (sign!=0) //d was negativ
	{
		strcpy(result,string);
		strcpy(string,"-");
		strcat(string,result);
		dec++;
	};
	if (count==0) 
	{
		if (len==0)
				strcpy(result,"0");
			else
				strcpy(result,string);
		return result;
	};
	strnset(result,0,1); //initializing
//	cout<<sign<<endl;

/*	if (dec==0 || (sign!=0 && dec==1)) // no decimal part
	{
		strcpy(result,string);
		return result;
	};
*/
	if (dec<=1 && sign!=0) //abs(d)<1, d<0
	{
		dec--;
		if (-count>dec)
			dec=-count;
		strcpy(result,"-0.");
		while (dec<0)
		{
			strcat(result,"0");
			dec++;
		};
		string++;
		strcat(result,string);
		return result;
	};

	if (dec>0) //d>1
	{
		strncat(result,&string[0],dec);
		while (dec>0)
		{
			string++;
			dec--;
		};
		strcat(result,".");
		strcat(result,string);
		return result;
	};

	// abs(d)<1 && d>0
	if (-count>dec)
		dec=-count;
	strcat(result,"0.");
	while (dec<0)
	{
		strcat(result,"0");
		dec++;
	};
	strcat(result,string);
	return result;
}

bool snapshotHistory::toDB(MYSQL* sgbd)
{
	time_t ltime;
	time( &ltime );
	struct tm* now=localtime(&ltime);

	using namespace std;
	char sql[512]; //that should be  enough!!!
	char* tmp=new char[16];
	for (int i=0;i<snapshots.size();i++)
	{
		strcpy(sql,"INSERT into historique values('");
		strcat(sql,snapshots[i]->company);
		strcat(sql,"',");

		strcat(sql,dtoa(snapshots[i]->dernier,2));
		strcat(sql,",");
		strcat(sql,dtoa(snapshots[i]->variation,2));
		strcat(sql,",");
		strcat(sql,dtoa(snapshots[i]->ouverture,2));
		strcat(sql,",");
		strcat(sql,dtoa(snapshots[i]->plushaut,2));
		strcat(sql,",");
		strcat(sql,dtoa(snapshots[i]->plusbas,2));
		strcat(sql,",");
		strcat(sql,dtoa(snapshots[i]->volume,2));
		strcat(sql,",'");
		itoa(now->tm_year+1900,tmp,10);
		strcat(sql,tmp);
		strcat(sql,"/");
		itoa(now->tm_mon,tmp,10);
		strcat(sql,tmp);
		strcat(sql,"/");
		itoa(now->tm_mday,tmp,10);
		strcat(sql,tmp);
		strcat(sql,"','");
		itoa(now->tm_hour,tmp,10);
		strcat(sql,tmp);
		strcat(sql,":");
		itoa(now->tm_min,tmp,10);
		strcat(sql,tmp);
		strcat(sql,":00')");
		mysql_query(sgbd, sql);
	};
	return true;
}



bool snapshotHistory::store (int row, int col, char* value)
{
	if (row>2)
	{
		if (col==1)
			strcpy(company,value);

		if (col==2)
			dernier=atof(value);

		if (col==3)
			variation=atof(value);

		if (col==5)
			ouverture=atof(value);

		if (col==6)
			plushaut=atof(value);

		if (col==7)
			plusbas=atof(value);

		if (col==8)
		{
			volume=atoi(value);
			newSnapshot=new snapshot(company, dernier, variation, ouverture, plushaut, plusbas, volume);
			snapshots.push_back(newSnapshot);
		}


	}
	return true;
}


void snapshotHistory::add(snapshot* s)
{
	snapshots.push_back(s);
}


void snapshotHistory::listNew()
{
	using namespace std;
	for (int i=0;i<snapshots.size();i++)
		cout<<*snapshots[i];
}