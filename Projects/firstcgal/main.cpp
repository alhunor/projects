#include <conio.h>
#include "heap.h"
#include "hftimer.h"
#include "filesystems.h"
#include "octree.h"
#include "triangulation.h"

using namespace std;



#define IA 16807
#define IM 2147483647
const float AM =(float) (1.0/IM);
#define IQ 127773
#define IR 2836
#define NTAB 32
const int NDIV = 1+(IM-1)/NTAB;
const float EPS = (float)1.2e-7;
const float RNMX =(float)(1.0-EPS);


void init_ran(long i)
{

}


float ran1(long *idum)
/* Minimum random number generator of Park an Miller with Bays-Durham
shuffle and added safeguards. Returns uniform random deviate between
0.0 and 1.0 (exclusive of the endpoint values).  Call with idum a
negative integer to initialize; thereafter, do not alter idum between
successive deviates in a sequence. RNMX should approximate the largest
floating value that is less than 1. */
{
	int j;
	long k;
	static long iy=0;
	static long iv[NTAB];
	float temp;

	if(*idum <= 0 || !iy)
	{
		if(-(*idum) < 1) 
		{
				*idum=1;
		} else
		{
			*idum = -(*idum);
		}
		for(j = NTAB+7;j >= 0;j--)
		{
			k = (*idum)/IQ;
			*idum = IA*(*idum - k*IQ) - IR*k;
			if(*idum < 0) *idum += IM;
			if(j < NTAB) iv[j] = *idum;
		}
		iy = iv[0];
	} // if(*idum <= 0 || !iy)
	k = (*idum)/IQ; // Start here when not initializing
	*idum = IA*(*idum - k*IQ) - IR*k; // Compute idum = (IA*idum) % IM without overflows by Schrage's method.
	if(*idum < 0) 
	{
		*idum += IM;
	}
	j = iy/NDIV; // Will be in the range 0..NTAB-1.
	iy = iv[j]; // Output previously stored value and refill the shuffle table. */
	iv[j] = *idum;
	temp = AM*iy;
	if (temp > RNMX) return RNMX; // Because users don't expect endpoint values.
	return temp;
} // end ran1


void TestHep()
{
int lv2data[] = {2,30,3,20,4,25,8,16,4,10,10,15,5,12,8,16,9,15,5};
	LightVector<int> lv (19, lv2data);
	intervalHeap<int> ih(lv);

	ih.push(1);
	ih.push(25);
	ih.popMin();
	ih.popMax();

	srand(56);
	intervalHeap<int> iheap;
	priorityArray<int> pa;
	int i, j, r;
	int count = 0;
	const int heapsize = 200;

	hftimer hf;

	hf.startTimer();

	for (i=0; i<40; ++i)
	{
		r = rand() % 100 - 50;
		iheap.push(r);
		pa.push(r);
		++count;
	}

	for (i=0; i<10000; ++i)
	{
		r = rand() % 100 - 50;
		j = rand() % 5;
		switch(j)
		{
		case 0:
		case 1:
		case 2:
			if (count<heapsize)
			{
				iheap.push(r);
				pa.push(r);
				++count;
			}
			break;
		case 3:
			if (count>0)
			{
				iheap.popMax();
				pa.popMax();
				--count;
			}
			break;
		case 4:
			if (count>0)
			{
				iheap.popMin();
				pa.popMin();
				--count;
			}
		} // switch(j)
	} // for (i=0; i<100; ++i)

	cout<<"Elapsed: "<<hf.getTime()<<endl;

	// compare heaps
	while (!iheap.empty())
	{
		cout<<iheap.peekMin()<<"  "<<pa.peekMin()<<endl;
		iheap.popMin();
		pa.popMin();
	}
}

struct esrirecord
{
	int recno;
	int recsize; 
};

class esri
{ // reads an SHP shapefile. See http://en.wikipedia.org/wiki/Shapefile or "ESRI Shapefile Technical Description.pdf" from folder
public:
	esri() {opened = false; polynr=0;}
	bool open(const char* filename);
	double xmin, ymin, xmax, ymax;
	void readRecord();
	bool eof() {return bs.eof();}

protected:
	bool opened;
	int filesize; //
	int type;
	binstream bs;
	int polynr;
};

