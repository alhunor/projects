#include <iostream>
#include <fstream>
#include <conio.h>
#include <vector>
#include <string>
#include "parser.h"


using namespace std;

void main()
{

	parser p;
	//p.open("gog.html");
	p.open("example.html");
	p.buildParseTree();

//_exit:
	cout << "Press any key to exit.." << endl;
	_getch();
}