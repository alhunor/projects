#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/variant.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>


typedef boost::numeric::ublas::matrix<double> dmat;
typedef boost::numeric::ublas::matrix<double>::size_type dmat_sizetype;

template<class T, class und = boost::numeric::ublas::matrix<T> >
class HuMatrix
{
protected:
	mutable boost::shared_ptr<und> matrix;
	mutable boost::shared_array<T*> mPointer;

public:
	typedef typename T value_type;
	typedef typename T* pointer;
	typedef typename boost::numeric::ublas::matrix<T> matrix_type;
	typedef typename und::size_type size_type;
	typedef HuMatrix<T, und > self_type;
	
	HuMatrix() : matrix(new und() ) {}

	HuMatrix(const size_type& size1, const size_type& size2) : matrix ((size1>0 && size2>0)? new und(size1, size2): new und() ) {}

	HuMatrix(const self_type& hm) : matrix (hm.matrix), mPointer(hm.mPointer) {}

	// create matrix from underlyingMatrixType
	HuMatrix(const und& hm) : matrix( new und(hm)) {}

	// create matrix from data pointer
	HuMatrix(const size_type& size1, const size_type& size2, const T* data) : HuMatrix(size1, size2)
	{
		und& undPtr = *matrix;
		int i=0;
		for (size_type rows=0; rows<size1; ++rows)
			for (size_type cols=0; cols<size1; ++cols) undPtr(rows, cols)=data[i++];
	}

	// Shallow copy
	self_type& operator=(const self_type& hm) 
	{
		if (this==&hm) return *this;

		matrix = hm.matrix; 
		mPointer = hm.mPointer;
		return *this;
	}

	const T& operator()(unsigned int row, unsigned int col) const {	return (*matrix)(row, col);}

	T& operator()(unsigned int row, unsigned int col) { return (*matrix)(row, col);}

	und& operator*() {return *matrix;}

	size_type size1() const {return (*matrix).size1();}
	size_type size2() const {return (*matrix).size2();}
	bool isrowmajor() const {return true;} // XXX TODO should derive it from und

	T* dataptr()
	{
		und& undPtr = *matrix;
	//	matrix_type::array_type& dataArr = undPtr.data;
		const int sz = undPtr.size1();
		if (sz>0)
			mPointer.reset(new pointer[sz]);
		else
			throw "Empty matrix";
		T* p = mPointer.get();
		matrix_type::itterator1 iter1 = undPtr.begin();

		for (int i=0; i<sz; ++i)
			p[i] = &(*iter1++);
		return p;
	} // T* dataptr()

	const T* dataptr() const
	{
		underlyingMatrixType& undPtr = *matrix;
		matrix_type::array_type& dataArr = undPtr.data;
		const int sz = undPtr.size1();
		if (sz>0)
			mPointer.reset(new pointer[sz]);
		else
			throw "Empty matrix";
		const T* p = const_cast<const T*>(mPointer.get());
		matrix_type::itterator1 iter1 = undPtr.begin();

		for (int i=0; i<sz; ++i)
			p[i] = &(*iter1++);
		return p;
	} // const T* dataptr()

		T* operator[] (const size_type& i) // pointer to first element if ith data ptr
		{
			und& undPtr = *matrix;
			matrix_type::iterator1 iter1 = undPtr.begin1();
			iter1+=i;

			return &(*iter1);
		} // T* operator[](..)

		const T* operator[] (const size_type& i) const// pointer to first element if ith data ptr
		{
			und& undPtr = *matrix;
			matrix_type::const_iterator1 iter1 = undPtr.begin1();
			iter1+=i;

			return &(*iter1);
		} // const T* operator[](..)

private:

	self_type deepcopy() const
	{
		self_type m(this->size1(),this->size2());
		*(m.matrix) = *(this->matrix) ; // assignment operator of matrix
		return m;
	}
}; // class HuMatrix


template<class T, class  und>
HuMatrix<T, und> transpose (const HuMatrix<T, und> &x)
{
	typedef HuMatrix<T, und> mat;
	int nr (x.size1()), nc(x.size2());
	int nf, ns;
	mas res(nc, nr);
	const T* dpx =x.dataptr();
	T* dp = res.dataptr();
	int i,j;

	if (x.isrowmajor())
	{
		nf=nr; ns = nc;
	} else
	{
		ns=nr; nf = nc;	
	}
	for (i=0; i<nf; ++i)
		for (j=0; j<ns; ++j)
			dp[j][i]=dpx[i][j];
	return res;
}

