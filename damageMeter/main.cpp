#include <iostream>
#include <fstream>
#include <conio.h>
#include <map>
#include <string>
#include <algorithm>
#include "tokenizer.h"
#include "events.h"
#include "actors.h"

using namespace std;

#define MAX_LEN 2048

char* g;



void main()
{
	ifstream in;
	char dest[MAX_LEN];
	int count, count2;
//	in.open("archi.txt");
	in.open("arcane full 2.txt");
	tokenizer t;

	count = 1;
	actors actors;
	while (!in.eof())
	{
		in.getline(dest, MAX_LEN);
		count2 = (int)in.gcount();
		g = dest;

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
		/*cout << count++ << " -> ";
		for (i=0; i<t.nbTokens()-1; ++i)
		{
			cout<<t.token(i)<<"  ";
		
		cout<<endl;
		}*/
		++count;
		if (count == 33)
		{
			count = 34;
		}
	}
	actors.stat();
	cout<<"Press any key to exit..";
	_getch();
}