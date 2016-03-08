//#include "actors.h"
#include "events.h"
#include "mechanics.h"
#include "utils.h"
#include "..\\mystuff\\hash.h"

#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <sqlite3.h>

using namespace std;

// GLOBAL variables
map<int, string> spells;
guidClass guids;
extern sqlite3 *db;
#ifdef _DEBUG
#endif


int wowEvent::SPELL_DAMAGE, wowEvent::SPELL_CAST_SUCCESS, wowEvent::SPELL_CAST_FAILED, wowEvent::SPELL_CAST_START;
int wowEvent::SWING_DAMAGE, wowEvent::SWING_DAMAGE_LANDED;
int wowEvent::SPELL_AURA_APPLIED, wowEvent::SPELL_AURA_APPLIED_DOSE, wowEvent::SPELL_AURA_REFRESH, wowEvent::SPELL_AURA_REMOVED, wowEvent::SPELL_AURA_REMOVED_DOSE;
int wowEvent::SPELL_SUMMON;
map<int, dbTable > wowEvent::dbTables;
int wowEvent::count;

void wowEvent::init()
{
	char* const sSPELL_DAMAGE = "SPELL_DAMAGE";
	char* const sSWING_DAMAGE = "SWING_DAMAGE";
	char* const sSWING_DAMAGE_LANDED = "SWING_DAMAGE_LANDED";
	char* const sSPELL_CAST_SUCCESS = "SPELL_CAST_SUCCESS";
	char* const sSPELL_CAST_FAILED = "SPELL_CAST_FAILED";
	char* const sSPELL_CAST_START = "SPELL_CAST_START";
	char* const sSPELL_AURA_APPLIED = "SPELL_AURA_APPLIED";
	char* const sSPELL_AURA_APPLIED_DOSE = "SPELL_AURA_APPLIED_DOSE";
	char* const sSPELL_AURA_REFRESH = "SPELL_AURA_REFRESH";
	char* const sSPELL_AURA_REMOVED = "SPELL_AURA_REMOVED";
	char* const sSPELL_AURA_REMOVED_DOSE = "SPELL_AURA_REMOVED_DOSE";
	char* const sSPELL_SUMMON = "SPELL_SUMMON";


	SPELL_DAMAGE = hashString(sSPELL_DAMAGE, strlen(sSPELL_DAMAGE));
	SWING_DAMAGE = hashString(sSWING_DAMAGE, strlen(sSWING_DAMAGE));
	SWING_DAMAGE_LANDED = hashString(sSWING_DAMAGE_LANDED, strlen(sSWING_DAMAGE_LANDED));
	SPELL_CAST_SUCCESS = hashString(sSPELL_CAST_SUCCESS, strlen(sSPELL_CAST_SUCCESS));
	SPELL_CAST_FAILED = hashString(sSPELL_CAST_FAILED, strlen(sSPELL_CAST_FAILED));
	SPELL_CAST_START = hashString(sSPELL_CAST_START, strlen(sSPELL_CAST_START));
	SPELL_AURA_APPLIED = hashString(sSPELL_AURA_APPLIED, strlen(sSPELL_AURA_APPLIED));
	SPELL_AURA_APPLIED_DOSE = hashString(sSPELL_AURA_APPLIED_DOSE, strlen(sSPELL_AURA_APPLIED_DOSE));
	SPELL_AURA_REFRESH = hashString(sSPELL_AURA_REFRESH, strlen(sSPELL_AURA_REFRESH));
	SPELL_AURA_REMOVED = hashString(sSPELL_AURA_REMOVED, strlen(sSPELL_AURA_REMOVED));
	SPELL_AURA_REMOVED_DOSE = hashString(sSPELL_AURA_REMOVED_DOSE, strlen(sSPELL_AURA_REMOVED_DOSE));
	SPELL_SUMMON = hashString(sSPELL_SUMMON, strlen(sSPELL_SUMMON));

	count = 0;
} // void wowEvent::init()




