#include <iostream>
#include <conio.h>
#include "arcaneMage.h"
#include "combatlog.h"

using namespace std;


void main()
{
	//int i = atoi("scx");

	arcaneMage kettle;
	parseCombatLog("..\\simCraftGui\\combatlog-elite.txt", kettle, 9999);

	ofstream out;
	out.open("combatlog-elite.csv");
	for (int i = 0; i < kettle.dmg.size(); ++i)
	{
		out << kettle.dmg[i].time << "," << kettle.dmg[i].action << ","<< kettle.dmg[i].amount << ","<<kettle.dmg[i].target<<endl;
	}
	out.close();
	
	std::cout << "Press any key to exit...\n";
	_getch();
}