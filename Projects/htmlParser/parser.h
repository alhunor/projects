#pragma once

#include <fstream>
#include <sstream>
#include <string>
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






class parser
{
public:
	parser() : root(Document), validParse(false), hasTag(false) { 	}

	bool parseFile(const char fileName[])
	{	
		std::ifstream* ifs = new std::ifstream();
		ifs->open(fileName);
		in = ifs;
		// XXX TODO check that file exists and return error message if not
		return validParse = true;
	}

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

private:
	bool buildParseTree(node* root, std::string ctag);
	std::istream* in;
	std::string text, tag, attribute;
	bool validParse;
	bool hasTag;
	node root;
};