wowEvent::wowEvent(tokenizer&t)
{
	time_ms = t.time_ms;
	string tmp = t.token(1); // sourceGUID
	sourceFlags = asuIntFromHexa(t.token(3));
#ifndef _DEBUG
	string sourceName;
	string destName;
#else
	nr = ++count;
	if (nr == 2532)
	{
		nr = 2532;
	}
#endif // DEBUG
	if (tmp != "0000000000000000")
	{
		sourceName = t.token(2); trimQuotes(sourceName);
		guidImpl source = guids.insert(tmp, sourceName);
		sourceGUID = source.guid;
		atype = source.type;
	} else
	{
		atype = Nil;
	}
	sourceRaidFlags = asuIntFromHexa(t.token(4));

	tmp = t.token(5); // destGUID
	destName = t.token(6); trimQuotes(destName);
	destFlags = asuIntFromHexa(t.token(7));
	destRaidFlags = asuIntFromHexa(t.token(8));

	if (destName != "nil")
	{
		guidImpl dest = guids.insert(tmp, destName);
		destGUID = dest.guid;
	}
} // wowEvent::wowEvent(tokenizer&t)



bool wowEvent::readConfigFile(const char* fileName)
{

	ifstream in;
	in.open(fileName);
	if (in.bad()) return false;
	char str[64];

	int rc;
	char *zErrMsg = 0;

	int len;
	tokenizer ft;
	int found = 0;
	string tablename;
	int h;
	string sql;

	while (!in.eof())
	{
		in.getline(str, 64);
		len = (int)in.gcount();
		if (len < 1) continue;
		if (str[0] == '#') continue;
		ft.init(str, len);
		ft.tokenize(0, ' ');
		if (ft.nbTokens() == 1)
		{
			if (found > 0)
			{
				sql.append(");");
				rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);
				if (rc != SQLITE_OK)
				{
					fprintf(stderr, "SQL error: %s\n", zErrMsg);
					sqlite3_free(zErrMsg);
				}
				found = 0;
			}
			++found;
			tablename = ft.token(0);
			h = hashString(tablename.c_str(), tablename.length());
			dbTables[h].name = tablename;
			sql = "DROP table if exists ";
			sql.append(tablename.c_str());
			sql.append("; create table ");
			sql.append(tablename.c_str());
			sql.append(" (time_ms int,");
		}
		else
		{
			dbField dbf;
			dbf.fieldname = string(ft.token(0));
			trimStartingWhiteSpaces(dbf.fieldname); // eat starting tab, spaces, etc.
			dbf.fieldpos = asInt(ft.token(1));
			dbf.fieldtype = ft.token(2)[0];
			if (dbf.fieldtype == 'Q' || dbf.fieldtype == 'G')
			{
				trimQuotes(dbf.fieldname);
			}
			if (found > 1)
			{
				sql.append(",");
			}
			sql.append("  ");
			sql.append(dbf.fieldname.c_str());
			sql.append(" ");
			switch (dbf.fieldtype)
			{
			case 'I':
				sql.append("int");
				break;
			case 'H':
				sql.append("int");
				break;
			case 'Q':
				sql.append("text");
				break;
			case 'G':
				sql.append("int");
				break;
			case 'F':
				sql.append("double");
				break;
			case 'B': // there is no native support for boolean, they are stored as  0 (false) and 1 (true).
				sql.append("int");
				break;
			} // switch (dbf.fieldtype)
			dbTables[h].fields.push_back(dbf);
			//wowEvent::dbTables[tablename].fields.push_back(dbf);
			++found;
		}
	} // while (!in.eof())
	if (found > 0)
	{
		//save();
		sql.append(");");
		found = 0;
		rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}
	}
// create insert statements
	map<int, dbTable>::iterator it = dbTables.begin();
	list<dbField>::const_iterator it2;
	for (; it !=dbTables.end() ; ++it)
	{
		dbTable& tbl = it->second;
		tbl.insert = "insert into ";
		tbl.insert.append(tbl.name);
		tbl.insert.append("(time_ms");
		int ct = 0;
		for (it2 = tbl.fields.begin(); it2 != tbl.fields.end(); ++it2)
		{
			++ct;
			tbl.insert.append(",");
			tbl.insert.append(it2->fieldname);
		}
		tbl.insert.append(") Values(?");
		while (ct > 0)
		{
			tbl.insert.append(",?");
			--ct;
		}
		tbl.insert.append(");");
		//wcout << tbl.insert.c_str() << endl;

		int rc = sqlite3_prepare_v2(db, tbl.insert.c_str(), -1, &tbl.statement, NULL);
		if (SQLITE_OK != rc)
		{
			wcout << "Can't prepare insert statment, error: " << sqlite3_errmsg(db) << endl;
		}
	} // for (; it !=dbTables.end() ; ++it)
	return true;
} // bool wowEvent::readConfigFile(const char* fileName)


