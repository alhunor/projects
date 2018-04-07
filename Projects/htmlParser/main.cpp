#include <iostream>
#include <fstream>
#include <conio.h>
#include <vector>
#include <regex>
#include <string>
#include "parser.h"

#pragma warning ( disable : 4267)


using namespace std;

void preOrder(node* n, int level)
{
	if (!n) return;
	cout << nspaces(level, true) << ((n->tag == "") ? "text" : n->tag) << endl;
	cout << ((n->tag == "") ? "text" : n->tag) << endl;
	for (int i = 0; i < n->nbChildren(); ++i)
	{
		preOrder(n->child(i), level + 1);
	}
}

bool is_valid_email(node* n)
{
	static const std::regex pattern("(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+");
	if (n->text == "") return false;
	// try to match the string with the regular expression
	return std::regex_match(n->text, pattern);
}

int counter = 0;

bool is_valid_email2(node* n)
{
	if (n->text == "") return false;
	cout << counter << " -> "<< n->text<<endl;
	++counter;
	if (counter == 73)
	{
		counter = 73;
	}
	enum {EMPTY, BEFORE, AT, AFTER, VALID} status = EMPTY;
	size_t len = n->text.length();
	unsigned char c = ' ';
	unsigned char prevCharacter;
	for (size_t i = 0; i < len; ++i)
	{
		prevCharacter = c;
		c = n->text[i];
		switch (status)
		{
			case EMPTY: // no character processed
				if (! isalnum(c)) return false; // emai must start with aplhabumeric
				status = BEFORE; // before '@'
				break;			
			case BEFORE: // before or on '@' character
				if (!isalnum(c) && c !='_')
				{
					if (c == '.')
					{
						if (prevCharacter == '.') return false; //successive dots are invalid, e.g. "a..b@c.com"
					} else if (c == '@')
					{
						if (prevCharacter == '.') return false; //dot preceeding '@' is invalid, e.g. "a.@c.com"
						status = AT;
					} else return false; // any other non alphanumeric character is forbidden here
				}
				break;
			case AT: // the next character folllowing "@" must be alphanumeric
				if (!isalnum(c)) return false;
				status = AFTER;
				break;
			case AFTER: // after '@'
			case VALID: // after '@' and after a '.'. Email is potentially valid unless it get invalidated later or it end on a '.'
				if (!isalnum(c))
				{
					if (c == '.')
					{
						if (prevCharacter == '.') return false; //successive dots are invalid, e.g. "ab@c..com"
						status = VALID;
					} else return false; // any other non alphanumeric character is forbidden here
				}
		} // switch (status)
	} // for (size_t i = 0; i < len; ++i)
	return status == VALID && c!='.'; // last character cannot be a dot e.g., "ab@c.com."
} // bool is_valid_email2(node* n)


void list_email_adresses(parser& p)
{
	node * n = p.getRoot()->leftmost();
	while (n = p.find(n, is_valid_email2, 0))
	{
		cout << n->text << endl;
		n = n->succ();
	}
}


bool is_href(node* n)
{
	if (n->tag != "A") return false;
	return true;

}

void list_links(parser& p)
{
	node * n;
	n = p.getRoot()->leftmost();
	while (n = p.find(n, is_href, 0))
	{
		cout << n->attribute << endl;
		n = n->succ();
	}
}


void list_ngramms(parser& p)
{
	node * n = p.getRoot()->leftmost();
	while (n = p.find(n, is_href, 0))
	{
		node * m = n->leftmost();
		while (m)
		{
			if (m->text != "" && !is_valid_email(m))
			{
				cout << m->text << endl;
			}
			m = m->succ(n);
		}
		n = n->succ();
	}
}




void remove(parser& p, const char* tag)
{
	node *m;
	node * n = p.getRoot()->leftmost();
	//finder f("H2");
	while (n = p.find(n, tag))
	{
		list(n, 0, false);
		cout << endl;
		m = n;
		n = n->succ();
		m->erase();
	}
}


void main()
{
	parser p;
	//p.open("gog.html");
	//p.open("rr.html");
	p.open("example2.html");
	bool b = p.buildParseTree();
	//preOrder(&p.root, 0);
	//list_email_adresses(p);
//	list_links(p);
//	list_ngramms(p);
	//cout <<"Stripped:" <<strip(" \n Bab h! \n\n\n\n") << endl;;

	remove(p, "H2");
	//list(p.getRoot()->children[0], 0, true); 
//	remove(p, "P");
	remove(p, "A");


	list(p.getRoot(), 0, true);

	node* n = p.find("P");
	list(n, 0, false);
	n = n->next();
	list(n, 0, false);

	list(p.getRoot(), 0, true);

//_exit:
	cout << "Press any key to exit.." << endl;
	_getch();
}