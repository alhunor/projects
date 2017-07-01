#ifndef DATES_H_
#define DATES_H_

extern int dateFromString(const char* ptr); // // Only convers from format like 27-Feb-11. Case sensitive

void ExcelDateToYMD(int nSerialDate, int &nYear, int &nMonth, int &nDay);
int YMDToExcelDate(int nYear, int nMonth, int nDay);

#endif