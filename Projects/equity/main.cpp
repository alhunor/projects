#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

string getLastLine(ifstream& fin)
{
	bool jumped = false;
	char ch;
	int tmp;

	if(! fin.is_open())
		throw "Rrror, file is not open";
	
    // go to one spot before the EOF
	fin.seekg(-1,ios_base::end);
	tmp = (int)fin.tellg();

    while(true)
	{
        // Get current byte's data
		fin.get(ch);
		if (ch!='\n' && ch!=13) jumped = true;
		if(tmp <= 1) // If the data was at or before the 0th byte
		{ 
			fin.seekg(0);                       // The first line is the last line
			break;
        } else if(ch == '\n') // If the data was a newline
		{
			if (jumped) break;
        }
		// Move to the front of that data, then to the front of the data before it
		fin.seekg(--tmp);
    } // while(keepLooping)
    string lastLine;
	getline(fin,lastLine);
	return lastLine;
} // getLastLine

int main()
{
    char* filename = "m.txt";
    ifstream fin;
    fin.open(filename);

    cout << "Result: " << getLastLine(fin) << '\n';     // Display it

    fin.close();
    char c;
	cin>>c;
} //main()