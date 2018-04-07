#pragma once

#include <fstream>
#include <string>

typedef enum { BEGIN, TEXT, TAG, ATTRIBUTE, END } states;


extern std::string Empty;


struct node
{
private:
	node(std::string& _text, std::string& _tag, std::string& _attribute) : text(_text), tag(_tag), attribute(_attribute), parent(NULL), nr(0) {}
public:
	node(std::string& _text) : node(_text, Empty, Empty) {  }
	node(std::string& _tag, std::string& _attribute) : node(Empty, _tag, _attribute) { }
	std::string tag, attribute, text;
	int nbChildren() const { 	return (int) children.size(); }
	node * child(int k)
	{
		if (k >= nbChildren() ) return NULL;
		return children[k];
	}

	node * next()
	{
		if (parent == NULL) return NULL;
		if (nr < parent->nbChildren()-1)
			return parent->children[nr + 1];
		return NULL;
	}

	void erase();

	node* leftmost()
	{
		node* n = this;
		while (n->children.size() != 0)
		{
			n = n->children[0];
		}
		return n;
	}



	// return the sucessor of the node in <subtree>
	// if <subtree> is not specified, search spans the whole tree
	// uses postorder traversal
	node* succ(node* subtree = NULL)
	{
		// sub-tree is empty, go to parents
		node * aux = parent;
		if (aux == NULL) return NULL;
		if (this == subtree) return NULL; // 
		if (aux->children.size()>nr + 1)
		{
			return aux->children[nr + 1]->leftmost();
			// return leftmost child from aux->children[n->nr + 1] subtree
		}
		else
		{
			return aux;
		}
	} // node* next(node* n, int method)


	node* parent;

private:
	std::vector<node*> children;
	friend class parser;
	size_t nr; // indicates position of this node in its parent's children vector. Needed to compute successor.
};

bool isWithoutEnd(std::string& tag);






// bool inset(char c, const char* mask)
// string strip(const string& s, const char* premask = " \n\t\r", const char* postmask = " \n\t\r");
std::string nspaces(int level, int prettify);
bool list(node* n, int k, bool prettify);






extern std::string Document;

typedef bool (searchfunction)(node* n);


class finder
{
public:
	finder(const char* _tag) : tag(_tag) {}
	finder(std::string& _tag) : tag(_tag) {}

	bool match(node* n) { return n->tag == tag; }

protected:
	std::string tag;
};




class parser
{
public:
	parser() : root(Document), validParse(false), hasTag(false) { 	}

	bool open(const char fileName[]);
	bool buildParseTree() { return buildParseTree(&root, Empty); }
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

private:
	bool buildParseTree(node* root, std::string ctag);
	std::ifstream in;
	std::string text, tag, attribute;
	bool validParse;
	bool hasTag;
	node root;
};