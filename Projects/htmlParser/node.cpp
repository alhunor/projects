#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>
#include "node.h"

#pragma warning ( disable : 4267)


std::string Empty = "";

std::ostream& operator<<(std::ostream& io, const attribute& a)
{
	io << a.raw;
	return io;
}


void node::erase() // completely erases this node and its subtree from the tree
{
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
	while (tmp !=this)
	{
		tmp2 = tmp;
		tmp = tmp->succ(this);
		delete tmp2;
	}
}

void node::attachChild(node* m) // removes node <m> from its parent and reattaches it as n's last child
{
	if (!m) return;

	// remove <m> from its parent
	node* p = m->parent;
	if (p)
	{
		std::vector<node* >::iterator it = std::find(p->children.begin(), p->children.end(), m);
		std::vector<node* >::iterator it2 = it;
		for (; it != p->children.end(); ++it)
		{
			(*it)->nr--;
		}
		p->children.erase(it2);
	}

	// attach it as the last child
	m->parent = this;
	children.push_back(m);
	m->nr = nbChildren()-1;
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


bool node::list(int k, bool prettify, std::ostream& os)
{
	if (text != Empty)
	{
		std::string s = strip(text, " \n\t\r", " \n\t\r");
		os << nspaces(k, prettify) << s;
		if (prettify) os << std::endl;
		return true;
	}
	os << nspaces(k, prettify) << "<" << tag;
	if (attribute != Empty)
	{
		os << " " << attribute;
	}
	os << ">";
	if (nbChildren() > 0 && prettify) os << std::endl;
	for (int i = 0; i < nbChildren(); ++i)
	{
		child(i)->list(k + 1, prettify, os);
	}
	if (!isWithoutEnd(tag))
	{
		if (nbChildren() > 0) os << nspaces(k, prettify);
		os << "</" << tag << ">";
	}

	if (prettify) os << std::endl;
	return true;
} // bool node::list(node* n, int k)


bool isWhiteSpace(char c)
{
	return c == ' ' || c == '\t' || c == '\n' || c == 13;
}

bool isWAlphaNumeric(char c)
{
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9');
}

bool stringToMap(const char* s, std::map<std::string, std::string>& mapy)
{
	enum { Outside, Var, Outside2, Eq, Outside3, Value } state;
	state = Outside;
	int len = strlen(s);
	char c;

	mapy.clear();
	std::string VarName = Empty;
	std::string VarValue = Empty;
	for (int i = 0; i < len; ++i)
	{
		c = s[i];
		if (isWAlphaNumeric(c))
		{
			switch (state)
			{
			case Outside:
				state = Var;
				VarName += c;
				continue;
			case Value:
				VarValue += c;
				continue;
			case Var:
				VarName += c;
				continue;
			default:
				return false; // invalid string format
			}
		}
		else if (isWhiteSpace(c))
		{
			switch (state)
			{
			case Outside:
			case Outside2:
			case Outside3:
				// do nothing;
				continue;
			case Var:
				state = Outside2;
				continue;
			case Eq:
				state = Outside3;
				continue;
			case Value:
				VarValue += c;
				continue;
			default:
				return false; // invalid string format
			}
		}
		else if (c == '\"')
		{
			switch (state)
			{
			case Outside3:
			case Eq:
				VarValue = Empty;
				state = Value;
				continue;
			case Value:
				state = Outside;
				mapy[VarName] = VarValue;
				VarName = Empty;
				VarValue = Empty;
				// do nothing;
				continue;
			default:
				return false; // invalid string format
			}
		}
		else if (c == '=')
		{
			if (state == Outside2 || state == Var)
			{
				state = Eq;
			}
			else if (state == Value)
			{
				VarValue += c;
			}
			else return false; // invalid string format
		}
		else // handle anything else
		{
			if (state == Value)
			{
				VarValue += c;
				continue;
			}
			return false; // invalid string format
		}
	} // for (int i = 0; i < len; ++i)
	if (state != Outside)
	{
		mapy.clear();
		return false;
	}
	return true;
} // bool stringToMap(char* s)



const std::string& attribute::operator[] (const char* key )
{
	if (!hasmap)
	{
		stringToMap(raw.c_str(), attr);
		hasmap = true;
	}
	std::map<std::string, std::string>::const_iterator it = attr.find(key);
	if (it != attr.end())
	{
		return it->second;
	}
	return Empty;
}