bool wowEvent::finalise()
{
	map<int, dbTable>::iterator it = dbTables.begin();
	for (; it != dbTables.end(); ++it)
	{
		dbTable& tbl = it->second;
		if (sqlite3_finalize(tbl.statement) != SQLITE_OK)
		{
			wcout << "Can't finalisestatment, error: " << sqlite3_errmsg(db) << endl;
		}
	} // for (; it !=dbTables.end() ; ++it)
	return true;
} // bool wowEvent::readConfigFile(const char* fileName)

bool wowEvent::savetoDB(tokenizer& t, int etype)
{
	int rc;
	sqlite3_stmt* statement;
	list<dbField>::const_iterator it;
	dbField dbf;
	int ii, hash;
	unsigned int ui;
	float f;
	string s;


	dbTable& tbl = dbTables[etype];
	statement = tbl.statement;
	if (statement == NULL)
	{
		return false;
	}
	rc = sqlite3_bind_int64(statement, 1, t.time_ms);
	for (it = tbl.fields.begin(); it != tbl.fields.end(); ++it)
	{
		dbf = *it;
		if (dbf.fieldtype == 'H')
		{
			ui = asuIntFromHexa(t.token(dbf.fieldpos));
			rc = sqlite3_bind_int(statement, dbf.fieldpos, ui);
		}
		else if (dbf.fieldtype == 'I')
		{
			ii = asInt(t.token(dbf.fieldpos));
			rc = sqlite3_bind_int(statement, dbf.fieldpos, ii);
		} else if (dbf.fieldtype == 'G')
		{
			s = t.token(dbf.fieldpos);
			trimQuotes(s);
			hash = hashString(s.c_str(), s.length());
			rc = sqlite3_bind_int(statement, dbf.fieldpos, hash);
		}
		else if (dbf.fieldtype == 'B')
		{
			ii = asInt(t.token(dbf.fieldpos));
			rc = sqlite3_bind_int(statement, dbf.fieldpos, ii);

		} else if (dbf.fieldtype == 'Q')
		{
			s = t.token(dbf.fieldpos);
			trimQuotes(s);
			rc = sqlite3_bind_text(statement, dbf.fieldpos, s.c_str(), s.length(), SQLITE_TRANSIENT);
		} else if (dbf.fieldtype == 'F')
		{
			f = asFloat(t.token(dbf.fieldpos));
			rc = sqlite3_bind_double(statement, dbf.fieldpos, f);
		}
	} // for (it = tbl.fields.begin(); it != tbl.fields.end(); ++it)

	rc = sqlite3_step(statement);
	if ( rc!= SQLITE_DONE)
	{
		wcout << "Can't step, error: " << sqlite3_errmsg(db) << endl;
	}
	sqlite3_reset(statement);
//	sqlite3_clear_bindings(statement);
	return true;
} // bool wowEvent::savetoDB(tokenizer& t, int etype)


atype actorType(string& s)
{
	atype at = Invalid;

	if (isPrefix("Player", s.c_str()))
	{
		return Player;
	}

	if (isPrefix("Pet", s.c_str()))
	{
		return Pet;
	}

	if (isPrefix("Creature", s.c_str()))
	{
		return Creature;
	}

	if (isPrefix("Item", s.c_str()))
	{
		return Item;
	}

	if (isPrefix("Vehicle", s.c_str()))
	{
		return Vehicle;
	}

	if (isPrefix("0000", s.c_str()))
	{
		return Nil;
	}

	return Invalid;
} // atype actortype(string* s)



guidImpl& guidClass::insert(string& guidString, string& name)
{
	atype at = actorType(guidString);
	data[0].type = at;
	if (at == Nil)
	{
		return data[0];
	}
	if (at >Vehicle)
	{
		return data[0];
	}
	GUID hashcode = hash(guidString, at);
	std::map<GUID, guidImpl>::iterator it = data.find(hashcode);
	if (it == data.end())
	{
		guidImpl gi;
		gi.name = name;
		gi.type = at;
		gi.guid = hashcode;
		gi.guidString = guidString;

		return data[hashcode]=gi;		
	} else
	{
		return it->second;
	}
} // guidImpl guidClass::insert(string& guid, string& name)

GUID guidClass::hash(std::string& s, atype at)
{
	int len = s.length();
	return hashString(s.c_str(), len);
}


guidImpl& guidClass::lookUpPet(GUID hash)
{
	std::map<GUID, guidImpl>::iterator it = data.find(hash);
	if (it == data.end())
	{
		guidImpl gi;
		gi.name = "";
		gi.type = Invalid; // found a pet without a known owner. Record the event as the owner may be found later
		gi.guid = hash;
		return data[hash] = gi;
	} else
	{
		return it->second;
	}
} //guidImpl& guidClass::lookUp(GUID hash)


