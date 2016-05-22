#ifndef EXCELCONVERSIONS_H_
#define EXCELCONVERSIONS_H_

#include "dataStructures.h"
#include "mystuff/MotherOfAllBaseObjects.h"
//#include "FileSystems.h"
#include "xlw/xlw.h"

extern handleT handle;

xlw::XlfOper labelValuetoXlfOper(labelValue& lv);
void labelValueAddXlfOper(labelValue& lv, xlw::XlfOper xl);


#endif