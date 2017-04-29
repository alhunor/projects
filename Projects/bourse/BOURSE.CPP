// bourse.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <io.h>
#include <fcntl.h>
#include <mbstring.h>
#include <tchar.h>

#include "fileStream.h"
#include "httpStream.h"
#include "transactionHistory.h"
#include "snapshotHistory.h"
#include "epoch.h"

const char* const string1="<!-- CO -->";
const char* const string2="<!-- TRANSACTIONS -->";
const char* const string3="<!-- TABLEAU COURS-->";

transactionHistory th;
snapshotHistory sh;

#include "mysql\mysql.h"
MYSQL	* myData ;



char* myTrimHtml(char* c)
// Returns the text outside the tags ex: <font face="..."> xxxx </font>
{

	int len=strlen(c);
	char* newc=(char*)malloc(len);
	bool b=false;
	int lenc=0;
	for(int i=0;i<len;i++)
	{
		if (c[i]=='<')
		{
			b=true;
			continue;
		};
		if (c[i]=='>')
		{
			b=false;
			continue;
		};
		if (!b)
			newc[lenc++]=c[i];
	};
	newc[lenc]=0;
	return newc;
}

/* Table check */

int tableState;
int intable;
inline void initTableCheck() {tableState=0;intable=0;}
int checkTable(char c)
{
	if (intable==2)
		return 2;
	switch (tableState)
	{
	case 0:
		if (c=='<')
			tableState=1;
		break;
	case 1:
		switch(c)
		{
		case ' ':
			break;
		case 'T':
			tableState=2;
			break;
		default:
			if (intable==0)
				tableState=0;
			else
				tableState=7;
			break;
		}
		break;
	case 2:
		if (c=='A')
			tableState=3;
		else
			if (intable==0)
				tableState=0;
			else
				tableState=7;
		break;
	case 3:
		if (c=='B')
			tableState=4;
		else
			if (intable==0)
				tableState=0;
			else
				tableState=7;
		break;
	case 4:
		if (c=='L')
			tableState=5;
		else
			if (intable==0)
				tableState=0;
			else
				tableState=7;
		break;
	case 5:
		if (c=='E')
			tableState=6;
		else
			if (intable==0)
				tableState=0;
			else
				tableState=7;
		break;
	case 6:
		switch(c)
		{
		case '>':
			if (intable==1)
			{
				intable=2;
				tableState=0;
			} else
			{
				tableState=7;
			}
			break;
		case ' ':
			tableState=8;
			break;
		default:
			tableState=0;
			intable=0;
		};
		break;
	case 7:
		intable=1;
		if (c=='<')
		{
			tableState=9;
		};
		break;
	case 8:
		if (c=='>')
			tableState=7;
		break;
	case 9:
		if (c=='/')
			tableState=1;
		break;
	};
	return intable;
}

/* end: Table check */


void getTransactions(Stream* input)
{
	int state=0;
	char nc;
	int tag;
	char acc[256];
	int accl;
	int row=0;
	int col;
	int intable=0;
	initTableCheck();
//	for(int i=0;i<1650;i++)
	while (intable<2 && !input->eof())
	{	
		nc=input->get();
		if (islower(nc))
			nc=_toupper(nc);
		intable=checkTable(nc);
		switch (state)
		{
		case 0:
			if (nc=='<')
				state=1;
			break;
		case 1:
			switch(nc)
			{
			case ' ':
				break;
			case 'T':
				state=2;
				break;
			default:
				state=0;
				break;
			};
			break;
		case 2:
			switch(nc)
			{
			case 'D':
				state=3;
				tag=1;// td
				break;
			case 'R':
				state=3;
				tag=2; //tr
				break;
			default:
				state=0;
				break;
			};
			break;
		case 3:
			switch(nc)
			{
			case '>':
				state=7;
				accl=0;
				break;
			case ' ':
				state=4;
				break;
			default:
				state=0;
				break;
			};
			break;
		case 4:
			switch(nc)
			{
			case '>':
				accl=0;
				state=7;
				break;
			default:
				break;
			};
			break;
		case 7:
			if (tag==1)
			{
//				cout<<"New column."<<flush;
				col++;
				state=10;
				acc[0]=nc;
				accl=1;
			};
			if (tag==2)
			{
//				cout<<endl<<"New row."<<endl;
				row++;
				col=0;
				state=0;
			};
			break;
		case 10:
			acc[accl]=nc;
			accl++;
			acc[accl]=0;
			if ((accl>3) && strcmp(acc+accl-4,"</TD")==0)
			{
				acc[accl-4]=0;
				th.store(row,col,myTrimHtml(acc));
				state=0;
			};
			break;
		};//switch (state)
	}; //for
} //getTransactions