bool esri::open(const char* filename)
{
	if (! bs.open(filename)) return false;

	int code = bs.read_bigendianint();
	if (code!=9994) throw "Error in file format";
	bs.skip(20);
	int filelen = bs.read_bigendianint() * 2;
	int version = bs.read_int();
	if (version !=1000) throw "Incorrect ESRI file version.";
	int type = bs.read_int();
	if (type!=5) throw "Program handle only polygon (type 5) files.";

	xmin = bs.read_double();
	ymin = bs.read_double();
	xmax = bs.read_double();
	ymax = bs.read_double();
	bs.skip(32); // position to the beginning of the first record header

	opened = true;
	return opened;
}

void esri::readRecord()
{
//	assumes cursor is set to the beginning of the next record directly on record header
	int recno = bs.read_bigendianint();
	int contentlen = bs.read_bigendianint()*2; 
	int rectype = bs.read_int();
//	cout<<recno<<"  "<<contentlen<<"  "<<rectype<<"  ";
	double x,y;
	if (rectype!=5)
	{
		cout<<"Skipping record"<<endl;
		bs.skip(contentlen-4);
		return;
	}
	// read bounding box for polygon
	double xmin, ymin, xmax, ymax;
	xmin = bs.read_double();
	ymin = bs.read_double();
	xmax = bs.read_double();
	ymax = bs.read_double();
	int numparts = bs.read_int();
//	cout<<numparts<<endl;
	int totalnumberofpoints = bs.read_int();
	int* parts = new int [numparts+1];
	int i,j;
	for (i=0; i<numparts; ++i)
	{
		parts[i] = bs.read_int();
	}
	parts[numparts]= totalnumberofpoints; // sentinel value
	// TODO properly recognise parts
	ofstream out;

	for (i=0; i<numparts; ++i) // iterating accross polynomial rings
	{
		++polynr;
		if (polynr==250)
		{
			out.open("hunor2.poly");
			out<<parts[i+1]-parts[i]-1<<"   "<<"2 0 0"<<endl;
		}

		for (j=parts[i]; j<parts[i+1]-1; ++j)
		{
			x = bs.read_double();
			y = bs.read_double();
			if (polynr==250)
			{
				out<<j-parts[i]<<"  "<< x <<"   "<< y <<endl;
//				out<<j-parts[i]<<"  "<< x <<"   "<< y <<"   0   1"<<endl;
			}
		};
		// eat up point
		x = bs.read_double();
		y = bs.read_double();

		if (polynr==250)
		{
			// segments
			out<<parts[i+1]-parts[i]-1<<"  0"<<endl;
			for (j=parts[i]; j<parts[i+1]-2; ++j)
			{
				out<<j-parts[i]<<"  "<< j <<"   "<< j+1 <<endl;
			}
			out<<j<<"  "<< j <<"   "<< 0 <<endl;
			out<< 0 <<endl;// number of holes
			out.close();
		}
	}
	delete[] parts;
}


void main()
{
	Voxel v;
	Triangulation* t;

	esri shapefile;
	if (! shapefile.open("L:\\cygwin64\\home\\hunor\\ne_10m_lakes.shp"))
	{
		cout<<"Error opening shapefile"<<endl;
		goto exit;
	}
	cout << " Bounding box (x0,y0, x2, y2) = (" << shapefile.xmin<<" , "<<shapefile.ymin<<" , "<<shapefile.xmax<<" , "<<shapefile.ymax<<")"<<endl;
	while (! shapefile.eof())
	{
		shapefile.readRecord();
	}
	


// initialiser lists: http://www.informit.com/guides/content.aspx?g=cplusplus&seqNum=424 




//	OctreeTest();


	goto exit;

	v.LoadRaw("terrain.raw", 2, 257, 257);
//	v.ExportOFF("terrain.off");
	
//	t = example();
	t = example2(v);
		
//	saveTriangulationToOFF("triangulation.off", t->tds());
	saveTriangulationToOFF("triangulation.off", *t);

//	cout<<*t<<endl;
	delete t;

exit:
	cout << "Press Any Key To Exit...";
	_getche();
}
