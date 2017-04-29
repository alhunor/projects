#pragma once

#include <sqlite3.h>

// GLOBAL variables
extern sqlite3 *db;


void saveDatabase(sqlite3 *source, const char* destination);
sqlite3* openDatabase(const char* dbname);
