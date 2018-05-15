#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <set>
#include "node.h"

typedef enum { BEGIN, TEXT, TAG, ATTRIBUTE, END } states;


bool isWithoutEnd(int tag);

extern std::string Document;

typedef bool (searchfunction)(node* n);


class finder
{
public:
	virtual bool match(node* n) const = 0;
};


class findOne : public finder
{
public:
	findOne(int _tag) : tag(_tag) {}
	virtual bool match(node* n) const { return n->tag == tag; }

protected:
	int tag;
};


class findMany : public finder
{
public:
	findMany(biMap<std::string> bm, std::vector<std::string> tags)
	{
		std::vector<std::string>::const_iterator it;
		for (it = tags.begin(); it != tags.end(); ++it)
		{
			int tag = bm.key(*it);
			tagValues.insert(tag);
		}	
	}

	virtual bool match(node* n) const
	{
		bool b;
		b = (tagValues.find(n->tag) != tagValues.end());
		return b;
	}

protected:
	std::set<int> tagValues;
};





class parser
{
public:
	parser() : root(Document), validParse(false), hasTag(false) 
	{
		init();
	}

	void init();

	bool parseFile(const char fileName[]);

	bool removeEmptyTags(); // removes all tags that have no text in them
	int keyValue(std::string key)
	{
		return bm.key(key);
	}

	bool parseString(const char* buff);

	
	bool buildParseTree() 
	{
		if (!validParse) return false;

		bool res = buildParseTree(&root, EmptyTag);
		// XXX TODO close ((std::ifstream*)in)->close() if not a string buff
		delete in;
		in = NULL;
		return res;
	}

	bool getToken(); // string getToken()
	node* getRoot() { return root.children[0];};

	void remove(node *& n);

	node* find(node* start, searchfunction func);
	node* find(node* start, const finder* funcObj);
	node* find(node* start, int tag)
	{
		findOne funcObj(tag);
		return find(start, &funcObj);
	}

	node* findNext(node*n)
	{
		if (singleSearch)
		{
			return find(n, tagValue);
		}
		else
		{
			return find(n, fm);
		}
		// XXX todo - write code for "searchfunction" based search as well
	}

	node* find(const std::string tag)
	{
		tagValue = bm.key(tag);
		if (tagValue < 0) return NULL; // unknown tag
		node* start = getRoot()->leftmost();
		singleSearch = true;
		return find(start, tagValue);
	}

	node* find(const findMany& _fm)
	{
		node* start = getRoot()->leftmost();
		fm = &_fm;
		singleSearch = false;
		return find(start, fm);
	}



	void list(int k, bool prettify, std::ostream& os = std::cout)
	{
		getRoot()->list(k, prettify, bm, os);
	}

	bool isValidParse() { return validParse; }

	biMap<std::string> bm;

private:
	bool buildParseTree(node* root, int ctag);
	std::istream* in;
	std::string text, tag, attribute;
	bool validParse;
	bool hasTag;
	node root;
	int tagValue; // used during search
	const findMany * fm;
	bool singleSearch; // indicated for <findNext> if we are using a single or a multi-search
};
