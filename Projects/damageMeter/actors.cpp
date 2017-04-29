#include "actors.h"
#include "events.h"
//#include "tokenizer.h"
#include "utils.h"
#include <string>
#include <iostream>

#include <io.h>
#include <fcntl.h>
#include <vector>

using namespace std;
extern guidClass guids;
std::string actors::unknown = "";



actor::actor(GUID _guid, std::string& _name) : x(0), y(0), currentHP(1), maxHP(1), alive(true)
{
	guid = _guid;
	name = _name;
}

petT::petT(GUID _guid, std::string& _name) : actor(_guid, _name)
{
	atype = Pet;
/*	if (eve->etype == wowEvent::SPELL_SUMMON)
	{
		petName = guids.lookUpPet(eve->destGUID).name;
	} else
	{
		petName = guids.lookUpPet(eve->sourceGUID).name;
	}
*/
	owner = NULL;
//	name = raid[eve->destGUID].name; ??
}

player::player(GUID _guid, std::string& _name) : actor(_guid, _name), currentLevel (100), itemLevel(720), mastery(10), crit(10), haste(10), multistrike(10), versatility(10), intellect(10)
{
	atype = Player; 
}


player::~player()
{
	petListT::const_iterator it;
/*	for (it = pets.begin(); it != pets.end(); ++it)
	{
		delete *it;
	}*/
	// TODO location where pets are freed

}


actors::~actors()
{
	actorMapT::const_iterator it;
	petMapT::const_iterator it2;
	for (it = players.begin(); it != players.end(); ++it)
	{
		delete it->second;
	}
	for (it2 = pets.begin(); it2 != pets.end(); ++it2)
	{
		delete it2->second;
	}
	for (it = enemies.begin(); it != enemies.end(); ++it)
	{
		delete it->second;
	}

}

actor::~actor()
{
	wowEventListT::const_iterator it2;
	for (it2 = actions.begin(); it2 != actions.end(); ++it2)
	{
		delete *it2;
	}
}


petT* actors::searchPetOwner(GUID petGuid, GUID  ownerName)
{
	petMapT::const_iterator searchPet;
	petT* pet;

	searchPet = pets.find(petGuid);
	if (searchPet == pets.end())
		return NULL;

	pet = searchPet->second;
	return pet;
} // petT* actors::searchPetOwner(std::string petName, std::string ownerName)


void actors::summonPet(wowEvent* eve)
{
	actorMapT::const_iterator search;
	petMapT::const_iterator searchPet;
	petT* pet = NULL;

	player* owner;

	search = players.find(eve->sourceGUID);
	if (search == players.end())
	{
		owner = new player(eve->sourceGUID, eve->sourceName);
		players[eve->sourceGUID] = owner;
	} else
	{
		owner = static_cast<player*>(search->second);
	}

	for (searchPet = pets.begin(); searchPet != pets.end(); ++searchPet)
	{
		if (searchPet->second->owner == owner && searchPet->second->name==eve->destName)
		{
			pet= searchPet->second;
			return;
		}
	}
	// new pet
	pet = new petT(eve->destGUID, eve->destName);
	pet->owner = owner;
	pets[eve->destGUID] = pet;
	/*if (pet->owner == NULL)
	{
		pet->owner = owner;
	} else if (owner != NULL && pet->owner != owner)
	{
		throw "TODO :: Check error, pet has 2 owners";
	}
	*/
} // void actors::summonPet(wowEvent* eve)


