#pragma once

#include <fstream>
#include <string>

typedef enum { BEGIN, TEXT, TAG, ATTRIBUTE, END } states;


extern std::string Empty;


struct node
{
private:
	node(std::string& _text, std::string& _tag, std::string& _attribute) : text(_text), tag(_tag), attribute(_attribute) {}
public:
	node(std::string& _text) : node(_text, Empty, Empty) {  }
	node(std::string& _tag, std::string& _attribute) : node(Empty, _tag, _attribute) { }
	std::string tag, attribute, text;
	std::vector<node*> children;
};

extern std::string Document;

class parser
{
public:
	parser() : root(Document), validParse(false) { 	}

	bool open(const char fileName[]);
	bool buildParseTree() { return buildParseTree(&root, Empty); }
	bool getToken(); // string getToken()

private:
	bool buildParseTree(node* root, std::string ctag);
	std::ifstream in;
	std::string text, tag, attribute;
	bool validParse;
	node root;
};