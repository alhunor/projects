#include "combatlog.h"
#include "arcaneMage.h"
#include "filesystems.h"
#include "tokenizer.h"
#include "utils.h"
#include <fstream>
#include <iostream>
#ifdef QTHUNOR
#include <QDebug>
#include <QScrollBar>
#else
std::ostream& qDebug() { return std::cout; }
#endif

using namespace std;

bool parseCombatLog(char* fileName, arcaneMage& kettle, int nbSteps)
{
    combatlog log;
	if (!log.init(fileName, kettle
#ifdef QTHUNOR
		, NULL
#endif
		)) return false;
    return log.parseCombatLog(999999);
} // bool parseCombatLog(char* fileName, arcaneMage kettle)



bool combatlog::init(const char* fileName, arcaneMage& player
#ifdef QTHUNOR
, QPlainTextEdit* edit
#endif
)
{
    count = 0;
    in.open(fileName);
    if (in.fail())
    {
        std::cout << "File not found" << endl;
        return false;
    }
    kettle = &player;
#ifdef QTHUNOR
    actionList = edit;
#endif
    return initialised = true;
} // bool combatlog::init(const char* fileName, arcaneMage& player)


bool combatlog::readLine()
{
    char* timeStr;
    kettle->modified = false;

    if (in.eof()) return false;

    in.getline(buff, MAX_LEN);
    len = (int)in.gcount();
    count++;

    if (len == MAX_LEN)
    {
        throw "Line is too long";
    }
    if (len == 0)
    {
        return false;
    }
    bool output = false;
    t.init(buff, len);
    timeStr = t.slice(' ');
    time = asFloat(timeStr);
    if (output) std::cout << time;
    t.tokenize(0, ' ');
    const char* who = t.token(0);
    if (output) std::cout << "  " << who;
    actor[who]++;
    const char* what1, *what2;
    int amount;

    if (strcmp(who, "Kettle_Active") == 0)
    {
    }
    else if (strcmp(who, "Kettle_Active_temporal_hero") == 0)
    {
    }
    else if (strcmp(who, "Fluffy_Pillow") == 0)
    {
        /*			what1 = t.token(2);
        else
        {
        std::cout << buff << endl;
        }*/

    }
    else if (strcmp(who, "Player") == 0)
    {
        return false;
    }
    else if (strcmp(who, "Kettle_Active_prismatic_crystal") == 0)
    {
     //   return true;
    }
    else
    {
        std::cout << who << endl;
    }

    const char* action = t.token(1);
    if (strcmp(action, "gains") == 0)
    {
        what1 = t.token(2);
        if (strcmp(what1, "incanters_flow_1") == 0 || strcmp(what1, "incanters_flow_2") == 0 || strcmp(what1, "incanters_flow_3") == 0 || strcmp(what1, "incanters_flow_4") == 0 || strcmp(what1, "incanters_flow_5") == 0)
        {
			kettle->set(time, INCANTERS_FLOW, what1[15] - 48, false);
			return true;
        }
		else if ((amount = asInt(what1))!=-123456)
		{
			what2 = t.token(3);
			if (t.nbTokens() >= 4 && strcmp(t.token(4), "mana") == 0)
			{
				kettle->changemana(time, amount);
				return false;
			}
			else if (strcmp(t.token(3), "intellect") == 0)
			{
				kettle->change(INTELLECT, time, amount);
			}
			else if (strcmp(t.token(3), "mastery_rating") == 0)
			{
				kettle->change(MASTERY, time, amount);
			}
			else if (strcmp(t.token(3), "haste_rating") == 0)
			{
				kettle->change(HASTE, time, amount);
			}
			else if (strcmp(t.token(3), "crit_rating") == 0)
			{
				kettle->change(CRIT, time, amount);
			}
			else if (strcmp(t.token(3), "multistrike_rating") == 0)
			{
				kettle->change(MULTISTRIKE, time, amount);
			}
			else if (strcmp(t.token(3), "spellpower_rating") == 0)
			{
				kettle->change(SPELLPOWER, time, amount);
			}
			else if (strcmp(t.token(3), "versatility_rating") == 0)
			{
				kettle->change(VERSATILITY, time, amount);
			}
			else
			{
				throw buff;
			}
			return true;
		}
        else if (strcmp(what1, "mage_armor_1") == 0)
        {
            return false;
        }
        else if (strcmp(what1, "bloodlust_1") == 0)
        {
            kettle->set(time, HEROISM, 1, false);
            return true;
        }
        else if (strcmp(what1, "exhaustion_1") == 0)
        {
            return false;
        }
        else if (strcmp(what1, "nithramus_1") == 0)
        {
            kettle->set(time, NITHRAMUS, 1, false);
            return true;
        }
        else if (strcmp(what1, "buttered_sturgeon_food_1") == 0)
        {
            return false;
        }
        else if (strcmp(what1, "draenic_intellect_potion_1") == 0)
        {
            kettle->set(time, DRAENIC_INTELLECT_POTION, 1, false);
            return true;
        }
        else if (strcmp(what1, "arcane_charge_1") == 0 || strcmp(what1, "arcane_charge_2") == 0 || strcmp(what1, "arcane_charge_3") == 0 || strcmp(what1, "arcane_charge_4") == 0)
        {
            kettle->set(time, ARCANE_CHARGE, what1[14] - 48, false);
            return true;
        }
        else if (strcmp(what1, "arcane_missiles_1") == 0 || strcmp(what1, "arcane_missiles_2") == 0 || strcmp(what1, "arcane_missiles_3") == 0)
        {
            kettle->set(time, ARCANE_MISSILES, what1[16] - 48, false);
            return true;
        }

        else if (strcmp(what1, "greater_draenic_intellect_flask_1") == 0)
        {
            return false;
        }
        else if (strcmp(what1, "mark_of_warsong_10") == 0)
        {
            kettle->set(time, WEAPON_ENCHANT, 1, false); // gains
            return true;
        }
        else if (strcmp(what1, "presence_of_mind_1") == 0)
        {
            // ignore this, handled by with presence_of_mind action
            return false;
        }
        else if (strcmp(what1, "casting_1") == 0)
        {
            kettle->cast(time, buff);
            return true;
        }
        else if (strcmp(what1, "temporal_power_1") == 0 || strcmp(what1, "temporal_power_2") == 0 || strcmp(what1, "temporal_power_3") == 0)
        {
            kettle->set(time, TEMPORAL_POWER, what1[15] - 48, false);
            return true;
        }
        else if (strcmp(what1, "arcane_power_1") == 0)
        {
            // ignore, handled by action
            return false;
        }
        else if (strcmp(what1, "mark_of_doom_1") == 0)
        {
            kettle->markOfDoom(time, true, who);  // gains mark of doom
            qDebug()<<buff<<"\n";
            return true;
        }
        else
        {
            throw buff;
        }
    }
    else if (strcmp(action, "loses") == 0)
    {
        what1 = t.token(2);
        if (strcmp(what1, "incanters_flow") == 0)
        {
            return false;
        }
        else if (strcmp(what1, "mage_armor") == 0)
        {
            return false;
        }
        else if (strcmp(what1, "bloodlust") == 0)
        {
            kettle->set(time, HEROISM, 0, false);
            return true;
        }
        else if (strcmp(what1, "exhaustion") == 0)
        {
            return false;
        }
        else if (strcmp(what1, "nithramus") == 0)
        {
            kettle->set(time, NITHRAMUS, 0, false);
            return true;
        }
        else if (strcmp(what1, "buttered_sturgeon_food") == 0)
        {
            return false;
        }
        else if (strcmp(what1, "draenic_intellect_potion") == 0)
        {
            kettle->set(time, DRAENIC_INTELLECT_POTION, 0, false);
            return true;
        }
        else if (strcmp(what1, "arcane_charge") == 0)
        {
            kettle->set(time, ARCANE_CHARGE, -1, true);
            return true;
        }
        else if (strcmp(what1, "arcane_missiles") == 0)
        {
            kettle->set(time, ARCANE_MISSILES, -1, true);
            return true;
        }
        else if (strcmp(what1, "greater_draenic_intellect_flask") == 0)
        {
            return false;
        }
        else if (strcmp(what1, "presence_of_mind") == 0)
        {
            kettle->set(time, PRESENCE_OF_MIND, 0, false);
            return true;
        }
        else if (strcmp(what1, "casting") == 0)
        {
            kettle->cast(time, buff);
            return true;
        }
        else if (strcmp(what1, "temporal_power") == 0)
        {
            kettle->set(time, TEMPORAL_POWER, -1, true);
            return true;
        }
        else if (strcmp(what1, "arcane_power") == 0)
        {
            kettle->set(time, ARCANE_POWER, 0, false);
            return true;
        }
        else if (strcmp(what1, "mark_of_warsong") == 0)
        {
            kettle->set(time, WEAPON_ENCHANT, 0, false); // loses
            return true;
        }
        else if (strcmp(what1, "mark_of_doom") == 0)
        {
            kettle->markOfDoom(time, false, who); // looses mark of doom
            return true;
        }
        else if ((amount = asInt(what1)) > 1)
        {
            what2 = t.token(3);
            if (strcmp(t.token(3), "intellect") == 0)
            {
                kettle->change(INTELLECT,time, -amount);
            }
            else if (strcmp(t.token(3), "mastery_rating") == 0)
            {
                kettle->change(MASTERY, time, -amount);
            }
            else if (strcmp(t.token(3), "haste_rating") == 0)
            {
                kettle->change(HASTE, time, -amount);
            }
            else if (strcmp(t.token(3), "crit_rating") == 0)
            {
                kettle->change(CRIT,time, -amount);
            }
            else
            {
                throw buff;
            }
            return true;
		} if (strcmp(what1, "bleeding") == 0 || strcmp(what1, "mortal_wounds")==0 || strcmp(what1, "Health") == 0)
		{
			return false;
		}
        else
        {
            throw buff;
        }
    }
    else if (strcmp(action, "performs") == 0)
    {
        what2 = t.token(2);
        if (strcmp(what2, "greater_draenic_intellect_flask")==0)
        {
            return false;
        } else if (strcmp(what2, "snapshot_stats")==0)
        {
            return false;
        } else if (strcmp(what2, "potion")==0)
        {
            return false; // handled as action
        } else if (strcmp(what2, "unstable_magic_explosion")==0 || strcmp(what2, "presence_of_mind") == 0 || strcmp(what2, "doom_nova") == 0)
        {
			return false;
		} else if (strcmp(what2, "arcane_blast")==0 || strcmp(what2, "arcane_missiles") == 0 || strcmp(what2, "arcane_barrage") == 0)
        {
            return false;
        }

        //std::cout << buff << endl;

        //vector<pair<float, actionT> > act;
        // use it to collect Kettle's actions
        return true;
    }
    else if (strcmp(action, "uses") == 0)
    {
        what2 = t.token(2);
        if (strcmp(what2, "Food") == 0)
        {
			return false;
        } else
		{
			qDebug() << what2 << "\n";
			return true;
		}
        return true;
    }
    else if (strcmp(action, "potion") == 0)
    {
        return false;
    }
    else if (strcmp(action, "schedules") == 0)
    {
        return false;
    }
    else if (strcmp(action, "doom_nova") == 0)
    {
        kettle->damage(time, t.token(3), action, asInt(t.token(5)));
        return true;
    }
    else if (strcmp(action, "choose_target") == 0)
    {
        std::cout << buff << endl;
        return true;
    }
    else if (strcmp(action, "consumes") == 0)
    {
        amount = asInt(t.token(2));
        if (strcmp(t.token(3), "mana") == 0)
        {
            kettle->changemana(time, -amount);
			return false;
        }
        else
        {
            std::cout << buff << endl;
        }
        return true;
    }
    else if (strcmp(action, "arcane_missiles") == 0)
    {
        kettle->damage(time, t.token(3), action, asInt(t.token(5)));
        return true;
    }
    else if (strcmp(action, "arcane_missiles_tick") == 0)
    {
        kettle->damage(time, t.token(3), action, asInt(t.token(5)));
        return true;
    }
    else if (strcmp(action, "arcane_barrage") == 0)
    {
        kettle->damage(time, t.token(3), action, asInt(t.token(5)));
        return true;
    }
    else if (strcmp(action, "arcane_blast") == 0)
    {
        kettle->damage(time, t.token(3), action, asInt(t.token(5)));
        return true;
    }
    else if (strcmp(action, "evocation") == 0)
    {
        // nothing to do here, can be used to follow how many ticks an evocation is lasting
        return true;
    }
    else if (strcmp(action, "unstable_magic_explosion") == 0)
    {
        kettle->damage(time, t.token(3), action, asInt(t.token(5)));
        return true;
    }
    else if (strcmp(action, "prismatic_crystal") == 0)
    {
        kettle->damage(time, t.token(3), action, asInt(t.token(5)));
        return true;
    }
    else if (strcmp(action, "presence_of_mind") == 0)
    {
        kettle->set(time, PRESENCE_OF_MIND, 1, false);
        kettle->record_action(time, action, t.token(3));
        return true;
    }
    else if (strcmp(action, "arcane_power") == 0)
    {
        kettle->set(time, ARCANE_POWER, 1, false);
        kettle->record_action(time, action, t.token(3));
        return true;
    }
    else if (strcmp(action, "nithramus") == 0)
    {
        kettle->set(time, NITHRAMUS, 1, false);
        kettle->record_action(time, action, t.token(3));
        return true;
    }
    else if (strcmp(action, "arises.") == 0)
    {
        return false;
    }
    else if (strcmp(action, "demises..") == 0)
    {
        return false;
    }
    else if (strcmp(action, "tries") == 0)
    {
        return false;
    }
    else if (strcmp(action, "summons") == 0)
    {
		what1 = t.token(2);

		kettle->summon(time, what1);
        return true;
    }
    else if (strcmp(action, "shoot") == 0 || strcmp(action, "frostbolt") == 0 || strcmp(action, "melee") == 0)
    {
        kettle->damage(time, t.token(3), action, asInt(t.token(5)));
        return true;
    }
    else if (strcmp(action, "dismisses") == 0)
    {
		what1 = t.token(2);
		kettle->dismiss(time, what1);
        return true;
    }
    else if (strcmp(action, "prismatic_crystal") == 0)
    {
        std::cout << buff << endl;
        return true;
    }
    else
    {
        throw action;
    }

    return false;
} // bool combatlog::readLine()



