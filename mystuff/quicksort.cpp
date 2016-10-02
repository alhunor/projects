#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "quicksort.h"

using namespace std;

 
template<typename T>
int binary_search(T sorted_list[], int lo, int hi, T element)
{
	while (lo <= hi)
	{
		int middle = lo + (hi - lo)/2;
		if (element > sorted_list[middle])
			lo = middle + 1;
		else if (element < sorted_list[middle])
			hi = middle - 1;
		else
			return middle;
	}
	return -1;
}

template<typename T>
struct Point{T x,y;};

template<typename T>
inline bool smallerx(Point<T> a, Point<T> b) { return a.x < b.x; }

template<typename T>
inline bool smallery(Point<T> a, Point<T> b) { return a.y < b.y; }

int add(int a, int b) {return a+b;}
int sub(int a, int b) {return a-b;}

template<typename T, T (*Y)(T,T)>
inline T operate(T a, T b)
{
	return Y(a, b);
}

int main()
{
	Point<double> p;
	
	cout<<operate<int, sub>(2,5)<<endl;
	
//	cout<<sizeof(p)<<endl;

	#define ARRAY_SIZE 250000
	int list[ARRAY_SIZE];

	int i;

	for (i=0; i < ARRAY_SIZE; ++i)
		list[i] = rand();

//	heap_sort(list, cmpfun, ARRAY_SIZE-1);
	quicksort(&list[0], 0, ARRAY_SIZE-1);
//	selection_sort(list, 0, ARRAY_SIZE-1);
//	bubble_sort(list, 0, ARRAY_SIZE-1);

	for (i=1; i < ARRAY_SIZE; ++i)
	{
		if (list[i - 1] > list[i])
		{
			printf("bad sort\n");
			return(1);
		}
	}
	return(0);
}
