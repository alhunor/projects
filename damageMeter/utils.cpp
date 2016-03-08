#include "utils.h"
#include <string>
#include <sqlite3.h>

// GLOBAL variables
sqlite3 *db;


void trimStartingWhiteSpaces(std::string& s)
{
	while (s[0] == ' ' || s[0] == '\t')
	{
		s.erase(0, 1);
	}
}


void trimQuotes(std::string& s)
{
	while (s[0] == ' ' || s[0] == '\t')
	{
		s.erase(0, 1);
	}
	if (s[0]=='\"')
		s.erase(0,1);
	int last = s.length() - 1;
	if (s[last] == '\"')
		s.erase(last);
}


int asInt(const char* s)
{
	int i = atoi(s);
	return i;
}

float asFloat(const char* s)
{
	float f = (float)atof(s);
	return f;
}


unsigned long asuIntFromHexa(const char* s)
{
//	unsigned long int i = strtoul("0x80000000", NULL, 16);
	unsigned long int i = strtoul(s, NULL,16);
	return i;
}

bool isPrefix(const char* prefix, const char* s)
{
	const char* ptr = prefix;
	while (*ptr)
	{
		if (*ptr != *s++)
		{
			return false;
		}
		++ptr;
	}
	return true;
}


wchar_t* convert(const char* c)
{
	int i;
	int len = strlen(c);
	int wcount = 0;
	wchar_t* w = new wchar_t[len+1];
	wchar_t wc;
	for (i = 0; i < len; ++i)
	{
		if ((unsigned char)c[i] != 0xC3)
		{
			w[i - wcount] = c[i];
		}
		else
		{
			switch ((unsigned char)c[i + 1])
			{
			case 0x82:
				wc = L'Â';
				break;
			case 0x84:
				wc = L'Ä';
				break;
			case 0x89:
				wc = L'É';
				break;
			case 0xa6:
				wc = L'æ';
				break;
			case 0xa8:
				wc = L'è';
				break;
			case 0xa9:
				wc = L'é';
				break;
			case 0xae:
				wc = L'î';
				break;
			default:
				wc = L'*';
			}
			w[i - wcount] = wc;
			++i;
			wcount++;
		}
	}
	w[len - wcount] = 0;
	return w;
} // wchar_t* convert(char* c)

bool checkFlags(unsigned long int attribute, unsigned long int flags)
{
	int b = attribute & flags;
	return b != 0;
}



void saveDatabase(sqlite3 *source, const char* destination)
{
	sqlite3 *db2;
	int rc;
	rc = sqlite3_open(destination, &db2);

	sqlite3_backup* sql3b = sqlite3_backup_init(db2, "main", source, "main");

	if (sql3b == NULL)
	{
		sqlite3_close(db2);
		fprintf(stderr, "Error\n%s\n", sqlite3_errmsg(db2));
	}

	rc = sqlite3_backup_step(sql3b, -1);
	if (rc != SQLITE_DONE)
	{
		fprintf(stderr, "Error\n%s\n", sqlite3_errmsg(db2));
	}

	rc = sqlite3_backup_finish(sql3b);
	if (rc != SQLITE_OK)
	{
		printf("Error\n%s\n", sqlite3_errmsg(db2));
	}

	sqlite3_close(db2);
} // void saveDatabase(const char* destination) 

sqlite3* openDatabase(const char* dbname)
{
	char *zErrMsg = 0;
	int rc;

	const char* data = "Callback function called";

	sqlite3* db;
	rc = sqlite3_open(dbname, &db);

	if (rc)
	{
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
	}
	return db;
} // sqlite3* openDatabase(const char* dbname)



  /*

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

  char* sql = "SELECT * from COMPANY";
  rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
  if (rc != SQLITE_OK)
  {
  fprintf(stderr, "SQL error: %s\n", zErrMsg);
  sqlite3_free(zErrMsg);
  } 	else
  {
  fprintf(stdout, "SELECT Operation done successfully\n");
  }*/
