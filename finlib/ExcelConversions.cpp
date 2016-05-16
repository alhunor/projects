#include "ExcelConversions.h"
#include "dataStructures.h"
#include <xlw\xlw.h>

using namespace xlw;

/////////////   labelValue      ///////////////////////////////////////////////

XlfOper labelValuetoXlfOper(labelValue& lv)
{
	XlfOper	ret(lv.data.size(), 2);
	double d;
	std::string* s;
	int i=0;

	for ( std::map<std::string, valueT>::iterator it = lv.data.begin(); it!=lv.data.end(); ++it)
	{
		ret.SetElement(i,0, it->first);
		valueT v = it->second;
		if (v.isString)
		{
			s = (std::string*)(v.p);
			ret.SetElement(i, 1, *s);
		} else
		{
			d = *(double*)(it->second.p);
			ret.SetElement(i, 1, d);		
		}
		++i;
	} // for
	return ret;
} // XlfOper labelValuetoXlfOper(labelValue& lv)


void labelValueAddXlfOper(labelValue& lv, xlw::XlfOper xl)
{
	char *tmp;
	std::string name, value;

	if  (xl.columns()!=2)
		throw "Xloper should have exacly rwo columns.";

	for ( int i = 0; i < xl.rows(); i++)
	{
		tmp = xl(i, 0).AsString();
		name =tmp;
		if (xl(i, 1).IsString())
		{
			tmp = xl(i, 1).AsString();
			value = tmp;
			lv.add(name, value);
		} else
		{
			double d = xl(i, 1).AsDouble();
			lv.add(name, d);
		} // if
	} // for
} // void labelValueAddXlfOper(xlw::XlfOper xl)

