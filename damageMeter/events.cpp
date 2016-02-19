//#include "actors.h"
#include "events.h"
#include "mechanics.h"
#include "utils.h"
#include "..\\mystuff\\hash.h"

#include <string>
#include <iostream>
#include <sqlite3.h>

using namespace std;

// GLOBAL variables
map<int, string> spells;
guidClass guids;
extern sqlite3 *db;
#ifdef _DEBUG
int wowEvent::count = 0;
#endif

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
		sourceName = t.token(2); trim(sourceName);
		guidImpl source = guids.insert(tmp, sourceName);
		sourceGUID = source.guid;
		atype = source.type;
	}
	else
	{
		atype = Nil;
	}
	sourceRaidFlags = asuIntFromHexa(t.token(4));

	tmp = t.token(5); // destGUID
	destName = t.token(6); trim(destName);
	destFlags = asuIntFromHexa(t.token(7));
	destRaidFlags = asuIntFromHexa(t.token(8));

	if (destName != "nil")
	{
		guidImpl dest = guids.insert(tmp, destName);
		destGUID = dest.guid;
	}
}



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
	if (at == Nil)
	{
		return data[0];
	}
	GUID hash = hashString(guidString.c_str(), guidString.length());
	std::map<GUID, guidImpl>::iterator it = data.find(hash);
	if (it == data.end())
	{
		guidImpl gi;
		gi.name = name;
		gi.type = at;
		gi.guid = hash;
		gi.guidString = guidString;

		return data[hash]=gi;		
	} else
	{
		return it->second;
	}
} // guidImpl guidClass::insert(string& guid, string& name)

GUID guidClass::hash(std::string& s)
{
	return hashString(s.c_str(), s.length());
}

guidImpl& guidClass::lookUp(GUID hash)
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
		rc = sqlite3_bind_text(statement, 2, gi.name.c_str(), gi.name.length(), SQLITE_TRANSIENT);
		rc = sqlite3_bind_text(statement, 3, gi.guidString.c_str(), gi.guidString.length(), SQLITE_TRANSIENT);
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
	masterGuid = guids.insert(tmp, tmp).guid;

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
	string spellName = t.token(10); trim(spellName);
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

	t.tokenize();
	t.setTime(time_ms);
	eventName = t.token(0);

	if (strcmp(eventName, "SPELL_DAMAGE") == 0)
	{
		// mage DMG on prismatic Crystal will be filtered out laterin the damage calculation algorithm
		//if (strcmp(t.token(6), "\"Prismatic Crystal\"")!=0)
		{
			// do not count the Dmg done on the prismatic Crystal.
			eve = new spellDamage(t);
		}
	}
	else if (strcmp(eventName, "SPELL_CAST_SUCCESS") == 0)
	{
		eve = new spellCastSuccess(t);
	}
	else if (strcmp(eventName, "SPELL_AURA_APPLIED") == 0)
	{
		eve = new spellAuraApplied(t);
	} else if (strcmp(eventName, "SPELL_AURA_APPLIED_DOSE") == 0)
	{
		eve = new spellAuraAppliedDose(t);
	} else if (strcmp(eventName, "SPELL_AURA_REMOVED") == 0)
	{
		eve = new spellAuraRemoved(t);
	} else if (strcmp(eventName, "SPELL_AURA_REMOVED_DOSE") == 0)
	{
		eve = new spellAuraRemovedDose(t);
	}
	 else if (strcmp(eventName, "SPELL_AURA_BROKEN_SPELL") == 0)
	 {
		 eve = new spellAuraRemovedDose(t);
	 }
	else if (strcmp(eventName, "SPELL_AURA_REFRESH") == 0)
	{
		eve = new spellAuraRefresh(t);
	}else if (strcmp(eventName, "SPELL_AURA_BROKEN") == 0)
	{
		eve = NULL;
	}
	else if (strcmp(eventName, "SPELL_CAST_FAILED") == 0)
	{
		eve = new spellCastFailed(t);
	} else if (strcmp(eventName, "SPELL_SUMMON") == 0)
	{
		eve = new spellSummon(t);
	} else if (strcmp(eventName, "SWING_DAMAGE") == 0)
	{
		eve = new swingDamage(t);
	} else if (strcmp(eventName, "SWING_DAMAGE_LANDED") == 0)
	{
		eve = new swingDamageLanded(t);
	}  else if (strcmp(eventName, "SPELL_CAST_START") == 0)
	{
		 eve = new spellCastStart(t);
	}
	else if (strcmp(eventName, "SPELL_HEAL") == 0)
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

	else if (strcmp(eventName, "SPELL_MISSED") == 0)
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
	}
	return eve;
} // wowEvent* createEvent(const char* s, int len, tokenizer& t)


