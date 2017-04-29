#include <iostream>
#include <vector>
#include "fsa.h"


fsa::fsa(int size) : transitions(size), maxState(0), states(size)
{
	int i, j;
	// initilise transition matrix with 0 everywhere, meaning invalid transition
	for (i = 0; i < size; ++i)
	{
		states[i].funcPtr = NULL;
		states[i].terminalState = false;
		states[i].nbDest = 0;
		transitions[i].resize(N);
		for (j = 0; j <N; ++j)
		{
			transitions[i][j] = 0;
		}
	}
	// build the character map
	buildMap();
} // fsa::fsa(int size)

void fsa::addString(char* str, FUNCPTR funcptr)
{
	int pos = 0;
	int state = 0;
	unsigned char c;
	if (!str[0]) return; // handle empty string
	while (str[pos])
	{
		c = map[str[pos]];
		// If c is invalid we throw. This is needed as str is partially added to the FSA and we cannot roll it back
		if (c > N) throw "Input string contains invalid characers";
		if (transitions[state][c] == 0)
		{	// add new transition state
			++maxState;
			if (maxState == transitions.size()) // increase size if running out of capacity
			{
				transitions.resize(2 * maxState);
				states.resize(2 * maxState);
				int i, j;
				for (i = maxState; i < 2 * maxState; ++i)
				{
					states[i].funcPtr = NULL;
					states[i].terminalState = false;
					states[i].nbDest = 0;
					transitions[i].resize(N);
					for (j = 0; j < N; ++j)
					{
						transitions[i][j] = 0;
					}
				}
			}
			transitions[state][c] = maxState;
			++states[state].nbDest;
			state = maxState;
		}
		else
		{	// follow existing transition for character <c>
			/*if (!states[state].terminalState)
			{
				states[state].funcPtr = NULL; // If we reach this part, the state is used by at least 2 pattterns. remove function pointer as no partial matching is possible
			}*/
			state = transitions[state][c];
			//++states[state].nbOut;
		}
		++pos;
	} // while (str[pos])
	  // Set transition function for last state
	//transitions[state][c] = 0;
	states[state].funcPtr = funcptr;
	states[state].terminalState = true;
} // void fsa::addString(char* str)




void fsa::finaliseBuild()
{
	// use a dynamic programming argument, based on the observations that the ppreceeding state of any state in the FSA is smaller. This is enforced during ceation
	// if a position can only be reached by one input string set it acceptance function to that of the string.
	int state, dest, k;
	int* nbOuts = new int[maxState+1];
	for (state = maxState; state >= 0; --state)
	{
		nbOuts[state] = states[state].nbDest;
		if (states[state].nbDest== 1 && states[state].funcPtr == NULL)
		{
			/// find destination
			for (k = 0; k < N; ++k) if (transitions[state][k] != 0)
			{
				dest = transitions[state][k];
				if (nbOuts[dest]<2)
				{
					states[state].funcPtr = states[dest].funcPtr;
				}
				else nbOuts[state] = nbOuts[dest]; // register that descendent has more than one destination
			}
		}
		// no action is needed if nbDest==0 (as we already have funcPtr) or nbDest>1 (as this state would match several strings and cannot be used for partial matches)
	}
	delete[] nbOuts;
	// A special note on states that have only one Destination, but later one of their successor states has more than 1 destination. These states should not accept partial strings.
	// E.g. construct automata for triplet (aa, aaa, aab). Sates 0 and 1 should not accpet.
}


FUNCPTR fsa::match(char* str)
{
	int pos = 0;
	int state = 0;
	char c;
	while (str[pos])
	{
		c = map[str[pos]];
		if (transitions[state][c] == 0) return NULL; // str is not matched by the FSA

		// follow transition for character <c>
		state = transitions[state][c];
		++pos;
	} // while (str[pos])

	//if we reached the end of the input str the FSA has recognized it if it ended in a terminal state
	if (states[state].terminalState)
	{
		return states[state].funcPtr;
	}
	return NULL;
} // void* fsa::match(char* str)


FUNCPTR fsa::matchPattern(char* str)
{
	int pos = 0;
	int state = 0;
	char c;
	while (str[pos])
	{
		c = map[str[pos]];
		if (transitions[state][c] == 0) return NULL; // str is not matched by the FSA

		// follow transition for character <c>
		state = transitions[state][c];

		++pos;
	} // while (str[pos])

	  //if we reached the end of the str the FSA has recognized it
	if (str[pos] == 0)
	{
		return states[state].funcPtr;
	}
	return NULL;
} // void* fsa::matchPattern(char* str)


void fsa::alternative1()
{
	int i, j;
	transitions2 = new int[maxState*N];
	for (i = 0; i < maxState; ++i)
	{
		for (j = 0; j < N; ++j)
		{
			transitions2[i + j*maxState] = transitions[i][j];
		}
	}
} // void fsa::alternative1()


void fsa::alternative2()
{
	int i, j;
	transitions2 = new int[maxState*N];
	for (i = 0; i < maxState; ++i)
	{
		for (j = 0; j < N; ++j)
		{
			transitions2[i*N + j] = transitions[i][j];
		}
	}
} // void fsa::alternative2()


