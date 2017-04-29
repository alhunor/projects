#include <iostream>
//#include <boost/numeric/ublas/matrix.hpp>
#include "matrix.h"

using namespace std;
using namespace boost::numeric;
 
inline int change(char a, char b)
{
	return a==b?0:1;
}
 
inline int min3(int a, int b, int c)
{
	if (a<b) return a<c?a:c;
	return b<c?b:c;
}
 
inline int min2(int a, int b)
{
	return a<b?a:b;
}


int editDistance(char* x, char* y)
{
// See : http://en.wikipedia.org/wiki/Damerau%E2%80%93Levenshtein_distance
// The code allows for insertion, deletion, modification and transposition of adjacent characters.
	int nx=strlen(x);
	int my=strlen(y);
	// The M(i,j) matrix position contains the cost of transforming x[0..i-1] into y[0..j-1]
	HuMatrix<int> M(nx+1, my+1);
	int i,j;
 
	// the only way to transfor the empty string into y0..y[j-1] is to add "j" characters y0..y[j-1]
	for (j=0; j<=my; j++) M(0,j)=j*10;
 
	// similar reasons
	for (i=0; i<=nx; i++) M(i,0)=i*10;
 
	for (i=1; i<=nx; i++)
	{
		for (j=1; j<=my; j++)
		{
				M(i,j) = min3( M(i-1,j)+10, // Delete the final character of x and try to match x[0..i-2] with y[0..j-1]
						M(i, j-1)+10, // Insert y[j-1] at the end of x[0..i-1] and try to match x[0..i-1] with y[0..j-2]
						M(i-1,j-1)+10*change(x[i-1], y[j-1]) // Change x[n-1] into y[j-1] then match x[0..i-2] with y[0..j-2]
						);
 				if ((i > 1) && (j > 1) && (x[i-1] == y[j-2]) && (x[i-2] == y[j-1]))
							M(i, j) = min2( M(i, j), M(i-2, j-2) + 5); // cost of transposition is 5
 
 
		} // for (j=1; j<=my; j++)
	} // for (i=1; i<=nx; i++)
	return M(nx,my);
}

ublas::matrix<int> substitutionCosts(256, 256, 10);
int deleteCost[256];
int insertCost[256];
static bool editCostsInitialised = false;

void initialiseCost()
{
	unsigned int i,j, k, len;
	// set up low cost for substituting upper case with upper case and vice versa
	j='a';
	for (i='A',j='a';i<='Z'; ++i, ++j)
	{
		substitutionCosts(i,j) = substitutionCosts(j,i)=1;
	};
	for (i=0; i<=255; ++i)
	{
		substitutionCosts(i,i) = 0;
	}
	//equivalence classes
	char *eq[8] = {"aAâäàåÄÅá", "eEéÉêëè", "oOöÖóôò", "uUüÜúûùµ", "iIíîìï", "cCçÇ", "nNñÑ", "yYÿ"};
	unsigned char a, b;
	for (i=0; i<8; ++i)
	{
		len = strlen(eq[i]);
		for (j=0; j<len; ++j)
		{
			for (k=j+1; k<len; ++k)
			{
				a = (unsigned char)eq[i][j];
				b = (unsigned char)eq[i][k];
				substitutionCosts(a,b)=substitutionCosts(b,a)=1; // must be the same as upercase to lowercase cost
			}
		}
	} // for (i=0; i<8; ++i)
// Costs of Delete & Insert operations
	for (i=0; i<=255; ++i)
	{
		deleteCost[i]=10;
		insertCost[i]=10;
	}
	char* whiteSpaces = " .,()[]{}?!-<>'%$/|\\";
	len = strlen(whiteSpaces);
	for (i=0; i<len; ++i)
	{
		a = (unsigned char)whiteSpaces[i];
		deleteCost[a]=1;
		insertCost[a]=1;
	}

	editCostsInitialised = true;
} // void initialiseCost()


int editDistanceWithCost(char* x, char* y)
//HuMatrix<int> editDistanceWithCost(char* x, char* y)
{
// See: http://en.wikipedia.org/wiki/Damerau%E2%80%93Levenshtein_distance
// The code allows for insertion, deletion, modification and transposition of adjacent characters.

	if (! editCostsInitialised)
	{
		initialiseCost();
	}

	int nx=strlen(x);
	int my=strlen(y);

	// The M(i,j) matrix position contains the cost of transforming x[0..i-1] into y[0..j-1]
	//	ublas::matrix<int> M(nx+1, my+1);
	HuMatrix<int> M(nx+1, my+1);
	
	int i,j;
 
	// the only way to transfor the empty string into y0..y[j-1] is to add "j" characters y0..y[j-1]
	M(0,0) = 0;
	for (j=1; j<=my; j++) M(0,j) = M(0,j-1) + insertCost[y[j-1]];
 
	// similar reasons
	for (i=1; i<=nx; i++) M(i,0) = M(i-1,0) + deleteCost[x[i-1]];
 
	unsigned char deleteX, deleteY, substituteXY;
	unsigned char xi, yj;
	for (i=1; i<=nx; i++)
	{
		for (j=1; j<=my; j++)
		{
			xi = (unsigned char)x[i-1];
			yj = (unsigned char)y[j-1];

			deleteX = M(i-1,j)+deleteCost[xi]; // Delete the final character of x and try to match x[0..i-2] with y[0..j-1]
			deleteY = M(i, j-1)+insertCost[yj];// Insert y[j-1] at the end of x[0..i-1] and try to match x[0..i-1] with y[0..j-2] TODO XXX check the wordin, I suspect it should be : Delete y[j-1] and try to match x[0..i-1] with y[0..j-2]
			substituteXY = M(i-1,j-1)+substitutionCosts(xi, yj); // substitute x[n-1] with y[j-1] then match x[0..i-2] with y[0..j-2]
			M(i,j) = min3(deleteX, deleteY, substituteXY);
 
 			if ((i > 1) && (j > 1) && (xi == y[j-2]) && (x[i-2] == yj))
			{
				M(i, j) = min2( M(i, j), M(i-2, j-2) + 10); // transposition
			}
		} // for (j=1; j<=my; j++)
	} // for (i=1; i<=nx; i++)
	return M(nx,my);
//	return M;
} // int editDistanceWithCost(char* x, char* y)


