
#include <iostream>
#include <boost/numeric/ublas/matrix.hpp>
 
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
        ublas::matrix<int> M(nx+1, my+1);
        int i,j;
 
        // the only way to transfor the empty string into y0..y[j-1] is to add "j" characters y0..y[j-1]
        for (j=0; j<=my; j++) M(0,j)=j;
 
        // similar reasons
        for (i=0; i<=nx; i++) M(i,0)=i;
 
        for (i=1; i<=nx; i++)
        {
                for (j=1; j<=my; j++)
                {
                        M(i,j) = min3( M(i-1,j)+1, // Delete the final character of x and try to match x[0..i-2] with y[0..j-1]
                                                M(i, j-1)+1, // Insert y[j-1] at the end of x[0..i-1] and try to match x[0..i-1] with y[0..j-2]
                                                M(i-1,j-1)+change(x[i-1], y[j-1]) // Change x[n-1] into y[j-1] then match x[0..i-2] with y[0..j-2]
                                                );
 
                        if ((i > 1) && (j > 1) && (x[i-1] == y[j-2]) && (x[i-2] == y[j-1]))
                                   M(i, j) = min2( M(i, j), M(i-2, j-2) + 1); // transposition
 
 
                } // for (j=1; j<=my; j++)
        } // for (i=1; i<=nx; i++)
        return M(nx,my);
}

void test()
{
	char *x ="Bamba jancsi";
	char *y ="aBmba janosi";
	editDistance(x, y);
}