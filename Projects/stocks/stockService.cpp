// _Stocks.cpp: implementation of the C_Stocks class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <winsock2.h>
#include <windows.h>
#include <winuser.h>
#include "stdafx.h"
#include "stockService.h"

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
#include "snapshotHistory.h"
#include "epoch.h"

const char* const string1="<!-- CO -->";
const char* const string2="<!-- TRANSACTIONS -->";
const char* const string3="<!-- TABLEAU COURS-->";

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




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void stockService :: Run(DWORD, LPTSTR *)
{
	ReportStatus(SERVICE_START_PENDING);
	m_hStop = ::CreateEvent(0, TRUE, FALSE, 0);
	ReportStatus(SERVICE_RUNNING);
	char* database="bourse";
	httpStream web(16384);
	char request[128];
	const char* page="/tableaux/cours_az.phtml?MARCHE=SRD&LETTRE=";
	int l=strlen(page);

	if ( !(myData=mysql_init(NULL)))
	{
		::MessageBox(0, TEXT("Error initializing the mySQL data structures."), TEXT("Stock Server"), MB_OK);
		return;
	}

	while( ::WaitForSingleObject(m_hStop, 10) != WAIT_OBJECT_0 )
	{
		while (!LaBourseEstOuverte())
		{
			Sleep(60*1000);
			if (::WaitForSingleObject(m_hStop, 10) == WAIT_OBJECT_0)
				goto end;
		};

//		cout<<"Beginning a great new day"<<endl;

		if ( mysql_real_connect( myData, NULL, NULL, NULL, NULL, MYSQL_PORT, NULL, 0 )==0)
		{
			mysql_close( myData ) ;
			//::MessageBox(0, TEXT("Can't connect to the mysql server"), TEXT("Stock Server"), MB_OK);
			return;
		};

		if ( mysql_select_db( myData, database ) < 0 )
		{
			mysql_close( myData ) ;
			//::MessageBox(0, TEXT("Can't select the <history> database"), TEXT("Stock Server"), MB_OK);
			return;
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
				//::MessageBox(0, TEXT("Error writing to DataBase."), TEXT("Stock Server"), MB_OK);
				return;
			} else
			{
				// ::MessageBox(0, TEXT("Snapshot has been written."), TEXT("Stock Server"), MB_OK);
			}
			sh.zap();
//			cout<<"Sleeping 15 min..."<<endl; 
			for (int i=0;i<15;i++) // Faut attendre 15 min
			{
				if (::WaitForSingleObject(m_hStop, 10) == WAIT_OBJECT_0)
					goto end;
				Sleep(60*1000);
			}
		};// while(LaBourseEstOuverte())
//		cout<<"This is the end of the day"<<endl;
	} // while(...)
end:
	if( m_hStop )
		::CloseHandle(m_hStop);
	mysql_close( myData );
}


void stockService :: Stop()
{
	//	"The next operation may take me up to 11 seconds. Please be patient."
	ReportStatus(SERVICE_STOP_PENDING, 61000);
	if( m_hStop )
		::SetEvent(m_hStop);
}
