#ifndef _HASH_H
#define _HASH_H
// These functions have been extracted from http://www.concentric.net/~ttwang/tech/inthash.htm

typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned char uint8_t;


uint32_t ror32slow(uint32_t operand, uint8_t width);
uint32_t ror32(uint32_t a, uint32_t width);

// seems to be the best
//also, empirically hash(x) % N fills the space more uniformly than hash(x) & N. Not sure why. May depend on N, I've tried 99.
uint32_t hash32shift(uint32_t key);

uint32_t hash(uint32_t a);

uint32_t hash32shiftmult(uint32_t key);

uint64_t hash64shift(uint64_t key);

uint32_t hash6432shift(uint64_t key);

uint32_t hashString(const char* c, const int len);
int CRCHash(const char* s);

#endif