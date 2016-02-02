#include "actors.h"
#include "events.h"
#include "mechanics.h"
#include "utils.h"
#include <string>
#include <iostream>
using namespace std;

map<int, string> spells;

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

spellEvent::spellEvent(tokenizer& t) : wowEvent(t)
{
	spellID = asInt(t.token(9));
	string spellName = t.token(10); trim(spellName);
	spells[spellID] = spellName;
	spellSchool = asIntFromHexa(t.token(11));
}

wowEvent* createEvent(const char* s, int len, tokenizer& t)
{
	wowEvent* eve = NULL;
	const char *date, *time, *eventName;

	t.init(s, len);
	date = t.slice(' ');
	time = t.slice(' ');

	long int time_ms = (time[0] - 48) * 36000 + (time[1] - 48) * 3600 + (time[3] - 48) * 600 + (time[4] - 48) * 60 + (time[6] - 48) * 10 + (time[7] - 48); // number of seconds1
	time_ms = time_ms * 1000 + (time[9] - 48) * 100 + (time[10] - 48) * 10 + (time[11] - 48);
	t.tokenize();
	eventName = t.token(0);

	if (strcmp(eventName, "SPELL_DAMAGE") == 0)
	{
		eve = new spellDamage(t);
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
	else if (strcmp(eventName, "SPELL_AURA_REFRESH") == 0)
	{
		eve = new spellAuraRefresh(t);
	} else if (strcmp(eventName, "SPELL_CAST_FAILED") == 0)
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
	} else
	{
		std::cout << "Unknown event type: " << eventName<< endl;
	}

	if (eve)
	{
		eve->setTime(time_ms);
	}
	return eve;
} // wowEvent* createEvent(const char* s, int len, tokenizer& t)


wowEvent::wowEvent(tokenizer&t)
{
	sourceGUID = t.token(1);
	sourceName = t.token(2); trim(sourceName);
	sourceFlags = asIntFromHexa(t.token(3));
	sourceRaidFlags = asIntFromHexa(t.token(4));
	destGUID = t.token(5);
	destName = t.token(6); trim(destName);
	destFlags = asIntFromHexa(t.token(7));
	destRaidFlags = asIntFromHexa(t.token(8));
}


spellDamage::spellDamage(tokenizer& t) : spellEvent(t), sd(t,3), d(t,3)
{
	etype = SPELL_DAMAGE;

	if (sourceName == "Tyrande Whisperwind")
	{
		int a = 456;
	}
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


