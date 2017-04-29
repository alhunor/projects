#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "utils.h"
#include "filesystems.h"
#include "quicksort.h"



using namespace std;

typedef struct 
{
	time_t time;
	float bid, ask;
} dbRecordT;


int removeQuotes(char*s, int len)
{
	int i, pos;
	pos=0;
	for (i=0; i<=len; i++)
	{
		if (s[i]=='\"')
			continue;
		if (pos==len)
			continue;
		s[pos]=s[i];
		++pos;
	}
	return --pos;
} // int removeQuotes(char*s, int len)




int processFile(const char* filename, const char* basepath)
{
	char buff[256], aux[256];
	//ifstream in;
	delimiter d;
	dbRecordT rec;

	binstream in(filename, false);
	cout<<filename<<endl;

	if (in.fail())
		return 0;
	// first line is used to determine ccy pair
again:
	in.getline(buff, 256);
	int len = in.gcount();
	removeQuotes(buff, len);
	if (d.analyse(buff, len)<4)
		return 0;
	if (d.field(0)[1]>57 || d.field(0)[1]<48)
		goto again; // ignore header line if present.

	int filednr = 0;
	if (d.field(1)[1]==0)
		filednr=1;

	strcpy(aux, basepath);
	d.field(1+filednr)[3] = '-'; //replace USD/JPY by USD-JPY;
	strcat(aux, d.field(1+filednr));
	cout<<"  "<<aux<<endl;
	strcat(aux, ".db");
	dbTable dbt(aux);
	if (!dbt.init())
	{
		throw "Error";
	}

	// save first quote from file
	rec.time = string2time(d.field(2+filednr));
	rec.bid = (float)atof(d.field(3+filednr));
	rec.ask = (float)atof(d.field(4+filednr));
	dbt.writeRecord(&rec, sizeof(rec));

	//save all other lines
	int i=1;

	while (!in.eof())
	{
		in.getline(buff,128);
		len = removeQuotes(buff, in.gcount());
		if (len<1)
			break;
		d.analyse(buff, len);
		rec.time = string2time(d.field(2+filednr));
		rec.bid = (float)atof(d.field(3+filednr));
		rec.ask = (float)atof(d.field(4+filednr));
		dbt.writeRecord(&rec, sizeof(rec));
		i++;
		if (i==95)
			i=95;
//		cout<<d.field(1)<<","<<d.field(3)<<endl;
	}
	in.close();	
	return i;
} // int processFile(const char* filename, const char* basepath)




int checkifSorted(const char* filename, const char* basepath)
{
// return number of total records if file is sorted
// or minus position of first unsorted record
	dbRecordT rec;

	binstream in(filename, false);
	cout<<filename<<endl;

	if (in.fail())
		return 0;

	int recno=1;
	time_t tt=0;
	while (!in.eof())
	{
		if (in.readblock(&rec, sizeof(dbRecordT)) <sizeof(dbRecordT) )
			break;
//		cout<<rec.time<<"  "<<rec.bid<<endl;
		
		if (rec.time<tt)
		{
			cout<<"Unsorted."<<endl;
			in.close();	
			return -recno;
		}
		tt = rec.time;		
		recno++;
	}
	in.close();	
	return recno;
} // int checkifSorted(const char* filename, const char* basepath)


bool processDir(const char* dir, const char* destination, int (*process)(const char* , const char* ))
{
	fileFinder ff;
	int i;
	if (!ff.init(dir))
	{
		cout<<"Error processing "<<dir<<endl;
		return false;
	}
	char* filename, aux[256];
	while (!ff.eof())
	{
		filename = ff.nextfile();
		if (filename)
		{
			strcpy(aux, ff.getpath());
			strcat(aux, "\\");
			strcat(aux, filename);
			i = process(aux, destination);
			cout<< "   processed "<<i<< " records."<<endl;

		}
	}
	return true;
}

void swap(dbRecordT* rec, int i, int j)
{
	dbRecordT aux = rec[i];
	rec[i] = rec[j];
	rec[j] = aux;
}

int lesss(dbRecordT a, dbRecordT b)
{
	return a.time<b.time?-1:(a.time>b.time?1:0);
}