void actors::add(wowEvent* eve)
{
	actor* actor;
	petT* pet;
	actorMapT::const_iterator search;
	petMapT::const_iterator searchPet;

	if (eve->time_ms < minTime)
	{
		minTime = eve->time_ms;
	} else 	if (eve->time_ms > minTime)
	{
		maxTime = eve->time_ms;
	}

	// XXX TODO - we need to handle changes to auras before manging player only events. 
	// They can come from Neutral Items, NPC, or enemies and the rest of the function only manages friendly actions.
	
	// Exit is actor is not friendly;
	if (!checkFlags(eve->sourceFlags, COMBATLOG_OBJECT_REACTION_FRIENDLY))
		return;

	// Register summoned Pets with thir owners
	if (eve->etype == wowEvent::SPELL_SUMMON)
	{
		//std::string petName = eve->destName;
		summonPet(eve);
		return;
	} // if (eve->etype == SPELL_SUMMON)

	switch(eve->atype)
	{
	case Player:
		if (!checkFlags(eve->sourceFlags, COMBATLOG_OBJECT_TYPE_PLAYER))
			return;
		search = players.find(eve->sourceGUID);
		if (search == players.end())
		{
			{
				/*if (isPrefix("Kettle-Arathor", eve->sourceName.c_str()))
				{
					actor = new arcaneMage(eve);
				}
				else*/
				{
					actor = new player(eve->sourceGUID, eve->sourceName);
				}
				players[eve->sourceGUID] = actor;
			}
		} else
		{
			actor = search->second;
			if (actor->name == actors::unknown)
			{
				actor->name = eve->sourceName;
			}
		}
		// process the event
		actor->actions.push_back(eve);
		actor->processEvent(eve);
		break;
	case Pet:
	case Creature:
		{
			if (!checkFlags(eve->sourceFlags, COMBATLOG_OBJECT_CONTROL_PLAYER))
				return;

			GUID ownerGUID = eve->ownerID();
			player* owner;
			if (ownerGUID == 0)
			{
				owner = NULL;
			} else
			{
				search = players.find(ownerGUID);
				if (search == players.end())
				{
					// we reach this point if a pet pőerform an action before its player owner had a chance to be registered.
					// we do not know the name of the player, it will  be recorded later, when the player first performs an action
					owner = new player(ownerGUID, actors::unknown);
					players[eve->sourceGUID] = owner;
				}
				else
				{
					owner = static_cast<player*>(search->second);
				}
			}
			//std::string petName = eve->sourceName;
			GUID petGUID = eve->sourceGUID;
			pet = NULL;

			for (searchPet = pets.begin(); searchPet != pets.end(); ++searchPet)
			{
				if (searchPet->second->name == eve->sourceName)
				{
					if (searchPet->second->owner == NULL || owner == NULL || searchPet->second->owner == owner)
					{
						pet = searchPet->second;
					}
				}
			}
			if (pet == NULL)
			{
				// Todo check this
				pet = new petT(eve->sourceGUID, eve->sourceName);
				pet->owner = owner;
				pets[petGUID] = pet;
			} else
			{
				// check if the information about the owner is accurate
				if (pet->owner == NULL)
				{
					pet->owner = owner;
				} else if (owner!=NULL && pet->owner != owner)
				{
					// See line 6058 - apears to be an error
					//throw "TODO :: Check error, pet has 2 owners";
				}
			}
			if (pet)
			{
				pet->actions.push_back(eve);
				//pet->processEvent(eve);
			}
		}
		break;
	} // switch(actorType(eve))
} // actorType actors::add(wowEvent* eve)



bool actor::processEvent(wowEvent* eve)
{
	return true;
} // bool actor::processEvent(wowEvent* eve)


bool player::processEvent(wowEvent* eve)
{
	return actor::processEvent(eve);
}


bool arcaneMage::processEvent(wowEvent* eve)
{
	bool b = mage::processEvent(eve);

	if (eve->etype == wowEvent::SPELL_DAMAGE)
	{
		// ..
	}
	return b;
} // bool arcaneMage::processEvent(wowEvent* eve)


struct damageStat
{
	damageStat() : min(9999999), max(0), sum(0), sum2(0), n(0) {}
	double min, max;
	double sum, sum2;
	int n;
	void add(double damageInc);
	double stdev();
};

void damageStat::add(double damageInc)
{
	n++;
	sum += damageInc;
	sum2 += damageInc*damageInc;
	max = max>damageInc?max: damageInc;
	min = min<damageInc ? min : damageInc;
}

double damageStat::stdev()
{
	if (n <= 1) return 0;
	double res = (sum2 - sum*sum/n) / (n - 1);
	return sqrt(res);
}


typedef map<int, damageStat> DS;
typedef map<string, damageStat> DS2;

extern map<int, string> spells;


