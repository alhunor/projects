#pragma once
#include "arcaneMage.h"
#include "tokenizer.h"
#include "utils.h"
#include <fstream>
#include <iostream>

#ifdef QTHUNOR
#include <qplaintextedit.h>
#endif

bool parseCombatLog(char* fileName, arcaneMage& kettle, int nbSteps);

const int MAX_LEN = 512;

class combatlog
{
public:
    combatlog() : initialised(false), time (0) {}
    ~combatlog() {if (initialised) in.close();}

    void report();

    bool init(const char* fileName, arcaneMage& player
    #ifdef QTHUNOR
    , QPlainTextEdit* edit
    #endif
    );
    bool parseCombatLog(int nbSteps);
    bool step();
    float getTime() {return time;}
protected:
    bool readLine(); // return true if line is an action, false otherwise
    bool initialised;
    arcaneMage* kettle;
#ifdef QTHUNOR
    QPlainTextEdit* actionList;
#endif
    std::ifstream in;
    char buff[MAX_LEN];
    int len; // length of the string in <buff>

    tokenizer t;
    int count;
    float time;
    std::map<std::string, int> actor;
    std::map<std::string, int> action;
};