int sortElements(const char* filename, const char* basepath)
{
	//http://msdn.microsoft.com/en-us/library/ms810613.aspx
	cout<<filename<<endl;
	int i;

	memoryMappeddbTable mmdbt;
	tm* ptm;

	dbRecordT* rec = (dbRecordT*)mmdbt.map(filename);

	if (!rec)
		return 0;

	int nbRecords = mmdbt.nFileSize / sizeof(dbRecordT);

	//for (i=1; i<nbRecords; i++)
	//{
	//	if (rec[i].time < rec[i-1].time)
	//	{
	//		ptm = gmtime ( &rec[i].time );
	//		cout<<i<<"   "<<ptm->tm_year+1900<<"."<<ptm->tm_mon+1<<"."<<ptm->tm_mday<<":"<<ptm->tm_hour<<":"<<ptm->tm_min<<":"<<ptm->tm_sec<<"  "<<rec[i].bid<<endl;
	//	}
	//}


	efficientQuickSort<dbRecordT, lesss> qs(rec);
	
	qs.sort(0, nbRecords -1);

	for (i=1; i<nbRecords; i++)
	{
		if (rec[i].time < rec[i-1].time)
		{
			ptm = gmtime ( &rec[i].time );
			cout<<i<<"   "<<ptm->tm_year+1900<<"."<<ptm->tm_mon+1<<"."<<ptm->tm_mday<<":"<<ptm->tm_hour<<":"<<ptm->tm_min<<":"<<ptm->tm_sec<<"  "<<rec[i].bid<<endl;
		}
	}

//	quicksort<dbRecordT, lesss>(rec, 0, nFileSize / sizeof(dbRecordT)-1);

// Optionally use
	mmdbt.flush();
	mmdbt.unmap();
	return nbRecords;
}

int list(const char* filename, const char* basepath)
{
	dbRecordT rec;
	struct tm * ptm;

	binstream in(filename, false);
	if (in.fail())
		return 0;

	cout<<filename<<endl;

	int recno=1;
	time_t tt=0;
	while (!in.eof())
	{
		if (in.readblock(&rec, sizeof(dbRecordT)) <sizeof(dbRecordT) )
			break;
		ptm = gmtime ( &rec.time );

		cout<<recno<<"   "<<ptm->tm_year+1900<<"."<<ptm->tm_mon+1<<"."<<ptm->tm_mday<<":"<<ptm->tm_hour<<":"<<ptm->tm_min<<":"<<ptm->tm_sec<<"  "<<rec.bid<<endl;
		
		recno++;
		if (recno==50)
			break;
	}
	in.close();	

	return 0;
}


int eliminateDuplicates(const char* filename, const char* basepath)
{
	dbRecordT rec, prev;
	char path[256];
	int len = strlen(filename);

	while (filename[len]!='\\') len--;

	binstream in(filename, false);
	strcpy(path, basepath);
	strcat(path, &filename[len+1]);

	dbTable dbt(path);

	if (in.fail())
		return 0;

	cout<<filename<<endl;

	in.readblock(&prev, sizeof(dbRecordT));
	int recno=1;

	while (!in.eof())
	{
		if (in.readblock(&rec, sizeof(dbRecordT)) <sizeof(dbRecordT) )
			break;
		if (prev.time!=rec.time)
		{
			dbt.writeRecord(&prev, sizeof(prev));
			prev=rec;
			recno++;
		}
	}
	dbt.writeRecord(&rec, sizeof(rec));

	in.close();	

	return recno;
}



