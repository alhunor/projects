#pragma once
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
	actor(GUID guid, std::string& name);
	atype atype;
	float x, y; //positions
	int currentHP;
	int maxHP;
	bool alive;
	GUID guid;
	std::string name;
	wowEventListT actions;
	virtual bool processEvent(wowEvent* eve); // return true if the event has been processed successfully or ignored by design
	void computeDamageTimeline(std::vector<int>& dmg, long start, long end, int nbPeriods); // Return dmg doen in each sampling interval between start and end. There are "n" periods
};


class player;
class petT : public actor
{
public:
	petT(GUID guid, std::string& name);
	player* owner;
};

typedef std::list<petT*> petListT;

// http://www.dual-boxing.com/threads/47101-Google-Docs-script-for-pulling-character-information-from-the-wow-API
class player : public actor
{
public:
	player(GUID guid, std::string& name);
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
	petListT pets;
	// list of party buffs
};



class mage : public player
{
public:
	mage(GUID guid, std::string& name) : player(guid, name) {}
	bool processEvent(wowEvent* eve) { return player::processEvent(eve); }
	//protected:
	int currentMana;
	int maxMana;
	int spellPower;
};

class arcaneMage :public mage
{
public:
	arcaneMage(GUID guid, std::string& name) : mage(guid, name) {}
//protected:
	bool processEvent(wowEvent* eve);
	int arcaneCharges;
	float arcaneChargesExpires; // seconds until the arcane charges will expire
	// store temporary buffs
};



typedef std::map<GUID, actor*> actorMapT;
typedef std::map<GUID, petT*> petMapT;
class actors
{
public:
	actors() : minTime(INT_MAX), maxTime(-9999) {}
	~actors();
	int nbPlayers() { return players.size(); }
	void add(wowEvent* eve);
	void stat();
	petT* searchPetOwner(GUID petGuid, GUID  ownerName);
	// finds a player by its name
	actor* locatePlayer(std::string name);
	unsigned long minTime, maxTime;

	petMapT pets;
protected:
	actorMapT players;
	actorMapT enemies;
	std::map<std::string, GUID> petOwners;
	void summonPet(wowEvent* eve);
	static std::string unknown;
};