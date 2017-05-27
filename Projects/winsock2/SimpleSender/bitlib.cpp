#include "bitlib.h"
#include <intrin.h> // __lzcnt VStudio specific extension


void bitVector::setbit(int pos)
{
	int block = pos >> 3;
	int blockbit = pos & 7;
	ptr[block] |= 1 << blockbit;
}


char bitVector::getbit(int pos)
{
	int block = pos >> 3;
	int blockbit = pos & 7;
	return (ptr[block] & (1 << blockbit)) ? 1 :0;
}


int hibit(__int32 n)
{
	n |= (n >> 1);
	n |= (n >> 2);
	n |= (n >> 4);
	n |= (n >> 8);
	n |= (n >> 16);
	return n - (n >> 1);
}

int hibitPosition(__int32 n)
{
	int ct = 0;
	while (n > 1)
	{
		ct++;
		n = n >> 1;
	}
	return ct;
}

int leadingZeroes(__int32 n)
{
	return __lzcnt(n);
}

int trailingZeroes(__int32 n)
{
	return 0;
}

