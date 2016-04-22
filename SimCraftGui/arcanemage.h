#ifndef ARCANEMAGE
#define ARCANEMAGE

#include <string>
#include <vector>
#include <map>

#pragma warning (disable : 4100)

struct actionT
{
    std::string target;
	std::string action;
};

struct damageT
{
	std::string action;
	std::string target;
    int amount;
};

struct status
{
    int cd; //
    int charges;
    int duration;
//    int active;
};
;

typedef enum {PRESENCE_OF_MIND=0, ARCANE_POWER, TEMPORAL_POWER, ARCANE_CHARGE, ARCANE_MISSILES,
            DRAENIC_INTELLECT_POTION, EXHAUSTION, NITHRAMUS, WEAPON_ENCHANT, PRISMATIC_CRYSTAL, HEROISM, DOOMNOVA} flags;

typedef enum {MASTERY=0, HASTE, CRIT, MULTISTRIKE, INTELLECT, SPELLPOWER, VERSATILITY} stats;


class arcaneMage
{
public:
    arcaneMage() { init(); }
    ~arcaneMage() {}
    bool init();
    void changemana(float time, int amount)
    {
        time = time +0; // avoids warning during compillation by QT
        mana += amount;
        if (mana > maxMana)
        {
            mana = mana;
        }
    }
    void change (stats stat, float time, int amount);

    void cast(float time, const char* buff) {  }
    void markOfDoom(float time, const char* buff) {  }
    void summon(float time, const char* buff) {  }
    int stat_value(stats stat);
    float stat_percentage(stats stat);
    int health;
    int maxMana, mana, manareg;
    int incanters_flow;
    int mastery, haste, crit, multistrike, intellect, spellpower, versatility;
    int masteryBuff, hasteBuff, critBuff, multistrikeBuff, intellectBuff, spellpowerBuff, versatilityBuff;

    std::map<std::string, int> dmgonTarget;
    std::map<std::string, int> dmgwithSpell;
    void damage(float time, const char* target, const char* action, int amount);
    void set(float time, flags flag, int state);
    void record_action(float time, const char* target, const char* action);
    bool modified;
    std::vector<std::pair<float, damageT> >  dmg;
    std::vector<std::pair<float, actionT> > act;

// flags
    std::pair<float, status> presence_of_mind, arcane_power, temporal_power, arcane_charge, arcane_missiles, nithramus, prismatic_crystal;
    std::pair<float, status> draenic_intellect_potion, weapon_enchant, hero, doom_nova;
// level 100 ratios
    static const int ratio_crit =110;
    static const int ratio_mastery =110;
    static const int ratio_haste =90;
    static const int ratio_multi =60;
    static const int ratio_versa =130;
}; // class arcaneMage

#endif // ARCANEMAGE

