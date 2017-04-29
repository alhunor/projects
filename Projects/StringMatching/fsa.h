#ifndef _FSA_H
#define _FSA_H

#include <vector>

typedef void(*FUNCPTR)();

struct stateT
{
	bool terminalState;
	int nbDest; // number of destination states after current state
	FUNCPTR funcPtr; // ptr to function to call if pattern is matched
};

#define N 37 // size of the alphabet

class fsa
{
public:
	fsa() = delete;
	fsa(int size);

	void buildMap();
	// build the translation table between ASCII characters and the symbols of the alphabet matched by the FSA, currently : 0..9, a..z, A..Z, '_'. 
	// No differencebetween upppercase and lower case

	void addString(char* str, FUNCPTR);
	// builds the FSA by adding a string to it
	
	void finaliseBuild();
	// after all addString instructions have been executed <finaliseBuild> will set up to FSA for approximate matches.

	FUNCPTR match(char* str);
	// matches a string to those stored in the FSA. Return a pointer to the function associated with the string

	FUNCPTR matchPattern(char* str);
	// matches a pattern used for building the automata. 
	// This function is faster than match, but if the argument is not a pattern used for building the FSA the output of the function may be inaccurate.
	// <buildPatternMatchStates()> needs to be called once before the first call to <matchPattern>

	void listStates();
	// lists the internal states of the automata. used for debugging and tuning.

	void alternative1(); // replace vector with 1 pointer to contiguous array, row major
	void alternative2(); // replace vector with 1 pointer to contiguous array, column major
	void alternative3(); // adjacency list
	FUNCPTR matchPattern1(char* str); // replace vector with 1 pointer to contiguous array, row major
	FUNCPTR matchPattern2(char* str); // replace vector with 1 pointer to contiguous array, column major
	FUNCPTR matchPattern3(char* str); // adjacency list

protected:
	unsigned char map[256];
	int maxState;
	int* transitions2;
	std::vector<std::vector<int> > transitions;
	std::vector<stateT> states; // contains the function pointers for evry state. If the state does not accept the string the pointer would be null.
}; // class fsa

#endif