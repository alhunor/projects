#include <algorithm>
#include <conio.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <io.h>
#include <map>
#include <string>


#include "actors.h"
#include "events.h"
#include "..\\mystuff\\hash.h"
#include "tokenizer.h"


#define MAX_LEN 2048

extern guidClass guids;

using namespace std;


static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
	int i;
	for (i = 0; i<argc; i++)
	{
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

void main()
{
	ifstream in;
	in.open("events.txt");
	char str[64];
	while (!in.eof() && !in.bad())
	{

	}

// http://www.tutorialspoint.com/sqlite/sqlite_c_cpp.htm
	char *zErrMsg = 0;
	int rc;
	const char* data = "Callback function called";

	rc = sqlite3_open("wow.db", &db);
//	rc = sqlite3_open(":memory:", &db);

	if (rc)
	{
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		exit(0);
	} else
	{
		fprintf(stderr, "Opened database successfully\n");
	}
	char* sql = "DROP table if exists Company; CREATE TABLE COMPANY("  \
		"ID INT PRIMARY KEY     NOT NULL," \
		"NAME           TEXT    NOT NULL," \
		"AGE            INT     NOT NULL," \
		"ADDRESS        CHAR(50)," \
		"SALARY         REAL );";

	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	} else
	{
		fprintf(stdout, "Table created successfully\n");
	}

	sql = "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) "  \
		"VALUES (1, 'Paul', 32, 'California', 20000.00 ); " \
		"INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) "  \
		"VALUES (2, 'Allen', 25, 'Texas', 15000.00 ); "     \
		"INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)" \
		"VALUES (3, 'Teddy', 23, 'Norway', 20000.00 );" \
		"INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)" \
		"VALUES (4, 'Mark', 25, 'Rich-Mond ', 65000.00 );";

	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	} else
	{
		fprintf(stdout, "Records created successfully\n");
	}

	sql = "SELECT * from COMPANY";

	rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	} 	else 
	{
		fprintf(stdout, "SELECT Operation done successfully\n");
	}

	sql = "DROP TABLE if exists SpellDamage; CREATE TABLE SpellDamage("  \
		"ID						INT PRIMARY KEY," \
		"time_ms				unsigned big int NOT NULL," \
		"sourceGuid				INT NOT NULL," \
		"sourceFlag				int not null," \
		"sourceRaidFlag         int not null,"\
		"destinationGuid        int not null,"\
		"destinationFlag        int not null," \
		"destinationRaidFlag    int not null," \
		"SepllID				int not null," \
		//	"targetGuid				int not null," 
		"masterGuid				int not null," \
		"hitPoints				int not null," \
		"maxHitPoints			int not null," \
		"attackPower			int not null," \
		"spellPower				int not null," \
		"resolve				int not null," \
		"resourceType			int not null," \
		"resourceAmount			int not null," \
		"maxResourceAmount      int not null,"\
		"xPosition				REAL not null," \
		"yPosition				REAL not null," \
		"itemLevel				int not null," \
		"dmgDone				int not null," \
		"overkill				bool not null," \
		"resisted				int not null," \
		"blocked				int not null," \
		"absorbed				int not null," \
		"critical				bool not null," \
		"glancing				bool not null," \
		"crushing				bool not null," \
		"multistrike			bool not null);";

	rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "Error in CREATE TABLE SpellDamage: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	};

// https://www.sqlite.org/c_interface.html

	_setmode(_fileno(stdout), _O_U16TEXT);

	ifstream in;
	char dest[MAX_LEN];
	int count, count2;
//	in.open("archi.txt");
//	in.open("arcane full 2.txt");
/*	char * c = "æthushu";
	wchar_t* w = convert(c);
	//std::wcout << c << std::endl;
	std::wcout << w << std::endl;
*/
	//in.open("archi-log.txt");
	in.open("arcane3.txt"); // with prismatic crytal in garrison
	//in.open("frost.txt"); // with frozen orbs in garrison
	if (in.bad())
	{
		cout << "Error opening input file." << endl;
	}
	tokenizer t;
	count = 1;
	actors actors;
	while (!in.eof())
	{
		in.getline(dest, MAX_LEN);
		count2 = (int)in.gcount();

		if (count2==MAX_LEN)
		{
			throw "Line is too long";
		}
		if (count2 == 0)
		{
			break;
		}
		wowEvent * eve = createEvent(dest, count2, t);
		if (eve)
		{
			actors.add(eve);
		}
		/*cout << count++ << " -> ";
		for (i=0; i<t.nbTokens()-1; ++i)
		{
			cout<<t.token(i)<<"  ";
		
		cout<<endl;
		}*/
		++count;
		//wcout << count << endl;
		if (count >= 645)
	//	if (count >= 25959)
			{
			//wcout << count << endl;
		}
	}
	actors.stat();

	mage* kettle = (mage*)actors.locate("Player-1587-073AE460"); // Kettle-Arathor
	vector<int> v;

	wcout << "Battle between " << actors.minTime << " - " <<actors.maxTime << endl;

	kettle->computeDamageTimeline(v, actors.minTime, actors.maxTime, 100);
	
	guids.save();
	sqlite3_close(db);

	wcout<<"Press any key to exit..";
	_getch();
}