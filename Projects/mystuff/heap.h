#ifndef _HEAP_H
#define _HEAP_H

#include <functional>
#include "vector.h"

// http://faculty.cs.niu.edu/~freedman/340/340notes/340heap.htm

template <typename T>
class minHeap
{
public:
	minHeap() : internalStorage(true), data(* new LightVector<T>()) {}
	minHeap(LightVector<T> _data) : data(_data), internalStorage(false) {}
	minHeap(int initialSize) : data(*new LightVector<T>(initialSize)), internalStorage(false) {}
	~minHeap() {if (internalStorage) delete &data;}
	typedef T value_type;
	T popTop(bool makeheap=true);
	T top() const {return data[0];}
	bool empty() const {return data.size()==0;}
	void makeHeap();
	int quality() const;
	void push(T item, bool makeheap=true);
	void change(int h, T newValue);

protected:
	LightVector<T>& data; // references externally stored data
	std::less<T> smaller;
	int percolateDown(int root);
	int percolateUp(int h);
	bool internalStorage;
};

// returns the number of roots that violate the heap condition
template <typename T>
int minHeap<T>::quality() const
{
	int n =  data.size();
	int q = 0;
	for (int i = n/2-1; i>=0; --i)
	{
		if (comp(data[i*2+1], data[i]))
		{
			++q;
		} else if (2*i+2<n && comp(data[i*2+2],data[i]))
		{
			++q;
		}
	}
	return q;
} // int minheap::quality() const


template <typename T>
int minHeap<T>::percolateDown(int root)
{
	int n =  data.size();
	T tmp = data[root];
	int child = 2* root + 1; // start with left child

	while (child <n)
	{
		if ((child <n-1) && smaller(data[child+1], data[child]))
		{
			++child;
		}
		// child holds the position of the smallest sibling
		if ( smaller(tmp, data[child])) break;
		data[root] = data[child];
		root = child;
		child = 2*child +1;
	}
	data[root] = tmp;
	return root;
} // int minheap::percolateDown(int root)



template <typename T>
int minHeap<T>::percolateUp(int h)
{
	int parent =  (h-1)/2;
	T item = data[h];

	while ((h>0) && smaller(item, data[parent]))
	{
		data[h] = data[parent];
		h = parent;
		parent = (h-1)/2;
	}
	data[h] = item;
	return h;
} // int minheap::percolateUp(int h)


// removes the smallest element from the heap. If makeheap=true(default) it rearranges the data to satisfy the heap condition
template <typename T>
T minHeap<T>::popTop(bool makeheap)
{
	int n =  data.size();
	if (n==0) return T();
	T tmp = data[0];
	--n;
	data[0] = data[n];
	data.setSize(n);
	if (n>0 && makeheap)
	{
		percolateDown(0);
	}
	return tmp;
} // T minheap::pop()


// inserts an element into the heap. If makeheap=true(default) it rearranges the data to satisfy the heap condition
template <typename T>
void minHeap<T>::push(T item, bool makeheap)
{
	data.push_back(item);
	if (!makeheap) return;
	int h =  data.size();
	percolateUp(h-1); //percolate last position
} // int minheap::insert(T item)


template <typename T>
void minHeap<T>::makeHeap()
{
	int n =  data.size()/2-1; // index of the last subtree root. All further positions are leaves
	for (int i = n; i>=0; --i)
	{
		percolateDown(i);
	}
} // void minheap::makeHeap()


template <typename T>
void minHeap<T>::change(int pos, T newValue)
// overwrites the value of the lement in position "pos" with newValue
{
	if (smaller(data[pos],newValue))
	{
		data[pos] = newValue;
		percolateDown(pos);
	} else
	{
		data[pos] = newValue;
		percolateUp(pos);
	}
} // void minheap::change(int pos, T newValue)




template <typename T>
class ksmallest
{
// when used wiith external storage, size of vector should be 1 position bigger than what is needed - used for a sentinel
public:
	ksmallest();
	ksmallest(int _k) : k(_k), internalStorage(true), data(* new LightVector<T>(_k+1)) {}
	ksmallest(int _k, LightVector<T> _data) : k(_k), data(_data), internalStorage(false) {}
	~ksmallest() {if (internalStorage) delete &data;}
	typedef T value_type;

	bool empty() const {return data.size()==0;}
	void push(const T& item);
	void list();

protected:
	LightVector<T>& data; // references externally stored data
	std::less<T> comp;
	bool internalStorage;
	unsigned int k;
};

template <typename T>
void ksmallest<T>::push(const T& item)
{
	unsigned int n = data.size();
	if (n<k) data.setSize(n+1);
	data[n] =  item;
	T tmp = item;
	T tmp2;
	unsigned int i = 0;
	while (i<n)
	{
		if (comp(tmp,data[i]))
		{
			tmp2 = tmp;
			tmp = data[i];
			data[i] = tmp2;
		}
		++i;
	}
	data[n] = tmp;
};

