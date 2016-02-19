#pragma once
//#include "tokenizer.h"
#include "events.h"
#include <string>
#include <map>
#include <list>
#include <vector>


typedef std::list<wowEvent*> wowEventListT;

class actor
{
public:
	//actor();
	virtual ~actor();
	actor(wowEvent* eve);
	atype atype;
	float x, y; //positions
	int currentHP;
	int maxHP;
	bool alive;
	GUID guid;
	wowEventListT actions;
	virtual bool processEvent(wowEvent* eve); // return true if the event has been processed successfully or ignored by design
	void computeDamageTimeline(std::vector<int>& dmg, long start, long end, int nbPeriods); // Return dmg doen in each sampling interval between start and end. There are "n" periods
};

class petT : public actor
{
public: 
	petT(wowEvent* eve);
	std::string petName; // duplicate information for debugging
	//bool processEvent(wowEvent* eve) { return true; }
};

typedef std::list<petT*> petListT;

// http://www.dual-boxing.com/threads/47101-Google-Docs-script-for-pulling-character-information-from-the-wow-API
class player : public actor
{
public:
	player(wowEvent* eve);
	virtual ~player();
	//protected:
	virtual bool processEvent(wowEvent* eve);
	int currentLevel;
	int itemLevel;
	float mastery;
	float crit;
	float haste;
	float multistrike;
	float versatility;
	int intellect;
#ifdef _DEBUG
	std::string playerName; // duplicate information for debugging
#endif
	petListT pets;
	// list of party buffs
};


class mage : public player
{
public:
	mage(wowEvent* eve) : player(eve) {}
	bool processEvent(wowEvent* eve) { return player::processEvent(eve); }
	//protected:
	int currentMana;
	int maxMana;
	int spellPower;
};

class arcaneMage :public mage
{
public:
	arcaneMage(wowEvent* eve) : mage(eve) {}
//protected:
	bool processEvent(wowEvent* eve);
	int arcaneCharges;
	float arcaneChargesExpires; // seconds until the arcane charges will expire
	// store temporary buffs
};



typedef std::map<GUID, actor*> actorMapT;
class actors
{
public:
	actors() : minTime(INT32_MAX), maxTime(-9999) {}
	~actors();
	int nbActors() { return actorMap.size(); }
	void add(wowEvent* eve);
	//bool remove(std::string name);
	void stat();
	//petT* searchPetOwner(std::string petName, wowEvent* eve);
	petT* actors::searchPetOwner(std::string petName, GUID ownerGuid);
	actor* locate(std::string name);
	unsigned long minTime, maxTime;

protected:
	actorMapT actorMap;
	std::map<std::string, GUID> petOwners;
};