#include <iostream>
#include <set>
#include <string>
#include <vector>
#include "node.h"

#pragma warning ( disable : 4267)


std::string Empty = "";


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
	while (tmp)
	{
		tmp2 = tmp;
		tmp = succ(tmp);
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