template <typename T>
void ksmallest<T>::list()
{
	unsigned int n = data.size();
	for (unsigned int i = 0; i<n; ++i)
	{
		cout<<data[i]<<" ";
	}
	cout<<endl;
}


template <typename T>
class intervalHeap
{
// storage starts at zero. Even elements form a minHeap, odd elements a maxheap. root will be represented by { a=data[0], b=data[1] }
public:
	intervalHeap() : nbElements(0) {}
	intervalHeap(LightVector<T>& _data) : nbElements(_data.size()) {data = _data;}
	~intervalHeap() {}
	typedef T value_type;
	T popMin(bool makeheap=true);
	T popMax(bool makeheap=true);
	T peekMin() const;
	T peekMax() const;
	bool empty() const {return nbElements==0;}
	void makeHeap();
	void push(const T item, bool makeheap=true);
//	void change(int h, T newValue);

protected:
	LightVector< T > data; // stored data, may be internal or external
	int nbElements;

	std::less<T> smaller;
	int percolateDownMin(int root);
	int percolateDownMax(int root);
	int percolateUp(int h);
	int parent(int h) {return ((h-2)/4)*2;}
	int leftchild(int h) {return (h-h%2)*2+2;} // point at the minheap part in the left subtree
	int rightchild(int h) {return (h-h%2)*2+4;} // point at the minheap part in the right subtree
}; // intervalHeap

template <typename T>
T intervalHeap<T>::peekMin() const
{
	if (nbElements>0)
	{
		return data[0]; 
	} else
	{
		throw "Not enough elements";
	}
}

template <typename T>
T intervalHeap<T>::peekMax() const
{
	if (nbElements>1)
	{
		return data[1]; 
	} else 	if (data.size()>0)
	{
		return data[0]; 
	} else
	{
		throw "Not enough elements";
	}
}


// removes the smallest element from the heap. If makeheap=true(default) it rearranges the data to satisfy the interval heap condition
template <typename T>
T intervalHeap<T>::popMin(bool makeheap=true)
{
	if (nbElements==0) throw "Heap is empty";
	T tmp = data[0];
	--nbElements;
	data[0]=data[nbElements];
	if (nbElements>1 && makeheap)
	{
		percolateDownMin(0);
	}
	// we need to resize data to make sure that an insert following a pop knows that there is free space in data and 
	// can insert at the next free position without reallocating and leaving gaps.
	data.setSize(nbElements);
	return tmp;
} // T intervalHeap::popMin(bool makeheap=true)


// removes the biggest element from the heap. If makeheap=true(default) it rearranges the data to satisfy the interval heap condition
template <typename T>
T intervalHeap<T>::popMax(bool makeheap)
{
	T tmp;
	if (nbElements == 0)
	{
			throw "Heap is empty";
	} else
	{
		--nbElements;
		if (nbElements == 0)
		{
			tmp = data[0];
		} else
		{
			tmp = data[1];
			data[1] = data[nbElements];
		}
	}
	// we need to resize data to make sure that an insert following a pop knows that there is free space in data and 
	// can insert at the next free position without reallocating and leaving gaps.
	data.setSize(nbElements);
	if (nbElements>1 && makeheap)
	{
		percolateDownMax(1);
	}
	return tmp;
} // T intervalHeap::popMax()


template <typename T>
int intervalHeap<T>::percolateDownMin(int root)
{
	// last element is data[nbElements-1]
	// data[root] needs to be inserted into minheap
	T tmp = data[root];
	int lchild, rchild, child;
	lchild =leftchild(root); // start with left child
	rchild =rightchild(root); // start with left child

	while (lchild <nbElements)
	{
		child = lchild;
		// child holds the position of the smallest sibling
		if ((rchild<nbElements) && smaller(data[rchild], data[lchild]))
		{
			child=rchild;
		}

		// swap elements to make sure that the even element is always smaller than odd element
		if (child+1<nbElements && smaller(data[child+1], tmp))
		{
			swap(data[child+1], tmp);
		}
	
		if ( smaller(tmp, data[child])) break;

		data[root] = data[child];
		root = child;
		lchild =leftchild(root); // start with left child
		rchild =rightchild(root); // start with left child
	}
	data[root] = tmp;
	return root;
} // int intervalHeap::percolateDownMin(int root)


template <typename T>
int intervalHeap<T>::percolateDownMax(int root)
{
	// last element is data[nbElements-1]
	// data[root] needs to be inserted into minheap
	T tmp = data[root];
	int lchild, rchild, child;
	lchild =leftchild(root)+1; // max element in left child
	rchild =rightchild(root)+1; // max element in right child

	while (lchild <nbElements)
	{
		child = lchild;
		// child shoul hold holds the position of the bigggest sibling
		if ((rchild<nbElements) && smaller(data[lchild], data[rchild]))
		{
			child=rchild;
		}

		// swap elements to make sure that the even element is always smaller than odd element
		if (smaller(tmp, data[child-1]))
		{
			swap(data[child-1], tmp);
		}
	
		if ( smaller(data[child], tmp)) break;

		data[root] = data[child];
		root = child;
		lchild =leftchild(root)+1; // start with left child
		rchild =rightchild(root)+1; // start with left child
	}
	data[root] = tmp;
	return root;
} // int intervalHeap::percolateDownMax(int root)



