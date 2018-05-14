#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include "node.h"

typedef enum { BEGIN, TEXT, TAG, ATTRIBUTE, END } states;


bool isWithoutEnd(std::string& tag);

extern std::string Document;

typedef bool (searchfunction)(node* n);


class finder
{
public:
	finder(const char* _tag) : tag(_tag) {}
	finder(std::string& _tag) : tag(_tag) {}

	bool match(node* n) { return _stricmp(n->tag.c_str(), tag.c_str())==0 ; }

protected:
	std::string tag;
};


template <class KeyType = std::string>
class biMap
{
public:
	biMap() : counter(0) {}
	void clear()
	{
		keyToValue.clear();
		valueToKey.clear()
		counter = 0;
	}

	int insert(KeyType k) // insert a new key
	{
		std::map<KeyType, int>::const_iterator it = keyToValue.find(k);
		if (it != keyToValue.end()) // key is not in the map
		{
			return it->second;
		};
		keyToValue[k] = counter;
		valueToKey[counter] = k;
		return counter++;
	};

	KeyType value(int v) // returns key associated to value
	{
		std::map<int, KeyType>::const_iterator it = valueToKey.find(v);
		if (it != valueToKey.end())
		{
			return it->second;
		}
		throw ("Invalid key.");
	};

	int key(KeyType k) //  return value associated to key
	{
		std::map<KeyType, int>::const_iterator it = keyToValue.find(k);
		if (it != keyToValue.end())
		{
			return it->second;
		}
		throw ("Invalid key.");
	};


protected:
	std::map<KeyType, int> keyToValue;
	std::map<int, KeyType> valueToKey;
	int counter; // number of unique elements
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

	bool parseString(const char* buff);

	bool buildParseTree() 
	{
		bool res = buildParseTree(&root, Empty);
		// XXX TODO close ((std::ifstream*)in)->close() if not a string buff
		delete in;
		in = NULL;
		return res;
	}

	bool getToken(); // string getToken()
	node* getRoot() { return root.children[0];};

	void remove(node *& n);

	node* find(node* start, searchfunction func, int method);
	node* find(node* start, finder* funcObj);
	node* find(node* start, const char* tag)
	{
		finder funcObj(tag);
		return find(start, &funcObj);
	}
	node* find(const char* tag)
	{
		node* start = getRoot()->leftmost();
		return find(start, tag);
	}

	void list(int k, bool prettify, std::ostream& os = std::cout)
	{
		getRoot()->list(k, prettify, os);
	}

	bool isValidParse() { return validParse; }

private:
	bool buildParseTree(node* root, std::string ctag);
	std::istream* in;
	std::string text, tag, attribute;
	bool validParse;
	bool hasTag;
	node root;
	class biMap<std::string> bm;
};