void main(int argc, char** argv)
{

	//delimiter d;
	//char s[128];
	//strcpy(s, "6,USD/CHF,2001-04-01 19:07:58,1.74420000,1.74490000,,,,,,,D");
	//d.analyse(s, strlen(s)+1);


	cout<<"Processing files..."<<endl;

	//processDir("F:\\Cobra\\manual\\2000", "F:\\cobra\\processed\\2000\\", processFile);
	//processDir("F:\\Cobra\\manual\\2001", "F:\\cobra\\processed\\2001\\", processFile);
	//processDir("F:\\Cobra\\manual\\2002", "F:\\cobra\\processed\\2002\\", processFile);
	//processDir("F:\\Cobra\\manual\\2003", "F:\\cobra\\processed\\2003\\", processFile);
	//processDir("F:\\Cobra\\manual\\2004", "F:\\cobra\\processed\\2004\\", processFile);
	//processDir("F:\\Cobra\\manual\\2005", "F:\\cobra\\processed\\2005\\", processFile);
	//processDir("F:\\cobra\\manual\\2006", "F:\\cobra\\processed\\2006\\", processFile);
	//processDir("F:\\Cobra\\manual\\2007", "F:\\cobra\\processed\\2007\\", processFile);
	//processDir("F:\\Cobra\\manual\\2008", "F:\\cobra\\processed\\2008\\", processFile);
	//processDir("F:\\Cobra\\manual\\2009", "F:\\cobra\\processed\\2009\\", processFile);
	//processDir("F:\\Cobra\\manual\\2010", "F:\\cobra\\processed\\2010\\", processFile);
	//processDir("F:\\Cobra\\manual\\2011", "F:\\cobra\\processed\\2011\\", processFile);
	
	//processDir("F:\\Cobra\\processed\\2000", NULL, sortElements);
	//processDir("F:\\Cobra\\processed\\2001", NULL, sortElements);
	//processDir("F:\\Cobra\\processed\\2002", NULL, sortElements);
	//processDir("F:\\Cobra\\processed\\2003", NULL, sortElements);
	//processDir("F:\\Cobra\\processed\\2004", NULL, sortElements);
	//processDir("F:\\Cobra\\processed\\2005", NULL, sortElements);
	//processDir("F:\\Cobra\\processed\\2006", NULL, sortElements);
	//processDir("F:\\Cobra\\processed\\2007", NULL, sortElements);
	//processDir("F:\\Cobra\\processed\\2008", NULL, sortElements);
	//processDir("F:\\Cobra\\processed\\2009", NULL, sortElements);
	//processDir("F:\\Cobra\\processed\\2010", NULL, sortElements);
	//processDir("F:\\Cobra\\processed\\2011", NULL, sortElements);

	processDir("F:\\Cobra\\processed\\2000", "F:\\Cobra\\unique\\2000\\", eliminateDuplicates);
	processDir("F:\\Cobra\\processed\\2001", "F:\\Cobra\\unique\\2001\\", eliminateDuplicates);
	processDir("F:\\Cobra\\processed\\2002", "F:\\Cobra\\unique\\2002\\", eliminateDuplicates);
	processDir("F:\\Cobra\\processed\\2003", "F:\\Cobra\\unique\\2003\\", eliminateDuplicates);
	processDir("F:\\Cobra\\processed\\2004", "F:\\Cobra\\unique\\2004\\", eliminateDuplicates);
	processDir("F:\\Cobra\\processed\\2005", "F:\\Cobra\\unique\\2005\\", eliminateDuplicates);
	processDir("F:\\Cobra\\processed\\2006", "F:\\Cobra\\unique\\2006\\", eliminateDuplicates);
	processDir("F:\\Cobra\\processed\\2007", "F:\\Cobra\\unique\\2007\\", eliminateDuplicates);
	processDir("F:\\Cobra\\processed\\2008", "F:\\Cobra\\unique\\2008\\", eliminateDuplicates);
	processDir("F:\\Cobra\\processed\\2009", "F:\\Cobra\\unique\\2009\\", eliminateDuplicates);
	processDir("F:\\Cobra\\processed\\2010", "F:\\Cobra\\unique\\2010\\", eliminateDuplicates);
	processDir("F:\\Cobra\\processed\\2011", "F:\\Cobra\\unique\\2011\\", eliminateDuplicates);

	//eliminateDuplicates("F:\\cobra\\processed\\2005\\EUR-USD.db", "F:\\cobra\\unique\\2005\\");

//	list("F:\\cobra\\processed\\2005\\EUR-USD.db", NULL);
	//cout<<"Done. Press any key to exit.";
	//list("F:\\cobra\\processed\\2005\\EUR-USD2.db", NULL);
//	sortElements("F:\\cobra\\processed\\2005\\EUR-USD.db", NULL);
//	sortElements("F:\\cobra\\processed\\2000\\EUR-USD.db", NULL);
//	list("F:\\cobra\\processed\\2000\\AUD-USD.db", NULL);
//	list("F:\\cobra\\processed\\2000\\EUR-USD.db", NULL);
//	processDir("F:\\cobra\\processed\\2000", NULL, checkifSorted);
//	processDir("F:\\cobra\\processed\\2000", NULL, sortElements);

	cout<<"Done. Press any key to exit.";
	char c;
	cin>>c;
}