template <typename T>
int intervalHeap<T>::percolateUp(int h) // insert new element from bottom
{
	// elements are stored in data[0..h], the root being in data[0..1]
	// this function is called when new element has already been placed in data[h]
	if (h==0) return 0;
	if (h==1)
	{
		if (smaller(data[1], data[0]))
		{
			swap (data[1], data[0]);
			return 0;
		} else
		{
			return 1;
		}
	}

	if (h % 2!=0) // last leaf already had one element an fills up with insertion of new one
	{
		// retaure interval property if needed
		if (smaller(data[h], data[h-1]))
		{
			// restaure interval property
			swap (data[h], data[h-1]);
			h--;
		}
	}
	// data[h] contains the new element at this stage after a potential swap in the last node if needed
	// if there was a swap, the right element will be replaced by the lement from the previous minheap leaf, 
	// thus by the interval property the maxheap is valid, we only need to rebuild the minheap by gradually moving data[h] upwards
	T item = data[h];
	int p = parent(h); // parent node in interval minheap or maxheap

	// if data[h]>data[parent(h)+1] insert in maxheap, else insert in minheap

	if (data[h]>data[p+1]) 
	{
		//insert in maxheap
		++p; // move to right end of the parent interval
		while ((h>1) && smaller(data[p], item))
		{
			data[h] = data[p];
			h = p;
			p = parent (p) + 1; // use right end of the parent interval
			// check and restore interval property not needed, as new item is bigger than the biggest element in the minheap
		}
	} else
	{ 
		//insert in minheap
		while ((h>0) && smaller(item, data[p]))
		{
			data[h] = data[p];
			h = p;
			p = parent(p);
			// check and restore interval property not needed, as new item is smaller than the smallest element in the maxheap
		}
	}
	data[h] = item;
	return h;
} // int intervalHeap::percolateUp(int h)







// inserts an element into the heap. If makeheap=true(default) it rearranges the data to satisfy the heap condition
template <typename T>
void intervalHeap<T>::push(T item, bool makeheap)
{
	nbElements++;
	data.push_back(item);
	if (!makeheap) return;
	percolateUp(nbElements-1); //percolate last position
} // int intervalHeap::insert(T item)


template <typename T>
void intervalHeap<T>::makeHeap()
{
	need to write this for intervalheap
	int n =  data.size()/2-1; // index of the last subtree root. All further positions are leaves
	for (int i = n; i>=0; --i)
	{
		percolateDown(i);
	}
} // void intervalHeap::makeHeap()


template <typename T>
class priorityArray
{
public:
	priorityArray() : nbElements(0) {}
	priorityArray(LightVector<T>& _data) : nbElements(_data.size()) {data = _data;}
	~priorityArray() {}
	typedef T value_type;
	T popMin();
	T popMax();
	T peekMin() const;
	T peekMax() const;
	bool empty() const {return nbElements==0;}
	void sort();
	void push(T item, bool makeheap=true);
//	void change(int h, T newValue);

protected:
	LightVector< T > data; // stored data, may be internal or external
	int nbElements;
	std::less<T> smaller;
};

template <typename T>
T priorityArray<T>::peekMin() const
{
	if (nbElements>0)
	{
		return data[0]; 
	} else
	{
		throw "Not enough elements";
	}
}


template <typename T>
T priorityArray<T>::peekMax() const
{
	if (nbElements>0)
	{
		return data[nbElements-1]; 
	} else
	{
		throw "Not enough elements";
	}
}

template <typename T>
void priorityArray<T>::sort()
{
	int i,j;
	bool unchanged;
	for (i=0; i<mbElements; ++i)
	{
		unchanged = true;
		for (j=i+1; j<mbElements; ++j)
		{
			if (smaller(data[j], data[i]))
			{
				swap(data[j], data[i]);
				unchanged = false;
			}
		}
		if (unchanged) break;
	}
}

template <typename T>
void priorityArray<T>::push(const T item, bool sorted=true)
{
	data.push_back(item);
	int i = nbElements++;
	if (sorted)
	{
		while (i>0 && data[i]<data[i-1])
		{
			swap(data[i], data[i-1]);
			--i;
		}
	}
}

template <typename T>
T priorityArray<T>::popMax()
{
	if (nbElements<1)
	{
		throw "Not enough elements";
	}
	T tmp = data[--nbElements];
	data.setSize(nbElements);
	return tmp;
}

template <typename T>
T priorityArray<T>::popMin()
{
	if (nbElements<1)
	{
		throw "Not enough elements";
	}
	T tmp = data[0];
	--nbElements;
	for (int i=0; i<nbElements; ++i)
	{
		data[i]=data[i+1];
	}
	data.setSize(nbElements);
	return tmp;
}


#endif