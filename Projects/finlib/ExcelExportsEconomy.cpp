// Ecoomy Module, supporting my aticle "Machine Learning in Simulated Economies"

#define _CRT_SECURE_NO_WARNINGS


#include "myStuff/FileSystems.h"
#include "myError.h"
#include "ExcelConversions.h"

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
#include "randomNumbers.h"
#include "myStuff/Matrix.h"


const int N = 10;

using namespace xlw;

extern "C" LPXLFOPER __declspec(dllexport) xlAddSource(XlfOper sourceDescriptor)
{
	EXCEL_BEGIN;

	// Checks if called from the function wizard
	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);
	try {
		labelValue lv;
		if (!sourceDescriptor.IsMissing())
			labelValueAddXlfOper(lv, sourceDescriptor);

		string sourceName = lv.getStr("name");
		if (sourceName == "Brownian")
		{
			MessageBox(NULL, "Brownian", "Brownian title", MB_OK);
			//source = BlackScholes;
		}

		
		return XlfOper("Not implemneted.");
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




namespace
{

//Toy BGM
	XLRegistration::Arg xlAddSource[] = {
        { "Source Descriptor", "name, type, freq, params, etc.", "XLF_OPER" }
    };

    XLRegistration::XLFunctionRegistrationHelper registerToyBGM
	(   "xlAddSource", "addSoource", "Creates a source of randomness.",
        "FinLib", xlAddSource, 1);
}
