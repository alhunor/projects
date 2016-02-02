#pragma once
//#include <cstdlib>

class tokenizer
{
public:
	tokenizer() : tokenized(false), allocated(false) {}
	tokenizer(const char* s, int len = 0);
	~tokenizer() { clear(); };
	int nbTokens() { return nbtok; }
	const char* token(int n);
	bool init(const char* s, const int len);
	char* slice(char separator);
	void tokenize();

protected:
	void clear();
	bool tokenized;
	int allocated; // number ob bytes allocated for "data", including trailing zero
	const char* tokens[256];
	char* data, *data2;
	int nbtok;
};