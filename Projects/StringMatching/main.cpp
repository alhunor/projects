#include <conio.h>
#include <fstream>
#include <iostream>
#include <vector>
#include "fsa.h"

using namespace std;


void accepted()
{
	cout << "accepted"<<endl;
}




bool validEnglishWord(char* s)
{
// return true if the word is valid, false otherwise. Valid words contain only lower case and upercase letters. 
// the presence of numbers, apostrophes hyphenation fails the test. If f is not NULL terminated the function will overflow.
	while (*s)
	{
		if ( ! ( (*s >= 'A' && *s <= 'Z') || (*s >= 'a' && *s <= 'z'))) return false;
		++s;
	}
	return true;
} // bool validEnglishWord(char* s)


void processWordListFile(char* filename)
{
	ifstream in;
	ofstream validWords, invalidWords;
	in.open(filename);
	char word[128];
	validWords.open("validWords.txt");
	invalidWords.open("invalidWords.txt");
	while (!in.eof())
	{
		in.getline(word, 128);
		if (in.gcount() > 128) throw "line is too long";
		if (validEnglishWord(word))
		{
			validWords << word << endl;
		}
		else
		{
			invalidWords << word << endl;
		}
	}
	invalidWords.close();
	validWords.close();
	in.close();
}  // void processWordListFile(char* filename)

// http://www.semantilog.org/wn2sql.html <-list of english word with meaning in mysql format

//A file with the OXT file extension is an OpenOffice.org Extension file. These files are compressed using the same format as ZIP files. 
//If you'd like to view the contents of an OXT file, rename the extension from OXT to ZIP and use your favorite free file extractor program
// the dict file therein use the hunspell format, e.g. word/ext - extension is a link to another file containing a list of allowed suffices

//from performanceTesting.cpp
void test();

void accept();

void main()
{
	// construct a character mapper from ascii to the alphabet
	//processWordListFile("americanEnglishWords.txt");


	fsa fsa(256);
	fsa.addString("abb", &accept);
	fsa.addString("aaa", &accept);
	fsa.addString("ba", &accept);
	fsa.addString("bba", &accept);
	fsa.addString("ab", &accept);
	fsa.alternative3();
	fsa.finaliseBuild();

	FUNCPTR ptr = fsa.matchPattern3("ab");
	ptr = fsa.matchPattern3("aaa");
	ptr = fsa.matchPattern3("aa");
	ptr = fsa.matchPattern3("ac");


	test();

//	fsa.listStates();
	std::cout << "Terminated. Press any key to exit" << endl;
	_getch();
}

