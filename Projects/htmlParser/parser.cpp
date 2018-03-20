#include <iostream>
#include <set>
#include <string>
#include <vector>
#include "parser.h"

using namespace std;

std::string Empty = "";
std::string Document = "Document";


bool parser::open(const char fileName[])
{
	in.open(fileName);
	// TODO check that file exists and return error message if not

	return validParse = true;
} // bool parser::open(const char* fileName)

bool parser::getToken()
{
	_ASSERT(validParse);

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
			return true;
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


set<string> tagsWithoutEnd = {"BR", "HR"};
set<string> tagsWithOptionalEnd = {"P"};


bool parser::buildParseTree(node * n, string ctag)
{
	_ASSERT(validParse);

	node* child;

	int pos = 0;
	while (!in.eof())
	{
		getToken();
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
			else return false;
		}

		if (tag != "")
		{
			child = new node(tag, attribute);
			n->children.push_back(child);
			buildParseTree(child, tag);
			//++pos;
		} else if (! isWhiteChars(text))
		{
			child = new node(text);
			n->children.push_back(child);
			++pos;
		}
	}
	return true;
}

