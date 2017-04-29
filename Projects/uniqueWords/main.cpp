#include <fstream>
#include <iostream>
#include <string>
#include <map>


using namespace std;


/*

struct ltstr2
{
  bool operator()(const string s1, const string s2) const
  {
    return s1 < s2;
  }
};

typedef map<string, int, ltstr2> mapT;

void process(char* filename, mapT  &map)
{
	int nbWords=0;
	int nbUniqueWords=0;
	cout<<filename<<endl;
	fstream f;
	f.open(filename,  ios::in | ios::binary);
	if (f.bad())
	{
		f.close();
		throw "Could not open file";
	}

	f.unsetf(ios_base::skipws);

	char buffer[128]; 
	int pos=0;
	
	char c;
	int state=0;
	mapT::iterator it;

	while (!f.eof())
	{
		f>>c;
		if ( (c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9') || c=='ç' || c=='ô' || 
			c=='è' || c=='ê' || c=='é' || c=='û' || c=='ù' || c=='à' || c=='â' || c=='î' || c=='ï' || c=='\'')
		{
			state=1;
			buffer[pos]=c;
			pos++;
		} else 
		{
			if (pos>0) 
			{
				buffer[pos]=0;
				string s(buffer);
				//it=map.find(buffer);
				it=map.find(s);
				if (it==map.end())
				{ // new element
					//map[buffer]=0;
					map[s]=0;
					nbUniqueWords++;
				} else
				{
//					map[buffer]++;
					map[s]++;
				}
				//cout<<buffer<<endl;
				pos=0; 
				nbWords++;
			}
			state=0;
		}
	};
	f.close();
	cout<<nbUniqueWords <<"unique words out of " <<nbWords<<" words."<<endl;
//  iterator find ( const key_type& x );
}

int main(int argc, char** argv)
{
	int nb;
	mapT map;
	if (argc>1)
	{
		for(nb=1; nb<argc; nb++)
		{
			char* filename=argv[nb];
			process(filename, map);
		}
		
	}
	for (mapT::const_iterator it=map.begin(); it!=map.end(); ++it)
		cout<<it->first<<"  "<<it->second<<endl;
	char c;
	cin>>c;
	return 0;
}
*/



struct ltstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};

typedef map<char*, int, ltstr> mapT;


void process(char* filename, mapT  &map)
{
	int nbWords=0;
	int nbUniqueWords=0;
	cout<<filename<<endl;
	fstream f;
	f.open(filename,  ios::in | ios::binary);
	if (f.bad())
	{
		f.close();
		throw "Could not open file";
	}

	f.unsetf(ios_base::skipws);

	char buffer[128]; 
	int pos=0;
	
	char c;
	int state=0;
	mapT::iterator it;

	while (!f.eof())
	{
		f>>c;
		if ( (c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9') || c=='ç' || c=='ô' || 
			c=='è' || c=='ê' || c=='é' || c=='û' || c=='ù' || c=='à' || c=='â' || c=='î' || c=='ï')
		{
			state=1;
			buffer[pos]=c;
			pos++;
		} else 
		{
			if (pos>0) 
			{
				buffer[pos]=0;
				char* tmp = new char[pos+1];
				memcpy(tmp, buffer, pos+1);

				it=map.find(tmp);
				if (it==map.end())
				{ // new element
					map[tmp]=0;
					nbUniqueWords++;
				} else
				{
					map[tmp]++;
				}
				//cout<<buffer<<endl;
				pos=0; 
				nbWords++;
			}
			state=0;
		}
	};
	f.close();
	cout<<nbUniqueWords <<" unique words out of " <<nbWords<<" words."<<endl;
}

int main(int argc, char** argv)
{
	int nb;
	mapT map;
	if (argc>1)
	{
		for(nb=1; nb<argc; nb++)
		{
			char* filename=argv[nb];
			process(filename, map);
		}
		
	}
	for (mapT::const_iterator it=map.begin(); it!=map.end(); ++it)
		cout<<it->first<<"  "<<it->second<<endl;
	char c;
	cin>>c;
	return 0;
}