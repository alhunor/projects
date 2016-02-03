#include "actors.h"
#include "events.h"
//#include "tokenizer.h"
#include "utils.h"
#include <string>
#include <iostream>

actor::actor(wowEvent* eve)
{
	name = eve->sourceName;
}

player::~player()
{
	petListT::const_iterator it;
	for (it = pets.begin(); it != pets.end(); ++it)
	{
		delete *it;
	}

}

atype actorType(wowEvent* eve)
{
	atype at=Invalid;

	if (isPrefix("Player", eve->sourceGUID.c_str()))
	{
		return Player;
	}

	if (isPrefix("Pet", eve->sourceGUID.c_str()))
	{
		return Pet;
	}

	if (isPrefix("Creature", eve->sourceGUID.c_str()))
	{
		return Creature;
	}

	return Invalid;
} // atype actortype(wowEvent* eve)


actors::~actors()
{
	actorMapT::const_iterator it;
	for (it = actorMap.begin(); it != actorMap.end(); ++it)
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

bool checkFlags(int attribute, int flags)
{
	int b = attribute & flags;
	return b != 0;
}


petT* actors::searchPetOwner(std::string petName, std::string ownerName)
{
	actorMapT::const_iterator search;
	petListT::const_iterator searchPet;
	petT* pet;
/*	std::string ownerName;
	switch (eve->etype)
	{
	case SPELL_DAMAGE:
//		ownerName = ((spellDamage*)(eve))->resourceActor;
		break;
	case SPELL_AURA_APPLIED:
		ownerName = ((spellDamage*)(eve))->destName;
		break;
	case SWING_DAMAGE:
	case SWING_DAMAGE_LANDED:
	case SPELL_CAST_SUCCESS:
	case SPELL_CAST_FAILED:
	case SPELL_CAST_START:

	case SPELL_AURA_APPLIED_DOSE:
	case SPELL_AURA_REFRESH:
	case SPELL_AURA_REMOVED:
	case SPELL_AURA_REMOVED_DOSE:
	case SPELL_SUMMON:
		return NULL;
	}
*/	
	search = actorMap.find(ownerName);
	if (search == actorMap.end())
		return NULL;

	player* owner = static_cast<player*>(search->second);
	for (searchPet = owner->pets.begin(); searchPet != owner->pets.end(); ++searchPet)
	{
		pet = *searchPet;
		if (pet->name == petName) // found an event generated by the pet
		{
			return pet;
		}
	}
	return NULL;
} // petT* actors::searchPetOwner(std::string petName, std::string ownerName)


void actors::add(wowEvent* eve)
{
	actor* actor;
	petT* pet;
	actorMapT::const_iterator search;
	petListT::const_iterator searchPet;


	// Exit is actor is not friendly;
	if (!checkFlags(eve->sourceFlags, COMBATLOG_OBJECT_REACTION_FRIENDLY))
		return;

	if (eve->etype == SPELL_SUMMON)
	{
		//std::string petName = eve->destName;
		petOwners[eve->destGUID] = eve->sourceName;
		player* owner;

		search = actorMap.find(eve->sourceName);
		if (search == actorMap.end())
		{
			throw "TODO :: Create the player and store it in ownerr and actorMap";
			owner = NULL; 
		} else
		{
			owner = static_cast<player*>(search->second);
		}
		bool found = false;
		for (searchPet = owner->pets.begin(); searchPet != owner->pets.end(); ++searchPet)
		{
			pet = *searchPet;
			if (pet->name == eve->destName) // found an event generated by the pet
			{
				found = true;
			}
		}
		if (!found)
		{
			// create pet and store it in player
			pet = new petT(eve);
			owner->pets.push_back(pet);
		}
		// All is done, we can exit actors::add(wowEvent* eve)
		return;
	}

	switch(actorType(eve))
	{
	case Player:
		if (!checkFlags(eve->sourceFlags, COMBATLOG_OBJECT_TYPE_PLAYER))
			return;
		search = actorMap.find(eve->sourceName);
		if (search == actorMap.end())
		{
			{
				if (isPrefix("Kettle-Arathor", eve->sourceName.c_str()))
				{
					actor = new arcaneMage(eve);
				}
				else
				{
					actor = new player(eve);
				}
				actorMap[eve->sourceName] = actor;
			}
		} else
		{
			actor = search->second;
		}
		// process the event
		actor->actions.push_back(eve);
		actor->processEvent(eve);
		break;
	case Pet:
		if (!checkFlags(eve->sourceFlags, COMBATLOG_OBJECT_CONTROL_PLAYER))
			return;
		pet = searchPetOwner(eve->sourceName, petOwners[eve->sourceGUID]);
		pet->actions.push_back(eve);
		//pet->processEvent(eve);

		break;
	case Creature:
		if (!checkFlags(eve->sourceFlags, COMBATLOG_OBJECT_CONTROL_PLAYER))
			return;
		pet = searchPetOwner(eve->sourceName, petOwners[eve->sourceGUID]);
		pet->actions.push_back(eve);
		//pet->processEvent(eve);
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

	switch (eve->etype)
	{
	case SPELL_DAMAGE:
		break;
	default:
		return false;
	}
	return b;
} // bool arcaneMage::processEvent(wowEvent* eve)

void actors::stat()
{
	actorMapT::const_iterator it;
	petListT::const_iterator it2;
	wowEventListT::const_iterator it3;
	//wowEvent* eve;
	actor* a;
	player *p;
	petT *pet;

	for (it = actorMap.begin(); it != actorMap.end(); ++it)
	{
		a = it->second;
		if (a->atype == Player)
		{
			p = (player*)a;
		}
		else
		{
			throw "Actor list has been contaminated by non-player entitities";
		}

		std::cout << p->name<<std::endl;

		for (it2 = p->pets.begin(); it2 != p->pets.end(); ++it2)
		{
			pet = *it2;
			std::cout << "   "<<pet->name << std::endl;
		}
		for (it3 = a->actions.begin(); it3 != a->actions.end(); ++it3)
		{

		}
	}
} // void actors::stat()