void actors::stat()
{
	actorMapT::const_iterator it;
	petListT::const_iterator it2;
	wowEventListT::const_iterator it3;
	wowEvent* eve;
	actor* a;
	player *p;
	petT *pet;

	_setmode(_fileno(stdout), _O_U16TEXT);
	wchar_t* w;
	int count = 0;
	wcout.precision(17);
//Damage per spell
	wcout << "damage per spell\n";
	for (it = players.begin(); it != players.end(); ++it)
	{
		p = (player*)it->second;

		DS dmg;
		spells[0] = "SwingDamage";
		int damageInc;
		int damageDone = 0;
		int friendlyDamage = 0;
		for (it3 = p->actions.begin(); it3 != p->actions.end(); ++it3)
		{
			eve = *it3;
			spellDamage* sd;
			damageInc = eve->damage_amount();
			if (damageInc < 1) continue;
			if (! checkFlags(eve->destFlags, COMBATLOG_OBJECT_REACTION_HOSTILE))
			{
				// ignore mage damage on Prismatic Crystal
				if (eve->destName == "Prismatic Crystal") continue;
				friendlyDamage += damageInc;
				//continue;
			}
			damageDone += damageInc;
			string s;
			if (eve->etype == wowEvent::SPELL_DAMAGE)
			{
				sd = (spellDamage*)eve;
				dmg[sd->spellID].add(damageInc);
				//s = spells[sd->spellID];
				//wcout << s.c_str() << "," << damageInc << endl;
			} else if (eve->etype == wowEvent::SWING_DAMAGE)
			{
				dmg[0].add(damageInc);
			}
		} // for (it3 = a->actions.begin(); it3 != a->actions.end(); ++it3)

		w = convert(p->name.c_str());
		std::wcout << w << " Hostile  " << damageDone ;
		std::wcout << " Friendly  " << friendlyDamage << std::endl;
		delete[] w;
		map<int, damageStat>::const_iterator it4;
		for (it4 = dmg.begin(); it4 != dmg.end(); ++it4)
		{			
			int spellID = it4->first;
			string s = spells[spellID];
			wcout << " ->" << s.c_str() << " , " << dmg[spellID].n<<" , " <<  dmg[spellID].min << " , " << dmg[spellID].max << " , " << trunc(dmg[spellID].sum/dmg[spellID].n) <<
				" , " << trunc(dmg[spellID].stdev())  <<endl;
		}

		for (it2 = p->pets.begin(); it2 != p->pets.end(); ++it2)
		{
			pet = *it2;
			int petDamageDone = 0;
			for (it3 = pet->actions.begin(); it3 != pet->actions.end(); ++it3)
			{
				wowEvent* eve = *it3;
				petDamageDone += eve->damage_amount();
			}
			w = convert(pet->name.c_str());
			if (petDamageDone > 0)
			{
				std::wcout << "     " << w << "   " << petDamageDone << std::endl;
			}
			delete[] w;
		} // Pet damage
	} // for (it = actorMap.begin(); it != actorMap.end(); ++it) for damage per spell

// now we compute damage per target

	wcout << "Damage per enemey\n";

//Damage per spell
	for (it = players.begin(); it != players.end(); ++it)
	{
		a = (player*)it->second;
		DS2 dmg;
		int damageInc;
		int damageDone = 0;
		for (it3 = a->actions.begin(); it3 != a->actions.end(); ++it3)
		{
			wowEvent* eve = *it3;
			damageInc = eve->damage_amount();
			if (damageInc>0)
			{
				damageDone += damageInc;
				dmg[eve->destName].add(damageInc);
			}
		} // for (it3 = a->actions.begin(); it3 != a->actions.end(); ++it3)

		w = convert(p->name.c_str());
		std::wcout << w << "   " << damageDone << std::endl;
		delete[] w;
		map<string, damageStat>::const_iterator it4;
		for (it4 = dmg.begin(); it4 != dmg.end(); ++it4)
		{
			string s = it4->first;
			wcout << " ->" << s.c_str() << " , " << it4->second.n << " , " << it4->second.sum << endl;
		}
		wcout << "Pets" << std::endl;
		for (it2 = p->pets.begin(); it2 != p->pets.end(); ++it2)
		{
			pet = *it2;
			for (it3 = pet->actions.begin(); it3 != pet->actions.end(); ++it3)
			{
				eve = *it3;
				damageInc = eve->damage_amount();
				if (damageInc>0)
				{
					dmg[eve->destName].add(damageInc);
				}
			}
		} // Pet damage
		for (it4 = dmg.begin(); it4 != dmg.end(); ++it4)
		{
			string s = it4->first;
			wcout << " ->" << s.c_str() << " , " << it4->second.n << " , " << it4->second.sum << endl;
		}
	} // for (it = actorMap.begin(); it != actorMap.end(); ++it) for damage per enemmy

} // void actors::stat()

  // Return dmg done in each sampling interval between [start, end). There are "nbPeriods" periods.
void actor::computeDamageTimeline(std::vector<int>& dmg, long start, long end, int nbPeriods)
{
	dmg.resize(nbPeriods);
	for (int i = 0; i < nbPeriods; ++i)
	{
		dmg[i] = 0;
	}
	if (end < start + 1) throw "Interval is ill defined";
	wowEventListT::const_iterator it3;
	int damageInc;
	long time;
	float freq = (float)nbPeriods/(end-start);
	int period;
	for (it3 = actions.begin(); it3 != actions.end(); ++it3)
	{
		wowEvent* eve = *it3;
		damageInc = eve->damage_amount();
		if (damageInc <= 0) continue;
		time = eve->time_ms;
		if (time < start || time >= end) continue;
		period = (int) ((time - start) * freq);
		dmg[period] += damageInc;
	}
} // void actor::computeDamageTimeline(std::vector<int>& dmg, long start, long end, int nbPeriods)


actor* actors::locatePlayer(std::string name)
{
	GUID h = guids.hash(name, Player);
	actorMapT::const_iterator it;
	actor* a;

	for (it = players.begin(); it != players.end(); ++it)
	{
		a = it->second;
		if (a->guid == h)
		{
			return a;
		}
	}
	return NULL;
} // actor* actors::locate(std::string name)