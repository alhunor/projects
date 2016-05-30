#include "mystuff/FileSystems.h"
#include "ImageProcessing.h"
#include "myError.h"
#include <WinDef.h>

class tga : public MotherOfAllBaseObjects
{
public:
	// Documented as per http://en.wikipedia.org/wiki/Truevision_TGA
	BYTE IDLength; // Length of the image ID field
	BYTE ColorMapType; // Whether a color map is included
	BYTE ImageType; //Compression and color types
	BYTE ColorMap[5]; // Describes the color map
	WORD xOrigin;
	WORD yOrigin;
	WORD width;
	WORD height;
	BYTE pixelDepth; // byte in the original format, declared int here for convenience
	BYTE descriptor;

	tga() : data(NULL) {}
	~tga() {if (data==NULL) delete[] data;}

	int ReadFromStream(binstream& b);
	virtual const char* name() {return "TrueVision TGA";}

protected:
	char* data; //image data
};

int tga::ReadFromStream(binstream& b)
{
	IDLength=(BYTE)b.read_byte();

	ColorMapType=(BYTE)b.read_byte();
	if (ColorMapType!=0) throw myError("Color Maps not supported");

	ImageType=(BYTE)b.read_byte();
//	if (ImageType!=3) throw myError("Only uncompressed true-color images are supported");
	if (ImageType!=10) throw myError("Only RLE true-color images are supported");

	b.skip(5);

	xOrigin = b.read_int16();
	yOrigin = b.read_int16();
	width = b.read_int16();
	height = b.read_int16();
	
	pixelDepth = (BYTE) b.read_byte()>>3;
	if (pixelDepth!=3)  throw myError("Only 24 bit RLE true-color images are supported");

	descriptor = (BYTE)b.read_byte();
// Used 18 bytes until now

	if (IDLength>0) b.skip(IDLength); // skip image identification field if present
	
	// There will be Color map data. 

// allocate memory to hold image data
	int totalLen = width*height*pixelDepth;
	data = new char[totalLen];
	int pos = 0;
	BYTE packetType;
	int RLELen;
	BYTE color[4];
	int i;

// read the data

	while (pos < totalLen)
	{
		// get packer header
		packetType = b.read_byte();
		RLELen = (packetType & 127) +1;
		if (packetType & 128) //RLE packet
		{
			b.readblock(color,pixelDepth);
			for (i=0; i<RLELen; ++i)
			{
				memcpy(data+pos,color,pixelDepth);
			}
			pos += RLELen * pixelDepth; // willl not overflow, mult done on 32 bits
		}
		if (packetType < 128) //RAW packet
		{

		}
	}

	return 0; // 0 is the error value
}

int OpenImageFile(char* filename)
{
	binstream b(filename, false);
	tga tga;
	tga.ReadFromStream(b);
	return 0;
}

int ProcessImage(int handle)
{
	if (handle==0) throw myError("Invalid handle");

	return 0;
}

int SaveImageFile(char* filename, int handle)
{
	if (handle==0) throw myError("Invalid handle");

	return 0;
}