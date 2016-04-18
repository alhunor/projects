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


typedef enum {PRESENCE_OF_MIND=0, ARCANE_POWER, TEMPORAL_POWER, ARCANE_CHARGE, ARCANE_MISSILES, BLOODLUST,
            DRAENIC_INTELLECT_POTION, EXHAUSTION, NITHRAMUS, MAGE_ARMOR, WEAPON_ENCHANT} flags;

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
    int health;
    int maxMana, mana, manareg;
    int mastery, haste, crit, multistrike, intellect, spellpower, versatility;
    int masteryBuff, hasteBuff, critBuff, multistrikeBuff, intellectBuff, spellpowerBuff, versatilityBuff;

    int incanters_flow;
    std::map<std::string, int> dmgonTarget;
    std::map<std::string, int> dmgwithSpell;
    void damage(float time, const char* target, const char* action, int amount);
    void set(float time, flags flag, int state);
    void record_action(float time, const char* target, const char* action);
    bool modified;
    std::vector<std::pair<float, damageT> >  dmg;
    std::vector<std::pair<float, actionT> > act;

// flags
    std::pair<float, int> presence_of_mind, arcane_power, temporal_power, arcane_charge, arcane_missiles, blooodlust, exhaustion, nithramus, mage_armor;
    std::pair<float, int> draenic_intellect_potion, weapon_enchant;
}; // class arcaneMage

#endif // ARCANEMAGE

