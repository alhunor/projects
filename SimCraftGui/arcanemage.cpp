#include "arcanemage.h"

using namespace std;

int arcaneMage::stat_value(stats stat)
{
    switch(stat)
    {
    case MASTERY:
        return mastery + masteryBuff;
    case HASTE:
        return hasteBuff + haste;
    case CRIT:
        return critBuff + crit;
    case MULTISTRIKE:
        return multistrikeBuff + multistrike;
    case INTELLECT:
        return intellectBuff + intellect;
    case SPELLPOWER:
        return spellpowerBuff + spellpower;
    case VERSATILITY:
        return versatilityBuff + versatility;
    }
    throw "Unknown stat";
}

float arcaneMage::stat_percentage(stats stat)
{
    switch(stat)
    {
    case MASTERY:
        return (float)(mastery + masteryBuff) / ratio_mastery;
    case HASTE:
        return (float)(hasteBuff + haste) / ratio_haste;
    case CRIT:
        return (float)(critBuff + crit) / ratio_crit;
    case MULTISTRIKE:
        return (float)(multistrikeBuff + multistrike) / ratio_multi;
    case INTELLECT:
        return 100.0f*(intellectBuff + intellect) / intellect;
    case SPELLPOWER:
        return (float)(spellpowerBuff + spellpower) / spellpower;
    case VERSATILITY:
        return (float)(versatilityBuff + versatility) / ratio_versa;
    }
    throw "Unknown stat";
}

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

void arcaneMage::set(float time, flags flag, int value)
{
    pair<float, status>* stateVariable;
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
    case HEROISM:
        stateVariable = &hero;
        break;
    case NITHRAMUS:
        stateVariable = &nithramus;
        break;
    case PRISMATIC_CRYSTAL:
        stateVariable = &prismatic_crystal;
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
    if (value<0)
    {
        temporal_power.second.charges += value;
        modified = true;
    }
    else
    {
        stateVariable->second.charges = value;
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

    presence_of_mind.first = -999;
    presence_of_mind.second.charges=0;
    presence_of_mind.second.cd=90;
    presence_of_mind.second.duration=60; // should be infinity

    arcane_power.first = -999;
    arcane_power.second.charges=0;
    arcane_power.second.cd=90;
    arcane_power.second.duration=15; //ok

    temporal_power.second.charges=0;
    temporal_power.second.cd=0;
    temporal_power.second.duration=12; // todo check duration

    hero.first = -999;
    hero.second.charges=0;
    hero.second.cd=600;
    hero.second.duration=40; //ok


    doom_nova.first = -0;
    doom_nova.second.charges=0;
    doom_nova.second.cd=0;
    doom_nova.second.duration=10; //check


    arcane_charge.first = 0;
    arcane_charge.second.charges = 0;
    arcane_charge.second.cd = 0;
    arcane_charge.second.duration = 15; //ok

    arcane_missiles.first = 0;
    arcane_missiles.second.charges =0;
    arcane_missiles.second.cd = 0;
    arcane_missiles.second.duration = 20; //ok

    nithramus.first = -999;
    nithramus.second.charges = 0;
    nithramus.second.cd = 120;
    nithramus.second.duration = 15; //ok

    draenic_intellect_potion.first = -999;
    draenic_intellect_potion.second.charges = 0;
    draenic_intellect_potion.second.cd = 60;
    draenic_intellect_potion.second.duration = 24; //ok

    weapon_enchant.second.charges = 0;
    weapon_enchant.second.cd = 0;
    weapon_enchant.second.duration = 10;

    prismatic_crystal.first = -999;
    prismatic_crystal.second.charges = 0;
    prismatic_crystal.second.cd = 90;
    prismatic_crystal.second.duration = 12; //ok


    return true;
/*	Core Stats : strength = 679 | 647(647)  agility = 933 | 889(889)  stamina = 8274 | 7522(7522)  intellect = 6850 | 6262(6016)  spirit = 1155 | 1155(1155)  health = 496440 | 451320  mana = 160000 | 160000
        Generic Stats : mastery = 65.93% | 46.19 % (1823)  multistrike = 27.39% | 22.39 % (1478)  versatility = 5.52% | 2.52 % (328)  leech = 0.00% | 0.00 % (0)
        Spell Stats : power = 10361 | 8831(2569)  hit = 15.00% | 15.00 % (0)  crit = 14.67% | 9.67 % (514)  haste = 32.39% | 24.70 % (2223)  speed = 32.39% | 24.70%  manareg = 3813 | 3591(0)
        Attack Stats : power = 0 | 0(0)  hit = 7.50% | 7.50 % (0)  crit = 14.67% | 9.67 % (514)  expertise = 7.50% | 7.50 % (0)  haste = 32.39% | 24.70 % (2223)  speed = 32.39% | 24.70%
        Defense Stats : armor = 1130 | 1130(1130) miss = 3.00% | 3.00%  dodge = 3.00% | 3.00 % (0)  parry = 0.00% | 0.00 % (0)  block = 0.00% | 0.00 % (0) crit = 0.00% | 0.00%  versatility = 2.76% | 1.26 % (328)
*/

    return true;
}
