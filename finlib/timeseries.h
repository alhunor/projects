#ifndef _TimeSeries_H_
#define _TimeSeries_H_

#include "dataStructures.h"
#include "fileSystems.h"
#include "MotherOfAllBaseObjects.h"

class series : public MotherOfAllBaseObjects
{
public:
	static series* createSeries(int n, float* ptr);
	float& operator[] (int n) {return data[n];}
	virtual const char* name() {return "series";};

protected:
	series() : initialised(false) {}
	bool init (int n, float* ptr);
	~series() { if (initialised) delete[] data;}

	float* data;
	int size;
	bool initialised;
};


/////////////   TimeSeriesFromFile      ///////////////////////////////////////////////
template <typename RecType>
class TimeSeriesFromFile : public MotherOfAllBaseObjects
{
public:
	TimeSeriesFromFile(char* Filename);
	~TimeSeriesFromFile(){mmdbt.unmap();}

	TimeSeriesFromFile(); // do not implement this

	//xlw::XlfOper list(int minPos, int maxPos, int step);
	const char* name() {return "TimeSeriesFromFile";}
	RecType* rec;
	int nbRecords;
	char* filename;

protected:
	memoryMappeddbTable mmdbt;
};

template <typename RecType>
TimeSeriesFromFile<RecType>::TimeSeriesFromFile(char* _filename)
{
	int len = strlen(_filename)+1;
	filename = new char[len];
	strcpy_s(filename, len, _filename);

	rec = (RecType*)mmdbt.map(filename);
	if (!rec)
		throw ("Error opening file");

	nbRecords =mmdbt.fileSize() / sizeof(RecType);
}


#endif

