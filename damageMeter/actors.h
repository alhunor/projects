#pragma once
//#include "tokenizer.h"
#include "events.h"
#include <string>
#include <map>
#include <list>

enum atype { Player, Pet, Creature, Invalid };

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
	typedef std::list<wowEvent*> wowEventListT;
	wowEventListT actions;
	virtual bool processEvent(wowEvent* eve) = 0; // return true if the venet has been processed successfully or ignored by design
};

class petT : public actor
{
public: 
	petT(wowEvent* eve) : actor(eve) { atype=Player; }
	bool processEvent(wowEvent* eve) { return true; }
};

typedef std::list<petT*> petListT;

// http://www.dual-boxing.com/threads/47101-Google-Docs-script-for-pulling-character-information-from-the-wow-API
class player : public actor
{
public:
	player(wowEvent* eve) : actor(eve) { atype = Player; }
	~player();
	//protected:
	bool processEvent(wowEvent* eve) { return 0; }
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


struct actorType
{
	atype atype;
	player *player;
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

protected:
	actorMapT actorMap;
};



class mage : public player
{
public:
	mage(wowEvent* eve) : player(eve) {}
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