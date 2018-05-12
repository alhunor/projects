#include <iostream>
#include <fstream>
#include <conio.h>
#include <vector>
#include <map>
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
		n->list(0, false);
		cout << endl;
		m = n;
		n = n->succ();
		m->erase();
	}
}


void process_FTSE100()
{
	parser p;
	p.parseFile("FTSE_100.htm");
	p.buildParseTree();

	node* n;
	n = p.find("td");
	do
	{
		n = p.find(n, "td");
		cout << n->child(0)->attribute << " , ";
		n = n->succ();
		n = p.find(n, "td");
		cout << n->child(0)->text << " , ";
		n = n->succ();
		n = p.find(n, "td");
		cout << n->child(0)->text << endl;
		n = n->succ();
	} while (n);
}

void process_comp(char* filename)
{
	parser p;
	p.parseFile(filename);
	if (!p.isValidParse()) return;
	p.buildParseTree();

	node *n;
	n = p.find("head");
	if (n)
	{
		// Remove all children
		while (n->nbChildren() > 0)
		{
			n->child(0)->erase();
		}
	}
	n = p.find("div");
	while (n)
	{
		if (n->attribute["id"] == "mw-navigation" ||
			n->attribute["id"] == "footer" ||
			n->attribute["id"] == "toc" ||
			n->attribute["id"] == "siteSub" ||
			n->attribute["id"] == "jump-to-nav" ||	
			n->attribute["id"] == "mwe-popups-svg" ||
			n->attribute["id"] == "contentSub" ||		
			n->attribute["role"] == "navigation" ||
			n->attribute["role"] == "note" ||
			n->attribute["id"] == "catlinks"
			|| n->attribute["class"] == "printfooter"
			|| n->attribute["class"] == "thumbinner"			
			)
		{
			p.remove(n);
		} else n = n->succ();
		n = p.find(n, "div");
	};

	n = p.find("SPAN");
	while (n)
	{
		if (n->attribute["class"] == "mw-editsection"
			)
		{
			p.remove(n);
		}
		else n = n->succ();
		n = p.find(n, "SPAN");
	};

	n = p.find("OL");
	while (n)
	{
		if (n->attribute["class"] == "references"
			)
		{
			p.remove(n);
		}
		else n = n->succ();
		n = p.find(n, "OL");
	};


	n = p.find("table");
	while (n)
	{
		if (n->attribute["class"] == "infobox vcard")
		{
			p.remove(n);
		}
		else n = n->succ();
		n = p.find(n, "table");
	};

	n = p.find("A");
	while (n)
	{
		if (n->attribute["class"] == "image")
		{
			p.remove(n);
		}
		else n = n->succ();
		n = p.find(n, "A");
	};

	n = p.find("UL");
	while (n)
	{
		p.remove(n);
		n = p.find(n, "UL");
	};

	n = p.find("CITE");
	while (n)
	{
		p.remove(n);
		n = p.find(n, "CITE");
	};

	n = p.find("SUP");
	while (n)
	{
		p.remove(n);
		n = p.find(n, "SUP");
	};

	n = p.find("IMG");
	while (n)
	{
		p.remove(n);
		n = p.find(n, "IMG");
	};


	ofstream out;
	out.open("out.html");
	p.list(0, true, out);
	out.close();

}


void main()
{
	
//	process_comp("3i.htm");
	process_comp("Royal_Mail.htm");

	return;

	process_FTSE100();

	parser p, p2;
	//p.open("gog.html");
	//p.open("rr.html");
	p2.parseFile("example2.html");
	p.parseString("<html> <head> </head> <body> </body> </html>"); 

	bool b = p.buildParseTree();
	b = p2.buildParseTree();
	//preOrder(&p.root, 0);
	//list_email_adresses(p);
//	list_links(p);
//	list_ngramms(p);
	//cout <<"Stripped:" <<strip(" \n Bab h! \n\n\n\n") << endl;;

	//remove(p, "H2");
	//list(p.getRoot()->children[0], 0, true); 
//	remove(p, "P");
	//remove(p, "A");


	p.list(0, true);

	node* n = p.find("BOdY");
	if (n)
	{
		node* m = p2.find("H2");
		n->attachChild(m); // adds m as last child of n
		m = p2.find("H1");
		n->attachChild(m); // adds m as last child of n
		//n->list(0, true);
	}
	p.list(0, true);

	cout << "original\n";

	p2.list(0, true);

	ofstream out;
	out.open("out.thml");
	p.list(0, true, out);
	out.close();

//_exit:
	cout << "Press any key to exit.." << endl;
	_getch();
} // void main()