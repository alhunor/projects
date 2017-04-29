#define _CRT_SECURE_NO_WARNINGS

#include "ExcelConversions.h"
#include "myError.h"

/*
A	Boolean	short (0=false or 1=true)
B	double	
C, F	char *	Null-terminated ASCII byte string.
D, G	unsigned char *	Length -counted ASCII byte string.
H	unsigned short [int]	WORD
I	[signed] short [int]	16-bit
J	[signed long] int	32-bit
K	FP	Floating-point array structure.
R	XLOPER	Values, arrays, and range references.
*/


#include <xlw\xlw.h>

// Force export of functions implemented in XlOpenClose.h and required by Excel
#pragma comment (linker, "/export:_xlAutoOpen")
#pragma comment (linker, "/export:_xlAutoClose")

using namespace xlw;


// Input arrays _A, _B should be sorted in increasing order
extern "C" LPXLFOPER __declspec(dllexport) xlMergeSortRemoveDuplicates(XlfOper _A, XlfOper _B)
{
	EXCEL_BEGIN;

	// Checks if called from the function wizard
	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);
	try {
		MyArray A = _A.AsArray();
		MyArray B = _B.AsArray();
		int sizeA = A.size();
		int sizeB = B.size();
		MyArray C(sizeA+sizeB);

		int i, j, pos;
		i=j=pos=0;
		while (i<sizeA || j< sizeB)
		{
			if (i==sizeA)
			{
				C[pos]=B[j];
				++j;
				++pos;
				continue;
			}
			if (j==sizeB)
			{
				C[pos]=A[i];
				++i;
				++pos;
				continue;
			}
			if (A[i]<B[j])
			{
				C[pos]=A[i];
				++i;
				++pos;
				continue;
			}
			if (A[i]>B[j])
			{
				C[pos]=B[j];
				++j;
				++pos;
				continue;
			}
			// at this point A[i]==B[j], we skip this element
			++i; ++j;
		}
		C.resize(pos);
		XlfOper xlo = XlfOper(C);
		return xlo;
	}
	 catch (myError& m)
	{
		return XlfOper(m.msg);
	}
	catch (...)
	{
		return XlfOper("Something went wrong.");
	}
	EXCEL_END;
} // extern "C" LPXLFOPER __declspec(dllexport) xlMergeSortRemoveDuplicates(XlfOper _A, XlfOper _B)


// Input arrays _A, _B should be sorted in increasing order
extern "C" LPXLFOPER __declspec(dllexport) xlMergeSortDuplicatesOnly(XlfOper _A, XlfOper _B)
{
	EXCEL_BEGIN;

	// Checks if called from the function wizard
	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);
	try {
		MyArray A = _A.AsArray();
		MyArray B = _B.AsArray();
		int sizeA = A.size();
		int sizeB = B.size();
		MyArray C(sizeA+sizeB);

		int i, j, pos;
		i=j=pos=0;
		while (i<sizeA && j< sizeB)
		{
			if (A[i]<B[j])
			{
				++i;
				continue;
			}
			if (A[i]>B[j])
			{
				++j;
				continue;
			}
			// at this point A[i]==B[j], we skip this element
			C[pos]=A[i];
			++pos;
			++i; ++j;
		}
		C.resize(pos);
		XlfOper xlo = XlfOper(C);
		return xlo;
	}
	 catch (myError& m)
	{
		return XlfOper(m.msg);
	}
	catch (...)
	{
		return XlfOper("Something went wrong.");
	}
	EXCEL_END;
} // extern "C" LPXLFOPER __declspec(dllexport) xlMergeSortDuplicatesOnly(XlfOper _A, XlfOper _B)


namespace
{

//Array MergeSortRemoveDuplicates
	
	XLRegistration::Arg xlMergeSortRemoveDuplicates[] = {
        { "Vector1", "First Vector", "XLF_OPER" },
        { "Vector2", "Second Vector", "XLF_OPER" }
    };

    XLRegistration::XLFunctionRegistrationHelper registerMergeSortRemoveDuplicates
	(   "xlMergeSortRemoveDuplicates", "MergeSortRemoveDuplicates", "MergeSort arrays and removes duplicates.",
        "FinLib", xlMergeSortRemoveDuplicates, 2);

//Array MergeSortDuplicatesOnly
	
	XLRegistration::Arg xlMergeSortDuplicatesOnly[] = {
        { "Vector1", "First Vector", "XLF_OPER" },
        { "Vector2", "Second Vector", "XLF_OPER" }
    };

    XLRegistration::XLFunctionRegistrationHelper registerMergeSortDuplicatesOnly
	(   "xlMergeSortDuplicatesOnly", "MergeSortDuplicatesOnly", "MergeSort arrays and keeps duplicates only.",
        "FinLib", xlMergeSortDuplicatesOnly, 2);
}