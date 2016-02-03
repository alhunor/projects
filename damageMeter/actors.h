#pragma once
//#include "tokenizer.h"
#include "events.h"
#include <string>
#include <map>
#include <list>

enum atype { Player, Pet, Creature, Invalid };


typedef std::list<wowEvent*> wowEventListT;

class actor
{
public:
	actor();
	virtual ~actor();
	actor(wowEvent* eve);
	atype atype;
	float x, y; //positions
	int currentHP;
	int maxHP;
	bool alive;
	std::string name;
	wowEventListT actions;
	virtual bool processEvent(wowEvent* eve); // return true if the event has been processed successfully or ignored by design
};

class petT : public actor
{
public: 
	petT(wowEvent* eve) : actor(eve) { atype = Pet; name = eve->destName; }
	//bool processEvent(wowEvent* eve) { return true; }
};

typedef std::list<petT*> petListT;

// http://www.dual-boxing.com/threads/47101-Google-Docs-script-for-pulling-character-information-from-the-wow-API
class player : public actor
{
public:
	player(wowEvent* eve) : actor(eve) { atype = Player; }
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



typedef std::map<std::string, actor*> actorMapT;
class actors
{
public:
	actors() {}
	~actors();
	int nbActors() { return actorMap.size(); }
	void add(wowEvent* eve);
	//bool remove(std::string name);
	void stat();
	//petT* searchPetOwner(std::string petName, wowEvent* eve);
	petT* actors::searchPetOwner(std::string petName, std::string ownerName);

protected:
	actorMapT actorMap;
	std::map<std::string, std::string> petOwners;
};