void guidClass::save()
{
	char *zErrMsg = 0;
	sqlite3_stmt *statement;

	char* sql = "DROP TABLE if exists Guids; CREATE TABLE guids("  \
		"GUID			INT PRIMARY KEY," \
		"guidString		TEXT not null," \
		"name			VARCHAR(64)," \
		"typ			int);";

	int rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "Error in CREATE TABLE: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	};
	std::map<GUID, guidImpl>::iterator it = data.begin();
	
	sql = "INSERT INTO guids values ( ?, ?, ?, ?);";

	rc = sqlite3_prepare_v2(db, sql, -1, &statement, NULL);
	if (SQLITE_OK != rc)
	{
		wcout << "Can't prepare insert statment, error: " << sqlite3_errmsg(db) << endl;
	}
	guidImpl gi;
	for (; it != data.end(); ++it)
	{
		gi = it->second;
		rc = sqlite3_bind_int(statement, 1, gi.guid);
		rc = sqlite3_bind_text(statement, 2, gi.guidString.c_str(), gi.guidString.length(), SQLITE_TRANSIENT);
		rc = sqlite3_bind_text(statement, 3, gi.name.c_str(), gi.name.length(), SQLITE_TRANSIENT);
		rc = sqlite3_bind_int(statement, 4, gi.type);

		if (sqlite3_step(statement) != SQLITE_DONE)
		{
			wcout << "Can't step, error: " << sqlite3_errmsg(db) << endl;
		}

		sqlite3_reset(statement);
		//sqlite3_clear_bindings(statement);
	} // for (; it != data.end(); ++it)
	if (sqlite3_finalize(statement) != SQLITE_OK)
	{
		wcout << "Can't finalisestatment, error: " << sqlite3_errmsg(db) << endl;
	}
} // guidClass::save()





damage::damage(tokenizer& t, int offset)
{
	dmgDone = asInt(t.token(22+offset));
	overkill = asInt(t.token(23+offset));
	magicSchool = asInt(t.token(24+offset));
	resisted = asInt(t.token(25 + offset));
	blocked = asInt(t.token(26 + offset));
	absorbed = asInt(t.token(27 + offset));
	critical = asInt(t.token(28 + offset));
	glancing = asInt(t.token(29 + offset));
	crushing = asInt(t.token(30 + offset));
	multistrike = asInt(t.token(31 + offset));
}

sourceOrDestination::sourceOrDestination(tokenizer& t, int offset)
{

	std::string tmp = t.token(9+offset);
	targetGuid = guids.insert(tmp, tmp).guid;
	tmp = t.token(10+offset);
	
	// this line is empirical, hate the lack of clear spec
	if (actorType(tmp) == Player)
	{
		masterGuid = guids.insert(tmp, tmp).guid;
	}
	else
	{
		masterGuid = 0;
	}

	hitPoints = asInt(t.token(11 + offset));
	maxHitPoints = asInt(t.token(12 + offset));

	attackPower  = asInt(t.token(13 + offset));
	spellPower = asInt(t.token(14 + offset));
	resolve = asInt(t.token(15 + offset));
	resourceType = asInt(t.token(16 + offset));
	resourceAmount = asInt(t.token(17 + offset));
	maxResourceAmount = asInt(t.token(18 + offset));

	xPosition = asFloat(t.token(19 + offset)); //22
	yPosition = asFloat(t.token(20 + offset)); //23
	itemLevel = asInt(t.token(21 + offset)); // 24
}

unsigned long int epoch = 0;
unsigned long int maxtime = 0;


spellEvent::spellEvent(tokenizer& t) : wowEvent(t)
{
	spellID = asInt(t.token(9));
	string spellName = t.token(10); trimQuotes(spellName);
	spells[spellID] = spellName;
	spellSchool = asuIntFromHexa(t.token(11));
}

