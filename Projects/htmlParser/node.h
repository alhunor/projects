#pragma once

#include <algorithm>
#include <iostream>
#include <string>

extern std::string Empty;
extern int EmptyTag;  // = bm.value(Empty)


// vectorized string upercase transformation  https://stackoverflow.com/questions/735204/convert-a-string-in-c-to-upper-case


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

	int insert(KeyType str) // insert a new key
	{
		std::transform(str.begin(), str.end(), str.begin(), ::toupper);
		std::map<KeyType, int>::const_iterator it = keyToValue.find(str);
		if (it != keyToValue.end()) // key is not in the map
		{
			return it->second;
		};
		keyToValue[str] = counter;
		valueToKey[counter] = str;
		return counter++;
	};



	KeyType value(int v) const// returns key associated to value
	{
		std::map<int, KeyType>::const_iterator it = valueToKey.find(v);
		if (it != valueToKey.end())
		{
			return it->second;
		}
		throw ("Invalid key.");
	};

	int key(KeyType str) const //  return value associated to key
	{
		std::transform(str.begin(), str.end(), str.begin(), ::toupper);
		std::map<KeyType, int>::const_iterator it = keyToValue.find(str);
		if (it != keyToValue.end())
		{
			return it->second;
		}
		return -1; // returns -1 if not inside
	};


protected:
	std::map<KeyType, int> keyToValue;
	std::map<int, KeyType> valueToKey;
	int counter; // number of unique elements
};


class attribute
{
public:
	attribute(std::string& _raw) : hasmap(false), raw(_raw) {}
	const std::string& operator[] (const char* key);
	bool operator!=(const std::string& s) const
	{
		return s != raw;
	}

protected:
	bool hasmap;
	std::string raw;
	std::map<std::string, std::string> attr;
	friend std::ostream& operator<<(std::ostream& io, const attribute& a);
};



struct node
{
private:
	node(std::string& _text, int _tag, std::string& _attribute) : text(_text), attribute(_attribute), parent(NULL), nr(0), tag(_tag) {}

public:
	node(std::string& _text) : node(_text, -1, Empty) {  }
	node(int _tag, std::string& _attribute) : node(Empty, _tag, _attribute) { }
	std::string text;
	int tag; // uses lookup via biMap structure; 
	attribute attribute;
	int nbChildren() const { 	return (int) children.size(); }
	void attachChild(node* m); // removes node <m> from its parent and reattaches it as n's last child


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

	bool list(int k, bool prettify, biMap<std::string>& bm, std::ostream& os = std::cout);

	node* parent;

private:
	std::vector<node*> children;
	friend class parser;
	size_t nr; // indicates position of this node in its parent's children vector. Needed to compute successor.
};

bool isWithoutEnd(int tag);


// bool inset(char c, const char* mask)
// string strip(const string& s, const char* premask = " \n\t\r", const char* postmask = " \n\t\r");
std::string nspaces(int level, int prettify);

