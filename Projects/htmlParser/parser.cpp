#include <iostream>
#include <set>
#include <string>
#include <vector>
#include "parser.h"

#pragma warning ( disable : 4267)

//using namespace std;

std::string Empty = "";
std::string Document = "Document";

std::set<std::string> tagsWithoutEnd = { "BR", "HR", "IMG", "META", "LINK", "INPUT", "WBR", "PARAM" };
std::set<std::string> tagsWithOptionalEnd = { "P" };


bool parser::open(const char fileName[])
{
	in.open(fileName);
	// TODO check that file exists and return error message if not

	return validParse = true;
} // bool parser::open(const char* fileName)




void node::erase()
{
	// XXX TODO check this function
	node* p = parent;

	// remove from parent
	std::vector<node* >::iterator it = std::find(p->children.begin(), p->children.end(), this);
	std::vector<node* >::iterator it2 = it;
	for (; it != p->children.end(); ++it)
	{
		(*it)->nr--;
	}

	p->children.erase(it2);

	// then delete subtree;
	parent = NULL;
	node * tmp = leftmost();
	node * tmp2;
	while (tmp)
	{
		tmp2 = tmp;
		tmp = succ(tmp);
		delete tmp2;
	}
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
	_ASSERT(validParse);
	if (hasTag)
	{
		hasTag = false;
		return true;
	}

again:
	tag = "";
	attribute = "";
	text = "";
	char c;
	states state = BEGIN;
	do
	{
		c = in.get();
		if (in.eof())
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
	if (c == '<') in.unget();
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
			c = toupper(in.get());
			if (in.eof())
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
	_ASSERT(validParse);

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



bool inset(char c, const char* mask)
{
	size_t len = strlen(mask);
	for (int i = 0; i < len; ++i)
	{
		if (c == mask[i]) return true;
	}
	return false;
}



std::string nspaces(int level, int prettify)
{
	if (level < 1 || !prettify) return "";

	std::string s = "";
	while (level > 0)
	{
		s += " ";
		--level;
	}
	return s;
}

std::string strip(const std::string& s, const char* premask = " \n\t\r", const char* postmask = " \n\t\r")
{
	int i = 0;
	std::string ret = "";
	while (inset(s[i], premask) && i < s.length())
	{
		++i;
	}
	while (i < s.length())
	{
		ret += s[i];
		++i;
	}
	i = ret.length() - 1;

	while (i > -1 && inset(ret[i], postmask))
	{
		ret.pop_back();
		--i;
	}
	return ret;
} // string strip(const string& s, const char* premask = " \n\t\r", const char* postmask = " \n\t\r")


bool list(node* n, int k, bool prettify)
{
	if (!n) return true;

	if (n->text != Empty)
	{
		std::string s = strip(n->text, " \n\t\r", " \n\t\r");
		std::cout << nspaces(k, prettify) << s;
		if (prettify) std::cout << std::endl;
		return true;
	}
	std::cout << nspaces(k, prettify) << "<" << n->tag;
	if (n->attribute != Empty)
	{
		std::cout << " " << n->attribute;
	}
	std::cout << ">";
	if (n->nbChildren() > 0 && prettify) std::cout << std::endl;
	for (int i = 0; i < n->nbChildren(); ++i)
	{
		list(n->child(i), k + 1, prettify);
	}
	if (!isWithoutEnd(n->tag))
	{
		std::cout << nspaces(k, prettify) << "</" << n->tag << ">";
	}

	if (prettify) std::cout << std::endl;
	return true;
} // bool list(node* n, int k)