wowEvent* createEvent(const char* s, int len, tokenizer& t)
{
	wowEvent* eve = NULL;
	const char *date, *time, *eventName;

	t.init(s, len);
	date = t.slice(' ');
	time = t.slice(' ');

//	unsigned long int time_ms = (time[0] - 48) * 36000 + (time[1] - 48) * 3600 + (time[3] - 48) * 600 + (time[4] - 48) * 60 + (time[6] - 48) * 10 + (time[7] - 48); // number of seconds1
	unsigned long int time_ms = epoch * 3600 + (time[3] - 48) * 600 + (time[4] - 48) * 60 + (time[6] - 48) * 10 + (time[7] - 48); // number of seconds
	time_ms = time_ms * 1000 + (time[9] - 48) * 100 + (time[10] - 48) * 10 + (time[11] - 48);
	if (time_ms > maxtime)
	{
		maxtime = time_ms;
	}
	if (time_ms < maxtime)
	{
		++epoch;
		time_ms += 3600;
	}

	t.tokenize(1, ',');
	t.setTime(time_ms);
	eventName = t.token(0);

	int hash = hashString(eventName, strlen(eventName));
	if (hash == wowEvent::SPELL_DAMAGE)
	{
		// mage DMG on prismatic Crystal will be filtered out laterin the damage calculation algorithm
		//if (strcmp(t.token(6), "\"Prismatic Crystal\"")!=0)
		{
			// do not count the Dmg done on the prismatic Crystal.
			eve = new spellDamage(t);
		}
	}
	else if (hash == wowEvent::SPELL_CAST_SUCCESS)
	{
		eve = new spellCastSuccess(t);
	}
	else if (hash == wowEvent::SPELL_AURA_APPLIED)
	{
		eve = new spellAuraApplied(t);
	} else if (hash == wowEvent::SPELL_AURA_APPLIED_DOSE)
	{
		eve = new spellAuraAppliedDose(t);
	} else if (hash == wowEvent::SPELL_AURA_REMOVED)
	{
		eve = new spellAuraRemoved(t);
	} else if (hash == wowEvent::SPELL_AURA_REMOVED_DOSE)
	{
		eve = new spellAuraRemovedDose(t);
	 } else if (hash == wowEvent::SPELL_AURA_REFRESH)
	{
		eve = new spellAuraRefresh(t);
	} else if (hash == wowEvent::SPELL_CAST_FAILED)
	{
		eve = new spellCastFailed(t);
	} else if (hash == wowEvent::SPELL_SUMMON)
	{
		eve = new spellSummon(t);
	} else if (hash == wowEvent::SWING_DAMAGE)
	{
		eve = new swingDamage(t);
	} else if (hash == wowEvent::SWING_DAMAGE_LANDED)
	{
		eve = new swingDamageLanded(t);
	}  else if (hash == wowEvent::SPELL_CAST_START)
	{
		 eve = new spellCastStart(t);
	} else if (strcmp(eventName, "SPELL_HEAL") == 0)
	{
		wowEvent::count++;
		eve = NULL;
	} else if (strcmp(eventName, "SPELL_PERIODIC_HEAL") == 0)
	{
		wowEvent::count++;
		eve = NULL;
	}
	else if (strcmp(eventName, "SPELL_PERIODIC_DAMAGE") == 0)
	{
		wowEvent::count++;
		eve = NULL;
	} else if (strcmp(eventName, "SPELL_PERIODIC_ENERGIZE") == 0)
	{
		wowEvent::count++;
		eve = NULL;
	}
	else if (strcmp(eventName, "SPELL_PERIODIC_MISSED") == 0)
	{
		wowEvent::count++;
		eve = NULL;
	}
	else if (strcmp(eventName, "SPELL_AURA_BROKEN_SPELL") == 0)
	{
		wowEvent::count++;
		eve = NULL;
	} 	else if (strcmp(eventName, "SPELL_AURA_BROKEN") == 0)
	{
		wowEvent::count++;
		eve = NULL;
	} else if (strcmp(eventName, "SPELL_MISSED") == 0)
	{
		wowEvent::count++;
		eve = NULL;
	}
	else if (strcmp(eventName, "SPELL_ABSORBED") == 0)
	{
		wowEvent::count++;
		eve = NULL;
	}
	else if (strcmp(eventName, "SPELL_INTERRUPT") == 0)
	{
		wowEvent::count++;
		eve = NULL;
	}

	else if (strcmp(eventName, "SWING_MISSED") == 0)
	{
		wowEvent::count++;
		eve = NULL;
	}
	else if (strcmp(eventName, "RANGE_DAMAGE") == 0)
	{
		wowEvent::count++;
		eve = NULL;
	}
	else if (strcmp(eventName, "SPELL_ENERGIZE") == 0)
	{
		wowEvent::count++;
		eve = NULL;
	} else if (strcmp(eventName, "UNIT_DESTROYED") == 0)
	{
		wowEvent::count++;
		eve = NULL;
	} else if (strcmp(eventName, "UNIT_DIED") == 0)
	 {
		 wowEvent::count++;
		 eve = NULL;
	 }
	else if (strcmp(eventName, "PARTY_KILL") == 0)
	{
		wowEvent::count++;
		eve = NULL;
	}
	else if (strcmp(eventName, "SPELL_CREATE") == 0)
	{
		wowEvent::count++;
		eve = NULL;
	}
	else if (strcmp(eventName, "SPELL_INSTAKILL") == 0)
	{
		wowEvent::count++;
		eve = NULL;
	}
	else if (strcmp(eventName, "ENCOUNTER_START") == 0)
	{
		wowEvent::count++;
		eve = NULL;
	}
	else if (strcmp(eventName, "ENCOUNTER_END") == 0)
	{
		wowEvent::count++;
		eve = NULL;
	}
	else if (strcmp(eventName, "SPELL_RESURRECT") == 0)
	{
		wowEvent::count++;
		eve = NULL;
	} else
	{
		wowEvent::count++;
		std::cout << "Unknown event type: " << eventName<< endl;
		return NULL;
	}
	if (eve)
	{
		wowEvent::savetoDB(t, eve->etype);
	}
// Generate DB code to save the event

	return eve;
} // wowEvent* createEvent(const char* s, int len, tokenizer& t)