spellDamage::spellDamage(tokenizer& t) : spellEvent(t), sd(t,3), d(t,3)
{
	etype = SPELL_DAMAGE;
	char *zErrMsg = 0;
	char* sql = "INSERT INTO spellDamage (time_ms, sourceGuid, sourceFlag, sourceRaidFlag, destinationGuid, destinationFlag, destinationRaidFlag, sepllID, "\
			"masterGuid, hitPoints, maxHitPoints, attackPower, spellPower, resolve, resourceType, resourceAmount, maxResourceAmount, xPosition, yPosition, itemLevel,"\
			"dmgDone, overkill, resisted, blocked, absorbed, critical, glancing, crushing, multistrike)"\
			" values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ? ,?, ?, ?, ?, ?);";

	sqlite3_stmt *statement;

	int rc = sqlite3_prepare_v2(db, sql, -1, &statement, NULL);
	if (SQLITE_OK != rc)
	{
		wcout<<"Can't prepare insert statment, error: "<< sqlite3_errmsg(db)<<endl;
	}
	rc = sqlite3_bind_int64(statement, 1, this->time_ms);
	rc = sqlite3_bind_int(statement, 2, this->sourceGUID);
	rc = sqlite3_bind_int(statement, 3, this->sourceFlags);
	rc = sqlite3_bind_int(statement, 4, this->sourceRaidFlags);
	rc = sqlite3_bind_int(statement, 5, this->destGUID);
	rc = sqlite3_bind_int(statement, 6, this->destFlags);
	rc = sqlite3_bind_int(statement, 7, this->destRaidFlags);
	rc = sqlite3_bind_int(statement, 8, this->spellID);
// source or destination
	rc = sqlite3_bind_int(statement, 9, this->sd.masterGuid);
	//	rc = sqlite3_bind_int(statement, 10, this->targetGUID);
	rc = sqlite3_bind_int(statement, 10, this->sd.hitPoints);
	rc = sqlite3_bind_int(statement, 11, this->sd.maxHitPoints);
	rc = sqlite3_bind_int(statement, 12, this->sd.attackPower);
	rc = sqlite3_bind_int(statement, 13, this->sd.spellPower);
	rc = sqlite3_bind_int(statement, 14, this->sd.resolve);
	rc = sqlite3_bind_int(statement, 15, this->sd.resourceType);
	rc = sqlite3_bind_int(statement, 16, this->sd.resourceAmount);
	rc = sqlite3_bind_int(statement, 17, this->sd.maxResourceAmount);
	rc = sqlite3_bind_double(statement, 18, this->sd.xPosition);
	rc = sqlite3_bind_double(statement, 19, this->sd.yPosition);
	rc = sqlite3_bind_int(statement, 20, this->sd.itemLevel);
// damage
	rc = sqlite3_bind_int(statement, 21, this->d.dmgDone);
	rc = sqlite3_bind_int(statement, 22, this->d.overkill);
	rc = sqlite3_bind_int(statement, 23, this->d.resisted);
	rc = sqlite3_bind_int(statement, 24, this->d.blocked);
	rc = sqlite3_bind_int(statement, 25, this->d.absorbed);
	rc = sqlite3_bind_int(statement, 26, this->d.critical);
	rc = sqlite3_bind_int(statement, 27, this->d.glancing);
	rc = sqlite3_bind_int(statement, 28, this->d.crushing);
	rc = sqlite3_bind_int(statement, 29, this->d.multistrike);

	if (SQLITE_OK != rc)
	{
		wcout << "Can't prepare bind statment, error: " << sqlite3_errmsg(db) << endl;
	}

	if (sqlite3_step(statement) != SQLITE_DONE)
	{
		wcout << "Can't step, error: " << sqlite3_errmsg(db) << endl;
	}

//	sqlite3_reset(statement);
//	sqlite3_clear_bindings(statement);
	if (sqlite3_finalize(statement) != SQLITE_OK)
	{
		wcout << "Can't finalisestatment, error: " << sqlite3_errmsg(db) << endl;
	}


/*	if (sqlite3_exec(db, sql, 0, 0, &zErrMsg) != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
	}
	else
	{
		int i = 0;
	}*/
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
	string resourceActorString;
/*	if (atype == Pet || atype == Creature)
	{
		resourceActorString = t.token(13);
	} else if (atype == Player)
	{
		resourceActorString = t.token(12);
	} else
	{
		throw ("???");
	}
*/
	resourceActorString = t.token(13);
	if (isPrefix("000", resourceActorString.c_str()))
	{
		resourceActor = 0;
		return;
	}

	trim(resourceActorString);
	GUID hash = guids.hash(resourceActorString);
	if (checkFlags(sourceFlags, COMBATLOG_OBJECT_REACTION_HOSTILE))
	{
	//	resourceActor = guids.lookUpEnemy(hash).guid;
	}
	else
	{
		resourceActor = guids.lookUp(hash).guid;
	}
} // spellCastSuccess::spellCastSuccess(tokenizer& t)

spellCastStart::spellCastStart(tokenizer& t) : spellEvent(t)
{
	etype = SPELL_CAST_START;
}


spellCastFailed::spellCastFailed(tokenizer& t) : spellEvent(t)
{
	etype = SPELL_CAST_FAILED;
	reason = t.token(12); trim(reason); // e.g. "Invalid target"
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


