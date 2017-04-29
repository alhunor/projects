#include "dbutils.h"
#include <string>
#include <sqlite3.h>

// GLOBAL variables
sqlite3 *db;




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