void getCommands(Stream* input)
{
	int state=0;
	char nc;
	int tag;
	char acc[256];
	int accl;
	int row=0;
	int col;

	int intable=0;
	initTableCheck();
	while (intable<2 && !input->eof())
	{	
		nc=input->get();
		if (islower(nc))
			nc=_toupper(nc);
		intable=checkTable(nc);
		switch (state)
		{
		case 0:
			if (nc=='<')
				state=1;
			break;
		case 1:
			switch(nc)
			{
			case ' ':
				break;
			case 'T':
				state=2;
				break;
			default:
				state=0;
				break;
			};
			break;
		case 2:
			switch(nc)
			{
			case 'D':
				state=3;
				tag=1;// td
				break;
			case 'R':
				state=3;
				tag=2; //tr
				break;
			default:
				state=0;
				break;
			};
			break;
		case 3:
			switch(nc)
			{
			case '>':
				state=7;
				accl=0;
				break;
			case ' ':
				state=4;
				break;
			default:
				state=0;
				break;
			};
			break;
		case 4:
			switch(nc)
			{
			case '>':
				accl=0;
				state=7;
				break;
			default:
				break;
			};
			break;
		case 7:
			if (tag==1)
			{
				// New column.
				col++;
				state=10;
				acc[0]=nc;
				accl=1;
			};
			if (tag==2)
			{
				// New row.
				cout<<endl<<"-----------------------"<<endl;
				row++;
				col=0;
				state=0;
			};
			break;
		case 10:
			acc[accl]=nc;
			accl++;
			acc[accl]=0;
			if ((accl>3) && strcmp(acc+accl-4,"</TD")==0)
			{
				acc[accl-4]=0;
				cout<<myTrimHtml(acc)<<" ";
				state=0;
			};
			break;
		};//switch (state)
	} //for
} //getCommands


void getCours(Stream* input)
{
	int state=0;
	char nc;
	int tag;
	char acc[256];
	int accl;
	int row=0;
	int col;

	int intable=0;
	initTableCheck();
	while (intable<2 && !input->eof())
	{	
		nc=input->get();
		if (islower(nc))
			nc=_toupper(nc);
		intable=checkTable(nc);
		switch (state)
		{
		case 0:
			if (nc=='<')
				state=1;
			break;
		case 1:
			switch(nc)
			{
			case ' ':
				break;
			case 'T':
				state=2;
				break;
			default:
				state=0;
				break;
			};
			break;
		case 2:
			switch(nc)
			{
			case 'D':
				state=3;
				tag=1;// td
				break;
			case 'R':
				state=3;
				tag=2; //tr
				break;
			default:
				state=0;
				break;
			};
			break;
		case 3:
			switch(nc)
			{
			case '>':
				state=7;
				accl=0;
				break;
			case ' ':
				state=4;
				break;
			default:
				state=0;
				break;
			};
			break;
		case 4:
			switch(nc)
			{
			case '>':
				accl=0;
				state=7;
				break;
			default:
				break;
			};
			break;
		case 7:
			if (tag==1)
			{
				// New column.
				col++;
				state=10;
				acc[0]=nc;
				accl=1;
			};
			if (tag==2)
			{
				// New row.
				row++;
				col=0;
				state=0;
			};
			break;
		case 10:
			acc[accl]=nc;
			accl++;
			acc[accl]=0;
			if ((accl>3) && strcmp(acc+accl-4,"</TD")==0)
			{
				acc[accl-4]=0;
				if (row>2 && row<24)
					sh.store(row,col,myTrimHtml(acc));
				state=0;
			};
			break;
		};//switch (state)
	} //for
} //getCours


void analyzeBourse(Stream* input)
{
	char nc;
	int state=0;
	char gs[50];
	while (!input->eof())
	{
		nc=input->get();
		nc=toupper(nc);
		if (nc=='<')
		{
			if (state==0)
			{
				state=1;
				gs[0]='<';
			};
			continue;
		};

		if (state>0 && string3[state]==nc)
		{
			gs[state]=nc;
			state++;
		} else if (state==21)
		{
			gs[state]=0;
			getCours(input);
			input->close();
			state=0;
		} else state=0;
	}; // while (!input.eof())
}

