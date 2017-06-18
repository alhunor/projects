#pragma once


class bitVector
{
public:
	bitVector() {}
	~bitVector() {} // do nothing as ptr is given from outside
	void init(int _maxPos, char* _ptr) { maxPos = _maxPos; ptr = _ptr; }
	void setbit(int i);
	char getbit(int i);
protected:
	char* ptr;
	int maxPos;
};