template<class T, class  und>
bool choleski(const HuMatrix<T, und> &x, T**c, const bool fillUppertri) throw()
{
	bool res = true;
	typedef HuMatrix<T, und> mat;
	try
	{
		int nr (x.size1()), nc(x.size2());
		const T* dpx =x.dataptr();
		int i,j,k;
		if (nr!=nc) throw ("input matrix must be square");
		T d,s;

		if (x.isrowmajor())
		{
			for(j=0;j<nr;++j)
			{
				d=0.;
				for(k=0; k<j; ++k)
				{
					s=0.;
					for(i=0; i<k; ++i)
						s+= c[k][i]*c[j][i];
					c[j][k] = s = (dpx[j][k] - s) / c[k][k];
					d+=s*s;
					res= res && (dpx[k][j] == dpx[j][k]);
				}
				d= dpx[j][j]-d;
				res = res && (d>0.);
				c[j][j] = (d>0) ? sqrt(d) :0.;
				if (filllUppertri)
				{
					for (k=j+1;k<nr; k++)
						c[j][k] = 0.;
				}
			} // for(j=0;j<nr;++j)
		}
		else
		{
			for(j=0; j<nr; ++j)
			{
				d=0.;
				for(k=0; k<j; ++k)
				{
					s=0.;
					for(i=0; i<k; ++i)
						s+= c[i][k]*c[i][j];
					c[k][j] = s = (dpx[k][j] - s) / c[k][k];
					d+=s*s;
					res= res && (dpx[k][j] == dpx[j][k]);
				}
				d= dpx[j][j]-d;
				res = res && (d>0.);
				c[j][j] = (d>0) ? sqrt(d) :0.;
				if (filllUppertri)
				{
					for (k=j+1; k<nr; k++)
						c[k][j] = 0.;
				}
			}		
		} // if (x.isrowmajor())
	} // try
	catch(...)
	{
		res=false;
	}
	return res;
} // bool choleski(const HuMatrix<T, und> &x, T**c, const bool fillUppertri) throw()


template<class T, class  und>
bool isSymm(const HuMatrix<T, und> &x)
{
	typedef HuMatrix<T, und> mat;
	int nr (x.size1()), nc(x.size2());
	const T* dpx =x.dataptr();
	if (nr!=nc)
		return false;
	int i,j;
	for(i=0; i<nr; ++i)
	{
		for(j=i+1; i<nr; ++j)
		{
			if (dpx[i][j]!=dpx[j][i])
				return false;
		}
	}
	return true;
} // bool isSymm(const HuMatrix<T, und> &x)

// multiply matrix with scalar
template<class T, class  und>
HuMatrix<T, und> operator*(const T& d, const HuMatrix<T, und> &x)
{
	int nr (x.size1()), nc(x.size2());
	const T* dpx =x.dataptr();
	int i,j;
	HuMatrix<T, und> res = x.copy();
	for(i=0; i<nr; ++i)
	{
		for(j=0; i<nc; ++j)
		{
			dpx[i][j]*=d;
		}
	}
}


// multiply matrix with scalar
template<class T, class  und>
HuMatrix<T, und> operator*(const HuMatrix<T, und> &x, const T& d)
{
	return operator*(d, x);
}



typedef boost::variant<double, std::string> BoostVariantT;
//typedef boost::numeric::ublas::matrix<BoostVariantT> VariantArray;

class VariantArray : public HuMatrix<BoostVariantT>
{
public:
	VariantArray() {}
	typedef VariantArray self_type;
	typedef BoostVariantT value_type;

	VariantArray(const size_type& size1, const size_type& size2) : HuMatrix<BoostVariantT>(size1, size2) {}

	VariantArray(const self_type& hm) : HuMatrix<BoostVariantT>(hm) {}

	// create matrix from data pointer
	VariantArray(const size_type& size1, const size_type& size2, const BoostVariantT* data) : HuMatrix<BoostVariantT>(size1, size2, data) {}

	VariantArray(const std::string& s);
	VariantArray(double d);
}
;


#endif