void analyzeAction(Stream* input, bool ordres,bool transactions)
{

	char nc;
	int state1=0;
	int state2=0;
	char gs1[50];
	char gs2[50];
	while (!input->eof())
	{
		nc=input->get();
		nc=toupper(nc);

		if (nc=='<')
		{

			if (state1==0)
			{
				state1=1;
				gs1[0]='<';
			};

			if (state2==0)
			{
				state2=1;
				gs2[0]='<';
			}
			continue;
		};

	if (ordres)
		if (state1>0 && string1[state1]==nc)
		{
			gs1[state1]=nc;
			state1++;
		} else if (state1==11)
		{
			gs1[state1]=0;
			getCommands(input);
			state1=0;
		} else state1=0;

	if (transactions)

		if (state2>0 && string2[state2]==nc)
		{
			gs2[state2]=nc;
			state2++;
		} else if (state2==21)
		{
			gs2[state2]=0;
			getTransactions(input);
			input->close();
			state2=0;
		} else state2=0;


	}; // while (!input.eof())
}

int listtable(char*  table)
{
	int j,k;
	char* query=(char*)malloc(strlen(table)+15);
	strcpy(query, "Select * from ");
	strcat(query,table);
	if (mysql_query( myData, query))
	{
		printf( "Couldn't execute <%s> on the server !\n",query) ;
		return 1;
	}

	MYSQL_ROW	row ;
	MYSQL_RES* res = mysql_store_result( myData ) ;
	int i = (int) mysql_num_rows( res );
	int l = 1 ;
	printf( "Number of records found:  %ld\n", i ) ;
	j = mysql_num_fields( res );
	while ( row = mysql_fetch_row( res ) )
	{
		cout<<"(";
		for ( k = 0 ; k < j-1 ; k++ )
			cout<< ((row[k]==NULL || strlen(row[k])==0  )?"NULL":row[k]) << ",";
		cout<< ((row[j-1]==NULL || strlen(row[j-1])==0  )?"NULL":row[j-1]) << ")"<<endl;
	}
	mysql_free_result( res ) ;
	return 0;
}

bool LaBourseEstOuverte()
{
	time_t ltime;
	time( &ltime );
	struct tm* now=localtime(&ltime);
	cout<<"."<<flush;
	if (now->tm_wday>0 && now->tm_wday<6 && now->tm_hour>=15 && now->tm_hour<18) //Lundi..vendredi
		return true;
	return false;
}

int main(int argc, char* argv[])
{
	char* database="bourse";
	httpStream web(16384);
	char request[128];
	const char* page="/tableaux/cours_az.phtml?MARCHE=SRD&LETTRE=";
	int l=strlen(page);

while(true)
{
	while (!LaBourseEstOuverte())
		Sleep(60*1000);

	cout<<"Beginning a great new day"<<endl;

	if ( !(myData=mysql_init(NULL)))
	{
		puts("Error initializing the mySQL data structures.");
		return 1;
	}

	if ( mysql_real_connect( myData, NULL, NULL, NULL, NULL, MYSQL_PORT, NULL, 0 )==0)
	{
		printf( "Can't connect to the mysql server on port %d !\n", MYSQL_PORT ) ;
		mysql_close( myData ) ;
		return 2;
	};

	if ( mysql_select_db( myData, database ) < 0 )
	{
		printf( "Can't select the %s database !\n", database) ;
		mysql_close( myData ) ;
		return 3;
	}

	while(LaBourseEstOuverte())
	{
		newEpoch();
		//cout<<"Taking a snapshot."<<endl; // Faudrait attendre 15 min
		for (char lettre='A';lettre<='Z';lettre++)
		{
			strcpy(request,page);
			request[l]=lettre;
			request[l+1]=0;
			web.open("www.boursorama.com",request); //www.boursorama.com
 			analyzeBourse(&web);
			web.close();
		}; // for (char lettre='A';lettre<='Z';lettre++)
		if (!sh.toDB(myData))
		{
			cerr<<"Error writing to DataBase."<<endl;
			return -1;
		} else
		{
			cout<<"Snapshot has been written."<<endl;
		}
		sh.zap();
		cout<<"Sleeping 15 min..."<<endl; // Faudrait attendre 15 min
		Sleep(15*60*1000);
	};// while(LaBourseEstOuverte())
	mysql_close( myData );
	cout<<"This is the end of the day"<<endl;
} // while(true)
return 0;
}