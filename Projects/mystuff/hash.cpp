#include <stdlib.h>
#include <iostream>
#include "hash.h"


using namespace std;

unsigned int hashString(const char* key, const int len)
{// computes hash value for <s>. len = strlen(s) must be provided externally.
    unsigned int hashVal = 0;
 
	for (int x = 0; x < len; ++x)
    {
        hashVal ^= (hashVal << 5) + (hashVal >> 2) + key[x];
    }
 
    return hashVal;
}


// These functions have been extracted from http://www.concentric.net/~ttwang/tech/inthash.htm

uint32_t hash32shift(uint32_t key)
{
  key = ~key + (key << 15); // key = (key << 15) - key - 1;
  key = key ^ (key >> 12);
  key = key + (key << 2);
  key = key ^ (key >> 4);
  key = key * 2057; // key = (key + (key << 3)) + (key << 11);
  key = key ^ (key >> 16);
  return key;
}

uint32_t hash(uint32_t a)
{
   a = (a+0x7ed55d16) + (a<<12);
   a = (a^0xc761c23c) ^ (a>>19);
   a = (a+0x165667b1) + (a<<5);
   a = (a+0xd3a2646c) ^ (a<<9);
   a = (a+0xfd7046c5) + (a<<3);
   a = (a^0xb55a4f09) ^ (a>>16);
   return a;
}


uint32_t hash32shiftmult(uint32_t key)
{
  uint32_t c2=0x27d4eb2d; // a prime or an odd constant
  key = (key ^ 61) ^ (key >> 16);
  key = key + (key << 3);
  key = key ^ (key >> 4);
  key = key * c2;
  key = key ^ (key >> 15);
  return key;
}


uint64_t hash64shift(uint64_t key)
{
  key = (~key) + (key << 21); // key = (key << 21) - key - 1;
  key = key ^ (key >> 24);
  key = (key + (key << 3)) + (key << 8); // key * 265
  key = key ^ (key >> 14);
  key = (key + (key << 2)) + (key << 4); // key * 21
  key = key ^ (key >> 28);
  key = key + (key << 31);
  return key;
}


uint32_t hash6432shift(uint64_t key)
{
  key = (~key) + (key << 18); // key = (key << 18) - key - 1;
  key = key ^ (key >> 31);
  key = key * 21; // key = (key + (key << 2)) + (key << 4);
  key = key ^ (key >> 11);
  key = key + (key << 6);
  key = key ^ (key >> 22);
  return (uint32_t) key;
}


uint32_t ror32slow(uint32_t operand, uint8_t width)
{ 
	_asm
	{
		mov eax, operand
		mov cl, width
		ror eax, cl
		mov operand, eax
	}
	return operand; 
} 

__inline __declspec(naked) uint32_t ror32(uint32_t a, uint32_t width)
{
	__asm
	{
		push ebp
		mov ebp, esp
		sub esp, __LOCAL_SIZE

		mov eax, DWORD PTR 8[ebp]
		mov ecx, DWORD PTR 8[ebp+4]
		ror eax, cl

		mov esp, ebp
		pop ebp
		ret
	}
}


void addHash(int &h, int ki)
{ // Do a 5-bit left circular shift of h. Then XOR in ki. Specifically
	int highorder = h & 0xf8000000;// extract high-order 5 bits from h 
	h = h << 5; // shift h left by 5 bits
	h = h ^ (highorder >> 27); // move the highorder 5 bits to the low-order end and XOR into h 
	h = h ^ ki; // XOR h and ki 
}

int CRCHash(const char* s)
{
	//https://www.cs.hmc.edu/~geoff/classes/hmc.cs070.200101/homework10/hashfuncs.html 
	// break <s> in 32 bit words
	int ki = 0;
	int pos = 0;
	int hash = 0;
	int highorder = 0;
	char spos = s[0];
	do
	{
		ki = ki << 8;
		if (spos != 0)
		{
			ki += spos;
			pos++;
			spos = s[pos];
		};
		if (pos % 4 == 0)
		{ // we've got 4 bytes already 
			addHash(hash, ki);
			ki = 0;
		}
	} while (spos != 0);
	// last block has less than 4 characters, add them to <h>
	if (pos % 4 != 0) addHash(hash, ki);
	return hash;
} //int CRCHash(const char* s)