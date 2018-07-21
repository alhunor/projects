
/*
 Copyright (C) 1998, 1999, 2001, 2002 Jérôme Lecomte
 Copyright (C) 2007, 2008 Eric Ehlers

 This file is part of xlw, a free-software/open-source C++ wrapper of the
 Excel C API - http://xlw.sourceforge.net/

 xlw is free software: you can redistribute it and/or modify it under the
 terms of the xlw license.  You should have received a copy of the
 license along with this program; if not, please email xlw-users@lists.sf.net

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

// $Id: xlwExample.cpp,v 1.12 2003/03/08 22:21:02 jlecomte Exp $

#include <xlw/xlw.h>
#include <sstream>
#include <vector>

// Force export of functions implemented in XlOpenClose.h and required by Excel
#pragma comment (linker, "/export:_xlAutoOpen")
#pragma comment (linker, "/export:_xlAutoClose")

using namespace xlw;

void parameter(XlfOper x)
{
	INT32 i, rows;
	double value;
	char* name;

	rows = x.rows();
	if (x.columns()!=2)
		return;

	for (i = 0; i < rows; i++)
	{
        name = x(i, 0).AsString();
		value = x(i, 1).AsDouble();
	}
} // void parameter(XlfOper x)



extern "C" {

    LPXLFOPER __declspec(dllexport) xlSumArray(XlfOper xlArray)
	{
        EXCEL_BEGIN;

		parameter(xlArray);

		MyArray vec = xlArray.AsArray();
		

        // Multiplies it.
        double ret = 0;
		for (unsigned int i=0; i<vec.size(); ++i)
		{
			ret +=vec[i];
		}

        // Returns the result as an XlfOper.
        return XlfOper(ret);
        EXCEL_END;
    }


    LPXLFOPER EXCEL_EXPORT xlCirc(XlfOper xlDiam)
	{
        EXCEL_BEGIN;
        // Converts d to a double.
        double ret = xlDiam.AsDouble();
        // Multiplies it.
        ret *= 3.14159;
        // Returns the result as an XlfOper.
        return XlfOper(ret);
        EXCEL_END;
    }

    LPXLFOPER EXCEL_EXPORT xlConcat(XlfOper xlStr1, XlfOper xlStr2)
	{
        EXCEL_BEGIN;
        // Converts the 2 strings.
        std::wstring str1 = xlStr1.AsWstring();
        std::wstring str2 = xlStr2.AsWstring();
        // Returns the concatenation of the 2 string as an XlfOper.
        std::wstring ret = str1 + str2;
        return XlfOper(ret);
        EXCEL_END;
    }

    LPXLOPER EXCEL_EXPORT xlConcat4(XlfOper4 xlStr1, XlfOper4 xlStr2)
	{
        EXCEL_BEGIN;
        // Converts the 2 strings.
        std::string str1 = xlStr1.AsString();
        std::string str2 = xlStr2.AsString();
        // Returns the concatenation of the 2 string as an XlfOper.
        std::string ret = str1+str2;
        return XlfOper4(ret);
        EXCEL_END_4;
    }

    LPXLOPER12 EXCEL_EXPORT xlConcat12(XlfOper12 xlStr1, XlfOper12 xlStr2) {
        EXCEL_BEGIN;
        // Converts the 2 strings.
        std::wstring str1 = xlStr1.AsWstring();
        std::wstring str2 = xlStr2.AsWstring();
        // Returns the concatenation of the 2 string as an XlfOper.
        std::wstring ret = str1+str2;
        return XlfOper12(ret);
        EXCEL_END_12;
    }


    /*! 
     * Demonstrates how to detect that the function is called by
     * the function wizard, and how to retrieve the coordinates
     * of the caller cell
     */
    LPXLFOPER EXCEL_EXPORT xlIsInWiz()
	{
        EXCEL_BEGIN;

        // Checks if called from the function wizard
        if (XlfExcel::Instance().IsCalledByFuncWiz())
            return XlfOper(true);

        // Gets reference of the called cell
        XlfOper res;
        XlfExcel::Instance().Call(xlfCaller,res,0);
        XlfRef ref = res.AsRef();

        // Returns the reference in A1 format
        std::ostringstream ostr;
        char colChar = 'A' + ref.GetColBegin();
        ostr << colChar << ref.GetRowBegin() + 1 << std::ends;
        std::string address = ostr.str();

        return XlfOper(address.c_str());

        EXCEL_END;
    }

    /*!
     * Registered as volatile to demonstrate how functions can be 
     * recalculated automatically even if none of the arguments 
     * has changed.
     *
     * \return the number of times the function has been called.
     */
    LPXLFOPER EXCEL_EXPORT xlNbCalls()
	{
        EXCEL_BEGIN;

        static short nbCalls = 0;
        ++nbCalls;
        return XlfOper(nbCalls);

        EXCEL_END;
    }
} // extern "C"

namespace
{

    // Register the function Circ.

    XLRegistration::Arg CircArgs[] = {
        { "diameter", "Diameter of the circle", "XLF_OPER" }
    };


    XLRegistration::XLFunctionRegistrationHelper registerCirc(
        "xlCirc", "Circ", "Compute the circumference of a circle",
        "xlw Example", CircArgs, 1);


    // Register the function SumArray.
	
    XLRegistration::Arg SumArrayArgs[] = {
        { "arrayElements", "Elements of the array", "XLF_OPER" }
    };
    XLRegistration::XLFunctionRegistrationHelper registerSumArray(
        "xlSumArray", "SumArray", "Summs the elemets of an array",
        "xlw Example", SumArrayArgs, 1);
	
	
	// Register the function Concat.

    XLRegistration::Arg ConcatArgs[] = {
        { "string1", "First string", "XLF_OPER" },
        { "string2", "Second string", "XLF_OPER" }
    };

    XLRegistration::XLFunctionRegistrationHelper registerConcat(
        "xlConcat", "Concat", "Concatenate two strings",
        "xlw Example", ConcatArgs, 2);

    // Register the function Concat4.

    XLRegistration::Arg Concat4Args[] = {
        { "string1", "First string", "P" },
        { "string2", "Second string", "P" }
    };

    XLRegistration::XLFunctionRegistrationHelper registerConcat4(
        "xlConcat4", "Concat4", "Concatenate two strings",
        "xlw Example", Concat4Args, 2, false, false, "P");

    // Register the function Concat12.

    XLRegistration::Arg Concat12Args[] = {
        { "string1", "First string", "Q" },
        { "string2", "Second string", "Q" }
    };

    XLRegistration::XLFunctionRegistrationHelper registerConcat12(
        "xlConcat12", "Concat12", "Concatenate two strings",
        "xlw Example", Concat12Args, 2, false, false, "Q");


    // Register the function IsInWiz.

    XLRegistration::XLFunctionRegistrationHelper registerIsInWiz(
        "xlIsInWiz", "IsInWiz", "Return true if the function is called "
        "from the function wizard, and the address of the caller otherwise",
        "xlw Example");

    // Register the function NbCalls as volatile.

    XLRegistration::XLFunctionRegistrationHelper registerNbCalls(
        "xlNbCalls", "NbCalls", "Return the number of times the function "
        "has been calculated since the xll was loaded (volatile)",
        "xlw Example", 0, 0, true);
}