void fsa::alternative3()
{
	int state, c, i;
	transitions2 = new int[maxState*N];
	
	// aux[k] hold the starting position of state "k" in transition2.
	int* aux = new int[maxState+1];
	int destinations[N]; // list of destination states from current state. Will rarely be full
	int nbDest; // number of destinations
	int pos = 0; // current position in transitions2. transitions2 has a complex structure and represents the FSA in condensed format.
	for (state = 0; state <= maxState; ++state)
	{
		aux[state] = pos;
		++pos; // leave space for nbDest, will be filled later
		nbDest = 0; // counts transitions out of state <i>
		// populate transitions
		for (c = 0; c < N; ++c) if (transitions[state][c] != 0)
		{
			destinations[nbDest] = transitions[state][c];
			++nbDest;
			transitions2[pos] = c;
			pos++;
		}
		// populate destination states
		if (states[state].funcPtr!=NULL) // accepting state
		{
			transitions2[pos-nbDest-1] = -nbDest; // store the number of the terminal state needed to get the acceptance function from fsa.states[]
			transitions2[pos] = (int)states[state].funcPtr;
			++pos;
		}
		else
		{
			transitions2[pos - nbDest - 1] = nbDest;
		}
		for (i = 0; i < nbDest; ++i)
		{
			transitions2[pos] = destinations[i];
			pos++;
		}
	}
	int maxpos = pos;
	for (pos = 0; pos < maxpos; )
	{
		nbDest = transitions2[pos];
		++pos;
		if (nbDest <= 0)
		{
			++pos; 
			nbDest = -nbDest;
		}
		pos += nbDest;
		for (i = 0; i < nbDest; ++i)
		{
			state = transitions2[pos];
			transitions2[pos] = aux[state];
			++pos;
		}
	}
	delete[] aux;
} // void fsa::alternative3()



FUNCPTR fsa::matchPattern1(char* str)
{
	int pos = 0;
	int state = 0;
	char c;
	while (str[pos])
	{
		c = map[str[pos]];
		if (transitions2[state + c*maxState] == 0) return NULL; // str is not matched by the FSA

		// follow transition for character <c>
		state = transitions2[state + c*maxState];

		++pos;
	} // while (str[pos])

	  //if we reached the end of the str the FSA has recognized it
	if (str[pos] == 0)
	{
		return states[state].funcPtr;
	}
	return NULL;
} // void* fsa::matchPattern1(char* str)



FUNCPTR fsa::matchPattern2(char* str)
{
	int pos = 0;
	int state = 0;
	char c;
	while (str[pos])
	{
		c = map[str[pos]];
		if (transitions2[state*N + c] == 0) return NULL; // str is not matched by the FSA

		// follow transition for character <c>
		state = transitions2[state*N + c];
		++pos;
	} // while (str[pos])

	  //if we reached the end of the str the FSA has recognized it
	if (str[pos] == 0)
	{
		return states[state].funcPtr;
	}
	return NULL;
} // void* fsa::matchPattern(char* str)

FUNCPTR fsa::matchPattern3(char* str)
{
	int pos = 0;
	int i, nbDest, fsapos = 0;
	char c;
	char negativeOffset = 0;
	int lo, hi, mid, midval;
	while (str[pos])
	{
		c = map[str[pos]];
		nbDest = transitions2[fsapos];
		if (nbDest <= 0)
		{
			if (str[pos]==0) // end of string reached
				return (FUNCPTR)transitions2[fsapos - nbDest + 1]; /// exact or partial match
			negativeOffset = 1;
			nbDest = -nbDest;
		} else negativeOffset = 0;
/*		if (nbDest < 10)
		{ // linear search is quicker for few elements
			for (i = 1; i <= nbDest; ++i)
			{
				if (transitions2[fsapos + i] == c)
				{
					fsapos = transitions2[fsapos + i + nbDest + negativeOffset];
					++pos;
					goto endWhile;
				}
			}
		}
		else*/
		{ // bin search is quicker for >9 elements
			lo = fsapos + 1; hi = fsapos + nbDest;
			int mid;
			int midval;
			while (lo <= hi)
			{
				mid = (lo + hi) / 2;
				midval = transitions2[mid];
				if (midval < c)
				{
					lo = mid + 1;
				}
				else if (midval > c)
				{
					hi = mid - 1;
				}
				else
				{
					fsapos = transitions2[mid + nbDest + negativeOffset];
					++pos;
					goto endWhile;
				}
			}
		}
		// if loop finishes no transition was found
		return NULL;
		endWhile:;
	}
	// fsapos points to the last node reached in the FSA.
	if (transitions2[fsapos] <= 0)
	{
		nbDest = transitions2[fsapos];
		if (nbDest <= 0)
		{
			return (FUNCPTR)transitions2[fsapos - nbDest + 1]; /// exact or partial match
		}
		else return NULL;
	}
	// Potential partial match, but acceptance function is very expensive back out from data stored.
	return NULL;
} // void* fsa::matchPattern(char* str)



void fsa::buildMap()
{
	int i;
	for (i = 0; i < 48; ++i)
	{
		map[i] = 255; // code for invalid characters
	}
	for (; i < 58; ++i)
	{
		map[i] = i - 48; // codes for numbers
	}
	for (; i < 65; ++i)
	{
		map[i] = 255; // // code for invalid characters
	}
	for (; i < 91; ++i)
	{
		map[i] = i - 65 + 10; // codes for upper case letters
	}
	for (; i < 97; ++i)
	{
		map[i] = 255; // code for invalid characters// code for invalid characters
	}
	for (; i < 123; ++i)
	{
		map[i] = i - 97 + 10; // codes for lower case letters are the same as codes for upper case
	}
	for (; i < 256; ++i)
	{
		map[i] = 255; // code for invalid characters
	}
	map['_'] = 36; // code for "_"
} // void fsa::buildMap()


void fsa::listStates()
{
	std::cout << "MaxState = " << maxState<<std::endl;
	for (int i = 0; i < maxState; ++i)
	{
		std::cout << i << "  " << states[i].terminalState << std::endl;
	}
} // void fsa::listStates()
