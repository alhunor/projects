#pragma once
#include "arcaneMage.h"
#include "tokenizer.h"
#include "utils.h"
#include <fstream>
#include <iostream>
#include <qplaintextedit.h>

bool parseCombatLog(char* fileName, arcaneMage& kettle, int nbSteps);

const int MAX_LEN = 512;

class combatlog
{
public:
    combatlog() : initialised(false) {}
    ~combatlog() {if (initialised) in.close();}

    void report();

    bool init(const char* fileName, arcaneMage& player, QPlainTextEdit* edit);
    bool parseCombatLog(int nbSteps);
    bool step();
protected:
    bool readLine(); // return true if line is an action, false otherwise
    bool initialised;
    arcaneMage* kettle;
    QPlainTextEdit* actionList;
    std::ifstream in;
    char buff[MAX_LEN];
    int len; // length of the string in <buff>

    tokenizer t;
    int count;
    std::map<std::string, int> actor;
    std::map<std::string, int> action;
};
