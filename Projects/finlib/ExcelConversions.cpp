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


xlw::XlfOper toXlfOper(historyT& h)
{
	xlw::XlfOper xlo(h.getRows() + 1, 2);
	xlo.SetElement(0, 0, "Tick");
	xlo.SetElement(0, 1, "buy/sell");
	HistoricRecordT hr;

	int j = 1;
	for (std::list<HistoricRecordT>::iterator it = h.beginHistory(); it != h.endHistory(); ++it)
	{
		hr = *it;
		xlo.SetElement(j, 0, (double)hr.tick);
		xlo.SetElement(j, 1, hr.order);
		j++;
	}
	return xlo;
} // xlw::XlfOper* toXlfOper(historyT& h)

