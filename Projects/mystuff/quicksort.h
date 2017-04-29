#ifndef _QUICKSORT_H
#define _QUICKSORT_H


/////////////////////////////////////////////
//           QuickSort
////////////////////////////////////////////
/*
http://cprogramminglanguage.net/quicksort-algorithm-c-source-code.aspx Quicksort sorts a list based on the divide and conquer strategy. In quicksort algorithm we divide the list into two sub-lists, sort these sub-lists and recursively until the list is sorted; The basic steps of quicksort algorithm are as follows:
	1) Choose a key element in the list which is called a pivot.
	2) Reorder the list with the rule that all elements which are less than the pivot come before the pivot and so that all elements greater than the pivot come after it. After the partitioning, the pivot is in its final position.
	3) Recursively reorder two sub-lists: the sub-list of lesser elements and the sub-list of greater elements.*/

template<typename T> 
inline void swap(T *x, T *y)
{
	T temp;
	temp = *x;
	*x = *y;
	*y = temp;
}

template<typename T, int (*compare)(T a,T b)>
class efficientQuickSort
{ // this class has been designed to use less memory, cannot be multithreaded since pivot is a class variable
public:
	efficientQuickSort(T* _list) {list=_list;}
	void sort(int lo, int hi);

protected:
	T key;
	int pivot;
	T* list;
};

template<typename T, int (*compare)(T a,T b)>
void efficientQuickSort<T, compare>::sort(int lo, int hi)
{
	int i,j;
again:
	if (lo<hi)
	{
		pivot = (lo+hi)/2;
		swap(&list[lo], &list[pivot]);
		key=list[lo];
		i=lo+1;
		j=hi;
		while(i<=j)
		{
			while((i<=hi) && (compare(list[i],key)<1)) i++; // list[i]<=key
			while((j>=lo) && (compare(list[j],key)==1)) j--; // list[i]>key
			if(i<j)
				swap(&list[i], &list[j]);
		}
		swap(&list[lo], &list[j]);
		if (j-lo < hi-j)
		{
			
			sort(lo, j-1);
			// avoid recursion
			lo=j+1; goto again; // sort(j+1, hi);
		} else
		{
			sort(j+1, hi);
			//sort(lo, j-1);
			// avoid recursion
			hi=j-1; goto again; // sort(lo, j-1);
		}
	}
}



template<typename T> // used internally by quick_select and quicksort
int defaultcompare(T a,T b) {return a<b?-1:(a>b?1:0);}

template<typename T, int (*compare)(T a,T b)=defaultcompare<T> > 
void quicksort(T* list, int lo, int hi)
// the function compare should return 1, if a>b, and 0 or less otherwise
{
	T key;
	int i,j,pivot;
again:
	if (hi-lo<50)
		return;
	if (lo<hi)
	{
		//pivot = (lo+hi)/2;
		pivot = lo + (hi - lo) / 2; // to avoid overflow
		swap(&list[lo], &list[pivot]);
		key=list[lo];
		i=lo+1;
		j=hi;
		while(i<=j)
		{
			while((i<=hi) && (compare(list[i],key)<1)) i++; // list[i]<=key
			while((j>=lo) && (compare(list[j],key)==1)) j--; // list[i]>key
			if(i<j)
				swap(&list[i], &list[j]);
		}
		swap(&list[lo], &list[j]);
		if (j-lo < hi-j)
		{
			quicksort<T,compare>(list, lo, j-1);
			lo=j+1; goto again; // quicksort(j+1, hi);
		} else
		{
			quicksort<T,compare>(list, j+1, hi);
			hi=j-1; goto again; // quicksort(lo, j-1);
		}
	}
}



//template<typename T, int (*compare)(T a, T b)=defaultcompare>
template<typename T, int (*compare)(T a, T b)>
int partition(T* data, int lo, int hi)
{ // used internally by quick_select
    T pivot = data[hi];
    
    while ( lo < hi )
    {
        while ( compare(data[lo], pivot)==-1 ) lo++; // data[lo] < pivot 
        
        while ( compare(data[hi],pivot)==1 ) hi--; // data[hi] > pivot
        
        if ( compare (data[lo], data[hi])==0 ) lo++; // data[lo] == data[hi]
	        else if ( lo < hi ) swap(&data[lo], &data[hi]);
    }  
    return hi;
}

//template<typename T, int (*compare)(T a, T b)=defaultcompare>
template<typename T, int (*compare)(T a, T b)>
int quick_select(typename T* data, int lo, int hi, int k)
// returns the position of the <k>th order element from data[lo..hi]. It will change data
// smallest element is obtained with k==0, largest with k=hi-lo+1;
{
    if ( lo >= hi ) return lo;
    int j = partition<T, compare>(data, lo, hi);
    int length = j - lo + 1;
    if ( length == k ) return j;
    else if ( k < length ) return quick_select<T, compare>(data, lo, j - 1, k);
    else  return quick_select<T, compare>(data, j + 1, hi, k - length);
}



