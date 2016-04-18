#include "arcanemage.h"

using namespace std;



void arcaneMage::change(stats stat, float time, int amount)
{
    switch(stat)
    {
    case MASTERY:
        masteryBuff +=amount;
        break;
    case HASTE:
        hasteBuff +=amount;
        break;
    case CRIT:
        critBuff +=amount;
        break;
    case MULTISTRIKE:
        multistrikeBuff +=amount;
        break;
    case INTELLECT:
        intellectBuff +=amount;
        break;
    case SPELLPOWER:
        spellpowerBuff +=amount;
        break;
    case VERSATILITY:
        versatilityBuff +=amount;
        break;
    }
} // void arcaneMage::change(stats stat, float time, int amount)

void arcaneMage::set(float time, flags flag, int state)
{
    pair<float, int>* stateVariable;
    switch (flag)
    {
    case PRESENCE_OF_MIND:
        stateVariable = &presence_of_mind;
        break;
    case ARCANE_POWER:
        stateVariable = &arcane_power;
        break;
    case TEMPORAL_POWER:
        stateVariable = &temporal_power;
        break;
    case ARCANE_CHARGE:
        stateVariable = & arcane_charge;
        break;
    case ARCANE_MISSILES:
        stateVariable = &arcane_missiles;
        break;
    case BLOODLUST:
        stateVariable = &blooodlust;
        break;
    case EXHAUSTION:
        stateVariable = &exhaustion;
        break;
    case NITHRAMUS:
        stateVariable = &nithramus;
        break;
    case MAGE_ARMOR:
        stateVariable = &mage_armor;
        break;
    case DRAENIC_INTELLECT_POTION:
        stateVariable = &draenic_intellect_potion;
        break;
    case WEAPON_ENCHANT:
        stateVariable = &weapon_enchant;
        break;
    default:
        throw "unknown field";
    } // switch (flag)
    if (state<0)
    {
        temporal_power.second = temporal_power.second + state;
        modified = true;
    }
    else
    {
        stateVariable->second = state;
        stateVariable->first = time;
        modified = true;
    }
} // void arcaneMage::set(float time, flags flag, int state)

void arcaneMage::record_action(float time, const char* target, const char* action)
{
    pair<float, actionT> d;
    d.first = time;
    d.second.action = action;
    d.second.target = target;
    act.push_back(d);
    modified = true;

}


void arcaneMage::damage(float time, const char* target, const char* action, int amount)
{
    dmgonTarget[target] += amount;
    dmgwithSpell[action] += amount;
    pair<float, damageT> d;
    d.first = time;
    d.second.amount = amount;
    d.second.action = action;
    d.second.target = target;
    dmg.push_back(d);
    record_action(time, target, action);
//    modified = true;
    record_action(time, action, target);


}

bool arcaneMage::init()
{
    // values are for fully buffed
    maxMana = mana = 160000;
    haste = 2223;
    mastery = 1823;
    versatility = 328;
    crit = 514;
    multistrike = 1478;
    intellect = 6850;
    incanters_flow = 2;
    spellpower = 10361;
    manareg = 3813;
    health = 496440;
    masteryBuff = hasteBuff = critBuff = multistrikeBuff = intellectBuff = spellpowerBuff = versatilityBuff = 0;

    return true;
/*	Core Stats : strength = 679 | 647(647)  agility = 933 | 889(889)  stamina = 8274 | 7522(7522)  intellect = 6850 | 6262(6016)  spirit = 1155 | 1155(1155)  health = 496440 | 451320  mana = 160000 | 160000
        Generic Stats : mastery = 65.93% | 46.19 % (1823)  multistrike = 27.39% | 22.39 % (1478)  versatility = 5.52% | 2.52 % (328)  leech = 0.00% | 0.00 % (0)
        Spell Stats : power = 10361 | 8831(2569)  hit = 15.00% | 15.00 % (0)  crit = 14.67% | 9.67 % (514)  haste = 32.39% | 24.70 % (2223)  speed = 32.39% | 24.70%  manareg = 3813 | 3591(0)
        Attack Stats : power = 0 | 0(0)  hit = 7.50% | 7.50 % (0)  crit = 14.67% | 9.67 % (514)  expertise = 7.50% | 7.50 % (0)  haste = 32.39% | 24.70 % (2223)  speed = 32.39% | 24.70%
        Defense Stats : armor = 1130 | 1130(1130) miss = 3.00% | 3.00%  dodge = 3.00% | 3.00 % (0)  parry = 0.00% | 0.00 % (0)  block = 0.00% | 0.00 % (0) crit = 0.00% | 0.00%  versatility = 2.76% | 1.26 % (328)
*/

    return true;
}