spellDamage::spellDamage(tokenizer& t) : spellEvent(t), sd(t,3), d(t,3)
{
	etype = SPELL_DAMAGE;

} // spellDamage::spellDamage(tokenizer& t)

swingDamage::swingDamage(tokenizer& t) : wowEvent(t), sd(t,0), d(t,0)
{
	etype = SWING_DAMAGE;

} // swingDamage::swingDamage(tokenizer& t)

swingDamageLanded ::swingDamageLanded(tokenizer& t) : wowEvent(t), sd(t, 0)
{
	etype = SWING_DAMAGE_LANDED;
}


spellCastSuccess::spellCastSuccess(tokenizer& t) : spellEvent(t), sd(t, 3)
{
	etype = SPELL_CAST_SUCCESS;
/*	string resourceActorString;

	resourceActorString = t.token(13);
	if (isPrefix("000", resourceActorString.c_str()))
	{
		//resourceActor = 0;
		return;
	}

	trimQuotes(resourceActorString);
	GUID hash = guids.hash(resourceActorString);
	if (checkFlags(sourceFlags, COMBATLOG_OBJECT_REACTION_HOSTILE))
	{
	//	resourceActor = guids.lookUpEnemy(hash).guid;
	} else
	{
		//resourceActor = guids.lookUp(hash).guid;
	}
	*/
} // spellCastSuccess::spellCastSuccess(tokenizer& t)

spellCastStart::spellCastStart(tokenizer& t) : spellEvent(t)
{
	etype = SPELL_CAST_START;
}


spellCastFailed::spellCastFailed(tokenizer& t) : spellEvent(t)
{
	etype = SPELL_CAST_FAILED;
	reason = t.token(12); trimQuotes(reason); // e.g. "Invalid target"
}

spellAuraApplied::spellAuraApplied(tokenizer& t) : spellEvent(t)
{
	etype = SPELL_AURA_APPLIED;
	buffOrDebuff = t.token(12);// Contains "BUFF" od "DEBUFF"
}

spellAuraRemoved::spellAuraRemoved(tokenizer& t) : spellEvent(t)
{
	etype = SPELL_AURA_REMOVED; 
	buffOrDebuff = t.token(12);// Contains "BUFF" od "DEBUFF"
}

spellAuraRefresh::spellAuraRefresh(tokenizer& t) : spellEvent(t)
{
	etype = SPELL_AURA_REFRESH;
	buffOrDebuff = t.token(12);// Contains "BUFF" od "DEBUFF"
}


spellAuraAppliedDose::spellAuraAppliedDose(tokenizer& t) : spellAuraApplied(t)
{
	etype = SPELL_AURA_APPLIED_DOSE;
	amount = asInt(t.token(13));
}

spellAuraRemovedDose::spellAuraRemovedDose(tokenizer& t) : spellAuraRemoved(t)
{
	etype = SPELL_AURA_REMOVED_DOSE;
	amount = asInt(t.token(13));
}


spellSummon::spellSummon(tokenizer& t) : spellEvent(t)
{
	etype = SPELL_SUMMON;
}