/////////////////////////////////////////////
//           BubbleSort
////////////////////////////////////////////
/*Bubble sorting is a simple sorting technique in sorting algorithm. In bubble sorting algorithm, we arrange the elements of the list by forming pairs of adjacent elements. It means we repeatedly step through the list which we want to sort, compare two items at a time and swap them if they are not in the right order. Another way to visualize the bubble sort algorithm is as its name, the smaller element bubble to the top. Here is the source code implements bubble sorting algorithm in C which sorts an unordered list of integer.*/

template<typename T>
void bubble_sort(T list[], int lo, int hi)
{
   int i,j;
   for(i=0; i<hi; i++)
      for(j=0; j<hi-i; j++)
             if(list[j] > list[j+1])
                    swap(&list[j], &list[j+1]);
}

/////////////////////////////////////////////
//           SelectionSort
////////////////////////////////////////////
/*Selection sort is a simplicity sorting algorithm. It works as its name as it is. Here are basic steps of selection sort algorithm:
	1) Find the minimum element in the list
	2) Swap it with the element in the first position of the list
	3) Repeat the steps above for all remainder elements of the list starting at the second position.*/

template<typename T> 
void selection_sort(T list[], int lo, int hi)
{
  int i, j, min;
 
  for (i = lo; i < hi; i++)
  {
    min = i;
    for (j = i+1; j <= hi; j++)
    {
       if (list[j] < list[min])
          min = j;
    }
    swap(&list[i], &list[min]);
  }
}

/////////////////////////////////////////////
//           HeapSort
////////////////////////////////////////////
/*http://cprogramminglanguage.net/heapsort-c-source-code.aspx Heapsort algorithm is a comparison-based sorting algorithm. The heap sort works as its name suggests. It begins by building a heap out of the data set, and then removing the largest item and placing it at the end of the sorted array. After removing the largest item, it reconstructs the heap, removes the largest remaining item, and places it in the next open position from the end of the sorted array. This is repeated until there are no items left in the heap and the sorted array is full. Elementary implementations require two arrays - one to hold the heap and the other to hold the sorted elements.

Heapsort inserts the input list elements into a heap data structure. The largest value (in a max-heap) or the smallest value (in a min-heap) are extracted until none remain, the values having been extracted in sorted order. The heap's invariant is preserved after each extraction, so the only cost is that of extraction.

During extraction, the only space required is that needed to store the heap. In order to achieve constant space overhead, the heap is stored in the part of the input array that has not yet been sorted. (The structure of this heap is described at Binary heap: Heap implementation.)

Heapsort uses two heap operations: insertion and root deletion. Each extraction places an element in the last empty location of the array. The remaining prefix of the array stores the unsorted elements.*/


template <typename T>
int cmpfun(T a, T b)
{
  if (a > b)
    return 1;
  else if (a < b)
    return -1;
  else
    return 0;
}
 

template <typename T>
void heap_sort(T* data, int (*func_pointer)(T, T), int hi) //<hi> is the last used position in the array to be sorted
{
	int half;
	int parents;

	int len = hi+1;

	if (len <= 1)
		return;

	half = len >> 1;
	for (parents = half; parents >= 1; --parents)
	{
	int tmp;
	int level = 0;
	int child;

	child = parents;
	/* bottom-up downheap */

	/* leaf-search for largest child path */
	while (child <= half)
	{
	  ++level;
	  child += child;
	  if ((child < len) &&
		  ((*func_pointer)(data[child], data[child - 1]) > 0))
		++child;
	}
	/* bottom-up-search for rotation point */
	tmp = data[parents - 1];
	for (;;)
	{
	  if (parents == child)
		break;
	  if ((*func_pointer)(tmp, data[child - 1]) <= 0)
		break;
	  child >>= 1;
	  --level;
	}
	/* rotate nodes from parents to rotation point */
	for (;level > 0; --level)
	{
	  data[(child >> level) - 1] =
		data[(child >> (level - 1)) - 1];
	}
	data[child - 1] = tmp;
	}

	--len;
	do
	{
	T tmp;
	int level = 0;
	int child;

	/* move max element to back of array */
	tmp = data[len];
	data[len] = data[0];
	data[0] = tmp;

	child = parents = 1;
	half = len >> 1;

	/* bottom-up downheap */

	/* leaf-search for largest child path */
	while (child <= half)
	{
	  ++level;
	  child += child;
	  if ((child < len) &&
		  ((*func_pointer)(data[child], data[child - 1]) > 0))
		++child;
	}
	/* bottom-up-search for rotation point */
	for (;;)
	{
	  if (parents == child)
		break;
	  if ((*func_pointer)(tmp, data[child - 1]) <= 0)
		break;
	  child >>= 1;
	  --level;
	}
	/* rotate nodes from parents to rotation point */
	for (;level > 0; --level)
	{
	  data[(child >> level) - 1] =
		data[(child >> (level - 1)) - 1];
	}
	data[child - 1] = tmp;
	} while (--len >= 1);
}

#endif