//int editDistanceWithOverlapDetection(char* x, char* y)
int editDistanceWithOverlapDetection(char* x, char* y)
{
// See: http://en.wikipedia.org/wiki/Damerau%E2%80%93Levenshtein_distance
// The code allows for insertion, deletion, modification and transposition of adjacent characters.

	if (! editCostsInitialised)
	{
		initialiseCost();
	}

	int nx=strlen(x);
	int my=strlen(y);

	// The M(i,j) matrix position contains the cost of transforming x[0..i-1] into y[0..j-1]
	HuMatrix<int> M(nx+1, my+1);
	int i,j;
 
	// the only way to transfor the empty string into y0..y[j-1] is to add "j" characters y0..y[j-1]
	for (j=0; j<=my; j++) M(0,j) = 0;
 
	// similar reasons
	for (i=0; i<=nx; i++) M(i,0) = 0;
 
	unsigned char deleteX, deleteY, substituteXY;
	unsigned char xi, yj;
	for (i=1; i<=nx; i++)
	{
		for (j=1; j<=my; j++)
		{
			xi = (unsigned char)x[i-1];
			yj = (unsigned char)y[j-1];

			deleteX = M(i-1,j)+deleteCost[xi]; // Delete the final character of x and try to match x[0..i-2] with y[0..j-1]
			deleteY = M(i, j-1)+insertCost[yj];// Insert y[j-1] at the end of x[0..i-1] and try to match x[0..i-1] with y[0..j-2] TODO XXX check the wordin, I suspect it should be : Delete y[j-1] and try to match x[0..i-1] with y[0..j-2]
			substituteXY = M(i-1,j-1)+substitutionCosts(xi, yj); // substitute x[n-1] with y[j-1] then match x[0..i-2] with y[0..j-2]
			M(i,j) = min3(deleteX, deleteY, substituteXY);
 
 			if ((i > 1) && (j > 1) && (xi == y[j-2]) && (x[i-2] == yj))
			{
				M(i, j) = min2( M(i, j), M(i-2, j-2) + 10); // transposition
			}
		} // for (j=1; j<=my; j++)
	} // for (i=1; i<=nx; i++)
	
	
	int d = 0, cost;
	for (i=1; i<=nx; i++)
	{
		for (j=1; j<=my; j++)
		{
			cost = min(i,j)*(10-M(i,j));
			if (cost>d) d = cost;
		}
	}
	M(0, 0) = d;
//	return M;
	return d;
} // int editDistanceWithOverlapDetection(char* x, char* y)

void testEditDistance()
{
	char *x ="Bamba jancsi";
	char *y ="aBmba janosika";
	x = "alma";
	y = " Alma";
	int d;
	d = editDistance(x, y);
	d = editDistanceWithCost(x, y);
}












char mapCaseTable[256];

void init_match(void)
{
        for (int i = 0; i < 256; ++i)
        {
                mapCaseTable[i] = toupper(i);
        }
}

bool wildCharMatch(const char* pat, const char* str)
// http://xoomer.virgilio.it/acantato/dev/wildcard/wildmatch.html#c_cpp_userjournal_algo
{
        int i, star;

new_segment:

        star = 0;
        if (*pat == '*')
        {
                star = 1;
                do
                {
                        pat++;
                } while (*pat == '*');
        }

test_match:

        for (i = 0; pat[i] && (pat[i] != '*'); i++)
        {
                if (mapCaseTable[str[i]] != mapCaseTable[pat[i]])
                {
                        if (!str[i]) return 0;
                        if ((pat[i] == '?') && (str[i] != '.')) continue;
                        if (!star) return 0;
                        str++;
                        goto test_match;
                }
        }
        if (pat[i] == '*')
        {
                str += i;
                pat += i;
                goto new_segment;
        }
        if (!str[i]) return 1;
        if (i && pat[i - 1] == '*') return 1;
        if (!star) return 0;
        str++;
        goto test_match;
} // bool wildCharMatch(char* pat, char* str)

