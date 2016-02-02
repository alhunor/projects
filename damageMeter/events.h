#pragma once
#include "tokenizer.h"
#include "mechanics.h"
//#include "actors.h"
#include <string>

//Type
#define COMBATLOG_OBJECT_TYPE_MASK     0x0000FC00
#define COMBATLOG_OBJECT_TYPE_OBJECT   0x00004000
#define COMBATLOG_OBJECT_TYPE_GUARDIAN 0x00002000
#define COMBATLOG_OBJECT_TYPE_PET      0x00001000
#define COMBATLOG_OBJECT_TYPE_NPC      0x00000800
#define COMBATLOG_OBJECT_TYPE_PLAYER   0x00000400
//Controller
#define COMBATLOG_OBJECT_CONTROL_MASK   0x00000300
#define COMBATLOG_OBJECT_CONTROL_NPC    0x00000200
#define COMBATLOG_OBJECT_CONTROL_PLAYER 0x00000100
//Reaction
#define COMBATLOG_OBJECT_REACTION_MASK     0x000000F0
#define COMBATLOG_OBJECT_REACTION_HOSTILE  0x00000040
#define COMBATLOG_OBJECT_REACTION_NEUTRAL  0x00000020
#define COMBATLOG_OBJECT_REACTION_FRIENDLY 0x00000010
//Controlleraffiliation
#define COMBATLOG_OBJECT_AFFILIATION_MASK     0x0000000F
#define COMBATLOG_OBJECT_AFFILIATION_OUTSIDER 0x00000008
#define COMBATLOG_OBJECT_AFFILIATION_RAID     0x00000004
#define COMBATLOG_OBJECT_AFFILIATION_PARTY    0x00000002
#define COMBATLOG_OBJECT_AFFILIATION_MINE     0x00000001
//Specialcases(non-exclusive)
#define COMBATLOG_OBJECT_SPECIAL_MASK 0xFFFF0000
#define COMBATLOG_OBJECT_NONE         0x80000000
#define COMBATLOG_OBJECT_MAINASSIST   0x00080000
#define COMBATLOG_OBJECT_MAINTANK     0x00040000
#define COMBATLOG_OBJECT_FOCUS        0x00020000
#define COMBATLOG_OBJECT_TARGET       0x00010000

typedef enum { SPELL_DAMAGE = 0, SWING_DAMAGE, SWING_DAMAGE_LANDED, SPELL_CAST_SUCCESS, SPELL_CAST_FAILED, SPELL_CAST_START, SPELL_AURA_APPLIED, SPELL_AURA_APPLIED_DOSE, SPELL_AURA_REFRESH,
SPELL_AURA_REMOVED, SPELL_AURA_REMOVED_DOSE, SPELL_SUMMON } etype;
// xxx TODO add all types


class damage
{
public:
	damage(tokenizer& t, int offset);

	int hitPoints;
	int maxHitPoints;

	float xPosition;
	float yPosition;

	int dmgDone;
	int overkill;
	int magicSchool;
	int resisted;
	int blocked;
	int absorbed;
	int critical;
	int glancing;
	int crushing;
	int multistrike;
}; // class damage


class sourceOrDestination
{
public:
	sourceOrDestination(tokenizer& t, int offset);

	int hitPoints;
	int maxHitPoints;

	int attackPower;
	int spellPower;
	int resolve;
	int resourceType;
	int resourceAmount;
	int maxResourceAmount;

	float xPosition;
	float yPosition;
	int itemLevel;
}; // class sourceOrDestination

class wowEvent
{
public:
	wowEvent(tokenizer& t);
	void setTime(long int _time_ms) { time_ms = _time_ms; }
	std::string sourceGUID; //1
	std::string sourceName; //2
	int sourceFlags; //3
	int sourceRaidFlags; //4
	std::string destGUID; //5
	std::string destName; //6
	int destFlags; //7
	int destRaidFlags; //8
	etype etype;
protected:
	long int time_ms;
};

class spellEvent : public wowEvent
{
public:
	spellEvent(tokenizer& t);
	int spellID; //9
    // std::string spellName;//10
	int spellSchool;
};



class spellDamage : public spellEvent
{
public:
	spellDamage(tokenizer& t);
	damage d;
	sourceOrDestination sd;
};

class swingDamage : public wowEvent
{
public:
	swingDamage(tokenizer& t);
	damage d;
	sourceOrDestination sd;
};

class swingDamageLanded : public wowEvent
{
public:
	swingDamageLanded(tokenizer& t);
	sourceOrDestination sd;
};


class spellCastSuccess : public spellEvent
{
public:
	spellCastSuccess(tokenizer& t);
	sourceOrDestination sd; // these fields are related to the damage dealer and not to the target
};

class spellCastFailed : public spellEvent
{
public:
	spellCastFailed(tokenizer& t);
	std::string reason; //12
};

class spellCastStart : public spellEvent
{
public:
	spellCastStart(tokenizer& t);
};


class spellAuraApplied : public spellEvent
{
public:
	spellAuraApplied(tokenizer& t);
	std::string buffOrDebuff; //12
};

class spellAuraRefresh : public spellEvent
{
public:
	spellAuraRefresh(tokenizer& t);
	std::string buffOrDebuff; //12
};


class spellAuraAppliedDose : public spellAuraApplied
{
public:
	spellAuraAppliedDose(tokenizer& t);
	int amount; //13
};


class spellAuraRemoved : public spellEvent
{
public:
	spellAuraRemoved(tokenizer& t);
	std::string buffOrDebuff; //12
};

class spellAuraRemovedDose : public spellAuraRemoved
{
public:
	spellAuraRemovedDose(tokenizer& t);
	int amount; //13
};

class spellSummon : public spellEvent
{
public:
	spellSummon(tokenizer& t);
};


wowEvent* createEvent(const char* s, int len, tokenizer& t);