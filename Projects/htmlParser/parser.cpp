#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>
#include "parser.h"

#pragma warning ( disable : 4267)

//using namespace std;

std::string Document = "Document";

std::set<std::string> tagsWithoutEnd = { "BR", "HR", "IMG", "META", "LINK", "INPUT", "WBR", "PARAM" };
std::set<std::string> tagsWithOptionalEnd = { "P" };


void parser::remove(node *& n)
{
	node* m = n;
	n = n->succ();
	m->erase();
	delete m;
}


bool parser::parseString(const char* buff)
{
	in = new std::stringstream(buff, std::ios_base::in);
	return (validParse = true);
}

bool parser::parseFile(const char fileName[])
{
	std::ifstream* ifs = new std::ifstream();
	ifs->open(fileName);
	in = ifs;

	// check that file exists and return error message if not
	if (!ifs->is_open())
	{
		return validParse = false; // mising or otherwise unavailable file
	}

	std::streambuf* sb = ifs->rdbuf();
	unsigned char buff[3];
	sb->sgetn((char*)buff, 3);
	if (buff[0] == 239 && buff[1] == 187 && buff[2] == 191) // HEX EF, BB BF to signal Unicode UTF-8 encoding
	{
		return validParse = false; // Cannot handle unicode
	}

	ifs->seekg(0, ifs->beg);

	return validParse = true;
}


bool isPrefix(const std::string& pref, const std::string& str)
{
	if (pref.length() > str.length()) return false;
	for (int i = 0; i < pref.length(); ++i)
	{
		if (pref[i] != str[i]) return false;
	}
	return true;
}


bool parser::getToken()
{
	 if (!validParse) return false;
	if (hasTag)
	{
		hasTag = false;
		return true;
	}

again:
	tag = "";
	attribute = "";
	text = "";
	unsigned char c;
	states state = BEGIN;
	if (in->eof()) return false;

	do
	{
		c = in->get();
		if (in->eof())
		{
			return false;
		}
		switch (state)
		{
		case BEGIN:
			if (c == '<')
			{
				state = TAG;
			}
			else
			{
				state = TEXT;
				text += c;
			}
			break;
		case TEXT:
			if (c == '<')
			{
				state = END;
			}
			else if (c == '>')
			{
				state = END;
			}
			else text += c;
			break;
		case TAG:
			if (c == ' ')
			{
				state = ATTRIBUTE;
			}
			else if (c == '>')
			{
				state = END;
			}
			else tag += toupper(c);
			break;
		case ATTRIBUTE:
			if (c == '>')
			{
				state = END;
			}
			else attribute += c;
			break;
		default:
			throw "Error.";
			return validParse = false;
		}
	} while (state != END);
	if (c == '<') in->unget();
	if (tag != "" && text != "")
	{
		throw "TAG and token are exclusive";
		return validParse = false;
	}
// Special hendling of scripts, we want to avoid building  a Javascript parser - exclude all data between <script>...</script>
	if (tag == "SCRIPT")
	{
		// eat all data between <script>...</script>
		std::string s = "";
		do
		{
			c = toupper(in->get());
			if (in->eof())
			{
				return false;
			}
			s += c;
			if (!isPrefix(s, "</SCRIPT>"))
			{
				s = "";
			}
			else if (s == "</SCRIPT>")
			{
				goto again;
			}
		} while (true);
	}

	return true;
} // void parser::getToken()

bool isWhiteChars(std::string& s)
{
	for (unsigned int i = 0; i < s.length(); ++i)
	{
		if (s[i] != ' ' && s[i] != '\t' && s[i] != '\n' && s[i] != 10) return false;
	}
	return true;
}


bool isWithoutEnd(std::string& tag)
{
	return tagsWithoutEnd.find(tag) != tagsWithoutEnd.end();
}

bool hasOptionalEnd(std::string& tag)
{
	return tagsWithOptionalEnd.find(tag) != tagsWithOptionalEnd.end();
}



bool parser::buildParseTree(node * n, std::string ctag)
{
	if (!validParse)
	{
		return false;
	}

	node* child;

	std::string ltag;
	while (getToken())
	{
		if (text == "\n") continue;
		if (tag[0] == '!') // skip javascript ?
		{
			continue;
		}
		if (tag[0] == '/')
		{
			if (ctag == tag.substr(1, tag.length() - 1))
			{
				return true;
			}
			else
			{
				if (hasOptionalEnd(ctag))
				{
					return hasTag = true;
				}
				else
				{
					std::cout << "Parsing error, not a valid HTML tree."<<ctag<<"  "<<tag<< std::endl;
					return false;
				}
			}
		}

		if (tag != "")
		{
			if (hasOptionalEnd(tag) && hasOptionalEnd(ctag))
			{
				return hasTag = true;
			}
			child = new node(tag, attribute);
			child->parent = n;
			child->nr = n->children.size();
			n->children.push_back(child);
			if (! isWithoutEnd(tag) )
			{
				if (!buildParseTree(child, tag)) return false;
			}
			//++pos;
		} else if (! isWhiteChars(text))
		{
			child = new node(text);
			child->parent = n;
			child->nr = n->children.size();
			n->children.push_back(child);
		}
	}
	return true;
} // bool parser::buildParseTree(node * n, string ctag)




node* parser::find(node* start, searchfunction func, int method)
{
	node* n = start;
	while (n)
	{
		//cout << n->text << endl;
		if (func(n)) return n;
		n = n->succ();
	}
	return NULL;
}


node* parser::find(node* start, finder* funcObj)
{
	if (!funcObj) return NULL;
	node* n = start;
	while (n)
	{
		//cout << n->text << endl;
		if (funcObj->match(n)) return n;
		n = n->succ();
	}
	return NULL;
}