bool combatlog::step()
{
    bool b = false;
#ifdef QTHUNOR
    QTextCursor cursor = actionList->textCursor();
    QTextCharFormat format = cursor.charFormat();
#endif
    while (!in.eof() && !b)
    {
        b = readLine();
#ifdef QTHUNOR
        cursor.insertBlock();
        if (b)
        {
            format.setForeground(QColor(255, 0, 0));
        } else
        {
            format.setForeground(QColor(0, 255, 0));
        }
        cursor.insertText(buff, format);
        QScrollBar *vbar = actionList->verticalScrollBar();
        vbar->triggerAction(QAbstractSlider::SliderToMaximum);
#else
		if (b)
		{
			cout << "----->";
		}
		else
		{
		}
		cout<<buff<<endl;
#endif
    }

    return b;
} // bool combatlog::step(int nbSteps)


bool combatlog::parseCombatLog(int nbSteps)
{
    while (!in.eof() && nbSteps>0)
    {
        step();
        --nbSteps;
    }

    return true;
} // bool combatlog::step(int nbSteps)




void combatlog::report()
{
    for (std::map<std::string, int>::const_iterator it = actor.begin(); it != actor.end(); ++it)
    {
        cout << it->first << "  " << it->second << endl;
    }
} // void combatlog::report()
