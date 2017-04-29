#include <algorithm>
#include <conio.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <io.h>
#include <map>
#include <string>


#include "actors.h"
#include "events.h"
#include "..\\mystuff\\hash.h"
#include "tokenizer.h"


#define MAX_LEN 2048

extern guidClass guids;

using namespace std;


void main()
{
// http://www.tutorialspoint.com/sqlite/sqlite_c_cpp.htm
// https://www.sqlite.org/c_interface.html

	db = openDatabase(":memory:");

	wowEvent::init();
	wowEvent::readConfigFile("events.txt");


	_setmode(_fileno(stdout), _O_U16TEXT);

	ifstream in;
	char dest[MAX_LEN];
	int count, count2;
//	in.open("archi.txt");
//	in.open("arcane full 2.txt");
/*	char * c = "æthushu";
	wchar_t* w = convert(c);
	//std::wcout << c << std::endl;
	std::wcout << w << std::endl;
*/
	in.open("archi-log.txt");
//	in.open("summons.txt");
	//in.open("arcane3.txt"); // with prismatic crytal in garrison
	//in.open("frost.txt"); // with frozen orbs in garrison
	if (in.bad())
	{
		cout << "Error opening input file." << endl;
	}
	tokenizer t;
	count = 1;
	actors actors;
	while (!in.eof())
	{
		in.getline(dest, MAX_LEN);
		count2 = (int)in.gcount();

		if (count2==MAX_LEN)
		{
			throw "Line is too long";
		}
		if (count2 == 0)
		{
			break;
		}
		wowEvent * eve = createEvent(dest, count2, t);
		if (eve)
		{
			actors.add(eve);
		}

		++count;

		if (count >= 581)
			//if (count >= 368)
		{
			wcout << count << endl;
		}

	}

	for (petMapT::const_iterator itt = actors.pets.begin(); itt != actors.pets.end(); ++itt)
	{
		wcout << itt->second->guid<<"  "<< itt->second->name.c_str() << " from " << (itt->second->owner ? itt->second->owner->name.c_str() : " ") << endl;
	}

	actors.stat();

	mage* kettle = (mage*)actors.locatePlayer("Player-1587-073AE460"); // Kettle-Arathor
	vector<int> v;

	wcout << "Battle between " << actors.minTime << " - " <<actors.maxTime << endl;

	kettle->computeDamageTimeline(v, actors.minTime, actors.maxTime, 100);
	
	guids.save();
	wowEvent::finalise();

	// TODO save the memory DB to the desk once completed https://www.sqlite.org/backup.html

	saveDatabase(db, "wow2.db");
	sqlite3_close(db);

	wcout<<"Press any key to exit..";
	_getch();
}