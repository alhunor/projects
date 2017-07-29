#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <boost/shared_ptr.hpp>
#include <initializer_list>

template<class T> class LightVector;

template<class T>
class und
{
//private:
public:
	typedef typename T value_type;
	und(unsigned int _allocatedSize) : allocatedSize(_allocatedSize), nb(0), hasSpecial(false) {if (allocatedSize>0) data = new T[allocatedSize]; }
	und(const und& u);
	T& operator[] (unsigned int pos) { return data[pos]; }
	const T& operator[] (unsigned int pos) const { return data[pos]; }
	void clear() {nb=0;}
	unsigned int size() const {return nb;}
	void push_back(T element);
	void setSize(unsigned int newSize); // changes size with reallocation if required, but preserves current elements
	void increaseAllocatedSize(unsigned int newAllocatedSize);

	int contains(T element);
	void fill(T v);
	void fill(unsigned int n, T* values);
	T* dataPtr() {return data;}

	unsigned int allocatedSize; 
	unsigned int nb;
	T* data;
	T special;
	bool hasSpecial;
	friend class LightVector<T>;
	void addSpecial(const T& specialValue) {special = specialValue; hasSpecial=true;}
public:
	// public destructor can be called by shared pointers and other 
	~und() {if (allocatedSize>0) delete[] data;}
};





template<class T>
void und<T>::fill(T v)
{
	for (unsigned int i=0; i<nb; ++i)
	{
		data[i]=v;
	}
}

template<class T>
void und<T>::fill(unsigned int n, T* values)
{
	if (n<nb) throw "Too few data points.";
	// does not channge the size of the vector
	for (unsigned int i=0; i<nb; ++i)
	{
		data[i]=values[i];
	}
}


template<class T>
void und<T>::setSize(unsigned int newSize)
{
	if (newSize>allocatedSize) 
	{
		increaseAllocatedSize(newSize);
	}
	nb = newSize;
}

template<class T>
void und<T>::increaseAllocatedSize(unsigned int newAllocatedSize)
{
	T* newPtr = new T[newAllocatedSize];
	if (nb>0) memcpy( newPtr , data, nb*sizeof(T));
	if (allocatedSize>0) delete[] data;
	data =  newPtr;
	allocatedSize = newAllocatedSize;
}


/*
template<class T>
void und<T>::resize(unsigned int n)
{
	if (n>allocated)
	{
		if (allocated>0) delete[] data;
		data =  new T[n];
		allocated = n;
	}
	nb = n;
}*/

template<class T>
void und<T>::push_back(T element)
{
	if (nb==allocatedSize) 
	{
		if (allocatedSize==0)
		{ 
			increaseAllocatedSize(4);
		} else
		{
			increaseAllocatedSize(allocatedSize*2);
		}
	} // if (nb==allocated) 
	data[nb] = element;
	nb++;
}


// returns position if found, else -1
template<class T>
int und<T>::contains(T element)
{
	unsigned int i;
	for (i=0; i<nb; ++i)
	{
		if (data[i]==element) return i;
	}
	return -1;
} 


template<class T>
und<T>::und(const und& u) : allocatedSize(u.allocatedSize), nb(u.nb), hasSpecial(u.hasSpecial), special(u.special)
{
	if (allocatedSize>0)
		data = new T[allocatedSize];
	memcpy(data, u.data, sizeof(T) * nb);
}




template<class T>
class LightVector
{
	mutable boost::shared_ptr< und<T> > und_data;
public:
	typedef typename T value_type;
	typedef LightVector<T> self_type;

	LightVector() : und_data(new und<T>(0)) {}
	explicit LightVector(const unsigned int initialSize) : und_data(new und<T>(initialSize)) { }
	explicit LightVector(const unsigned int initialSize, T* values) : und_data( new und<T>(initialSize)) { und_data->setSize(initialSize); und_data->fill(initialSize, values); }
	explicit LightVector(const unsigned int initialSize, T value) : und_data( new und<T>(initialSize)) { und_data->setSize(initialSize);  und_data->fill(value); }

	LightVector(const LightVector<T>& v) : und_data(new und<T>(*(v.und_data))) {}
	~LightVector() {  }
	LightVector<T>& operator=(const LightVector<T>& v);

	void clear() {und_data->clear();}
	void addSpecial(const T& specialValue) {und_data->addSpecial(specialValue);}
	void fill(T v) { und_data->fill(v);}

	inline unsigned int size() const {return und_data->size();}
	void setSize(unsigned int newSize) {und_data->setSize(newSize);}
	int contains(T element) {return und.contains(element); } // returns position if found, else -1
	T& operator[](int pos);
	const T& operator[](int pos) const;
	void push_back(T element)
	{
		und<T>* und = und_data.get();
		und->push_back(element);
		//und_data->push_back(element);
	}
	T* dataPtr() {return und->dataPtr();}
};


template<class T>
T& LightVector<T>::operator[] (int pos)
{
	if (pos<0)
	{
		if (und_data->hasSpecial) return und_data->special;
		throw "negative indices suppported only after addition of a special value";
	}
	if ((unsigned int)pos>=und_data->allocatedSize) throw "Index exceeds vector size.";

	return und_data->operator[](pos);
}

template<class T>
const T& LightVector<T>::operator[] (int pos) const
{
	if (pos<0)
	{
		if (und_data->hasSpecial) return und_data->special;
		throw "negative indices suppported only after addition of a special value";
	}
	if ((unsigned int)pos>=und_data->allocatedSize) throw "Index exceeds vector size.";

	return und_data->operator[](pos);
}


template<class T>
LightVector<T>& LightVector<T>::operator=(const LightVector<T>& v)
{
	if (this==&v) return *this;
//	und_data.reset();
	// TODO check this line, I believe that existing und_data should be destroyed if a new data is associated to the vector
	und_data = v.und_data;
	return *this;
}

#endif