#define _CRT_SECURE_NO_WARNINGS
#include "myStuff/FileSystems.h"
#include "myError.h"
#include "ExcelConversions.h"
#include "ImageProcessing.h"

#include "myStuff/hftimer.h"


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

inline double square(double x) {return x*x;}

#include <xlw\xlw.h>

// Force export of functions implemented in XlOpenClose.h and required by Excel
#pragma comment (linker, "/export:_xlAutoOpen")
#pragma comment (linker, "/export:_xlAutoClose")

using namespace xlw;

extern "C" LPXLFOPER __declspec(dllexport) xlOpenImageFile(XlfOper FileNameWithPath)
{
	EXCEL_BEGIN;

	// Checks if called from the function wizard
	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);
	try {
		char* filename = FileNameWithPath.AsString();
		int ret = OpenImageFile(filename);
		return XlfOper((double)ret);
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
}

extern "C" LPXLFOPER __declspec(dllexport) xlSaveImageFile(XlfOper FileNameWithPath, int h)
{
	EXCEL_BEGIN;

	// Checks if called from the function wizard
	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);


	double ret = 0;

	return XlfOper(ret);
	EXCEL_END;
}


extern "C" LPXLFOPER __declspec(dllexport) xlProcessImage(int h)
{
	EXCEL_BEGIN;

	// Checks if called from the function wizard
	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);


	double ret = 0;

	return XlfOper(ret);
	EXCEL_END;
}

namespace
{

//ImageProcessing
	XLRegistration::Arg xlOpenImageFile[] = {
        { "FileNameWithPath", "FileName with Path", "XLF_OPER" }
    };

    XLRegistration::XLFunctionRegistrationHelper registerOpenImageFile
	(   "xlOpenImageFile", "OpenImageFile", "Opens a TGA image file.",
        "FinLib", xlOpenImageFile, 1);

	XLRegistration::Arg xlSaveImageFile[] = {
        { "FileNameWithPath", "FileName with Path", "XLF_OPER" },
        { "Handle", "Handle to Internal Image Object", "J"}
    };

    XLRegistration::XLFunctionRegistrationHelper registerSaveImageFile
	(   "xlSaveImageFile", "SaveImageFile", "Saves a TGA image file.",
        "FinLib", xlSaveImageFile, 2);

	XLRegistration::Arg xlProcessImage[] = {
        { "Handle", "Handle to Internal Image Object", "J"}
    };

    XLRegistration::XLFunctionRegistrationHelper registerProcessImage
	(   "xlProcessImage", "ProcessImage", "Processes internal image object.",
        "FinLib", xlProcessImage, 1);
}
