#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>
#include "parser.h"

#pragma warning ( disable : 4267)

//using namespace std;

std::string Document = "Document";

std::set<int> tagsWithoutEndSet;
std::set<int> tagsWithOptionalEndSet;

/*
tag = bm.value(_tag);
if (tag < 0) // not found
{
	tag = bm.insert(_tag);
}*/

// removes n and its subtree. Then sets it to its successor
void parser::remove(node *& n)
{
	node* m = n;
	n = n->succ();
	m->erase();
	delete m;
}


void parser::init()
{
	std::set<std::string> tagsWithoutEnd = { "BR", "HR", "IMG", "META", "LINK", "INPUT", "WBR", "PARAM" };
	std::set<std::string> tagsWithOptionalEnd = { "P" };

	bm.insert(Document);
	EmptyTag = bm.insert(Empty);

	std::set<std::string>::const_iterator it;
	int tag;
	for (it = tagsWithoutEnd.begin(); it != tagsWithoutEnd.end(); ++it)
	{
		tag = bm.insert(*it);
		tagsWithoutEndSet.insert(tag);

	}
	for (it = tagsWithOptionalEnd.begin(); it != tagsWithOptionalEnd.end(); ++it)
	{
		tag = bm.insert(*it);
		tagsWithOptionalEndSet.insert(tag);
	}
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
		return (validParse = false); // mising or otherwise unavailable file
	}

	std::streambuf* sb = ifs->rdbuf();
	unsigned char buff[3];
	sb->sgetn((char*)buff, 3);
	if (buff[0] == 239 && buff[1] == 187 && buff[2] == 191) // HEX EF, BB BF to signal Unicode UTF-8 encoding
	{
		return (validParse = false); // Cannot handle unicode
	}

	ifs->seekg(0, ifs->beg);

	return (validParse = true);
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
			return (validParse = false);
		}
	} while (state != END);
	if (c == '<') in->unget();
	if (tag != "" && text != "")
	{
		throw "TAG and token are exclusive";
		return (validParse = false);
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


bool isWithoutEnd(int tag)
{
	return tagsWithoutEndSet.find(tag) != tagsWithoutEndSet.end();
}

bool hasOptionalEnd(int tag)
{
	return tagsWithOptionalEndSet.find(tag) != tagsWithOptionalEndSet.end();
}



bool parser::buildParseTree(node * n, int ctag)
{
	if (!validParse)
	{
		return false;
	}

	node* child;

	//std::string ltag;
	while (getToken())
	{
		if (text == "\n") continue;
		if (tag[0] == '!') // skip javascript ?
		{
			continue;
		}

		int tagValue; 
		if (tag[0] == '/')
		{
			tagValue = bm.insert(tag.substr(1, tag.npos));
			if (ctag == tagValue)
			{
				return true;
			} else 
			{
				if (hasOptionalEnd(ctag))
				{
					return hasTag = true;
				} else
				{
					std::cout << "Parsing error, not a valid HTML tree."<<ctag<<"  "<<tag<< std::endl;
					return false;
				}
			} 
		} else
		{
			tagValue = bm.insert(tag); 
		}

		if (tag != "")
		{
			if (hasOptionalEnd(tagValue) && hasOptionalEnd(ctag))
			{
				return hasTag = true;
			}
			child = new node(tagValue, attribute);
			child->parent = n;
			child->nr = n->children.size();
			n->children.push_back(child);
			if (! isWithoutEnd(tagValue) )
			{
				if (!buildParseTree(child, tagValue)) return false;
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




node* parser::find(node* start, searchfunction func)
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


node* parser::find(node* start, const finder* funcObj)
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


bool parser::removeEmptyTags() // removes all tags that have no text in them
{
	if (!validParse) return false;
	node* n = getRoot()->leftmost();
	while (n)
	{
		//cout << n->text << endl;
		if (n->text == Empty && n->nbChildren()==0 && // a tag with no children
			tagsWithoutEndSet.find(n->tag) == tagsWithoutEndSet.end() // that is not a lone tag
			&& tagsWithOptionalEndSet.find(n->tag) == tagsWithOptionalEndSet.end()) // that is not a lone tag
		{
			remove(n);
		} else n = n->succ();
	}
	return true;
}
