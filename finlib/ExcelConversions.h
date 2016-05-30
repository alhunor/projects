#ifndef EXCELCONVERSIONS_H_
#define EXCELCONVERSIONS_H_

#include "dataStructures.h"
#include "market.h"
#include "mystuff/MotherOfAllBaseObjects.h"
//#include "FileSystems.h"
#include "xlw/xlw.h"

 
xlw::XlfOper labelValuetoXlfOper(labelValue& lv);
void labelValueAddXlfOper(labelValue& lv, xlw::XlfOper xl);

xlw::XlfOper toXlfOper(historyT& h); // ideally, we should have an XlfOper ctor from historyT


#endif