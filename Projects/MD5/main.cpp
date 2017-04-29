#include "md5.h"
#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>

#pragma warning (disable : 4996)
//using namespace std;
#include "filesystems.h"


// Writes the message digest of the data from stdin onto stdout, followed by a carriage return.
static void MDFilter ()
{
  MD5_CTX mdContext;
  int bytes;
  unsigned char data[16];

  MD5Init (&mdContext);
  while ((bytes = fread (data, 1, 16, stdin)) != 0)
    MD5Update (&mdContext, (char*)data, bytes);
  MD5Final (&mdContext);
  MDPrint (&mdContext);
  printf ("\n");
}

// Runs a standard suite of test data.
static void MDTestSuite ()
{
  printf ("MD5 test suite results:\n\n");
  MDString ("");
  MDString ("a");
  MDString ("abc");
  MDString ("message digest");
  MDString ("abcdefghijklmnopqrstuvwxyz");
  MDString
    ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
  MDString
    ("1234567890123456789012345678901234567890\
1234567890123456789012345678901234567890");
  /* Contents of file foo are "abc" */
  MD5_CTX mdc;
  MDFile ("foo", &mdc);
}


// A time trial routine, to measure the speed of MD5. Measures wall time required to digest TEST_BLOCKS * TEST_BLOCK_SIZE characters.
// size of test block
#define TEST_BLOCK_SIZE 1000
// number of blocks to process
#define TEST_BLOCKS 10000
// number of test bytes = TEST_BLOCK_SIZE * TEST_BLOCKS
static long TEST_BYTES = (long)TEST_BLOCK_SIZE * (long)TEST_BLOCKS;
static void MDTimeTrial ()
{
  MD5_CTX mdContext;
  time_t endTime, startTime;
  unsigned char data[TEST_BLOCK_SIZE];
  unsigned int i;

  /* initialize test data */
  for (i = 0; i < TEST_BLOCK_SIZE; i++)
    data[i] = (unsigned char)(i & 0xFF);

  /* start timer */
  printf ("MD5 time trial. Processing %ld characters...\n", TEST_BYTES);
  time (&startTime);

  /* digest data in TEST_BLOCK_SIZE byte blocks */
  MD5Init (&mdContext);
  for (i = TEST_BLOCKS; i > 0; i--)
    MD5Update (&mdContext, (char*)data, TEST_BLOCK_SIZE);
  MD5Final (&mdContext);

  /* stop timer, get time difference */
  time (&endTime);
  MDPrint (&mdContext);
  printf (" is digest of test input.\n");
  printf
    ("Seconds to process test input: %ld\n", (long)(endTime-startTime));
  printf
    ("Characters processed per second: %ld\n",
     TEST_BYTES/(endTime-startTime));
}



int main (int argc, char** argv)
{
	char* nf;
	char* dir = "L:\\temp";
//	dir = "C:\\Users\\hunor\\Google Drive\\articles";
	dir = "e:\\GoogleDrive";
	char aux[1024];
	MD5_CTX mdc;

	fileFinder ff;
	if ( ! ff.init(dir)) 
	{
		std::cout<<"Error opening "<<dir<<std::endl;
		return 0;
	}
	while ( !ff.eof())
	{
		nf = ff.nextfile();
		strcpy(aux, ff.getpath());
		strcat(aux, "\\");
		strcat(aux, nf);
		if (MDFile(aux, &mdc))
		{
			MDPrint(&mdc);
			std::cout<<" "<<aux<<std::endl;
		}
	}

//MDFilter ();
//MDString (argv[0] + 2);
//MDTimeTrial ();
//MDTestSuite ();
}