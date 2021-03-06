
#define IA 16807
#define IM 2147483647
#define AM (float)(1.0/IM)
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1+(IM-1)/NTAB)
#define EPS 1.2e-7f
#define RNMX (1.0f-EPS)

float ran1(long *idum)
{
	int j;
	long k;
	static long iy = 0;
	static long iv[NTAB];
	float temp;

	if (*idum <= 0 || !iy) {
		if (-(*idum) < 1) *idum = 1;
		else *idum = -(*idum);
		for (j = NTAB + 7; j >= 0; j--) {
			k = (*idum) / IQ;
			*idum = IA*(*idum - k*IQ) - IR*k;
			if (*idum < 0) *idum += IM;
			if (j < NTAB) iv[j] = *idum;
		}
		iy = iv[0];
	}
	k = (*idum) / IQ;
	*idum = IA*(*idum - k*IQ) - IR*k;
	if (*idum < 0) *idum += IM;
	j = iy / NDIV;
	iy = iv[j];
	iv[j] = *idum;
	if ((temp = AM*iy) > RNMX) return RNMX;
	else return temp;
}
#undef IA
#undef IM
#undef AM
#undef IQ
#undef IR
#undef NTAB
#undef NDIV
#undef EPS
#undef RNMX





#define norm 2.328306549295728e-10
#define m1 4294967087.0
#define m2 4294944443.0
#define a12 1403580.0
#define a13n 810728.0
#define a21 527612.0
#define a23n 1370589.0
double MRG32k3a(void)
{
	static double s10 = 12345, s11 = 12345, s12 = 123, s20 = 12345, s21 = 12345, s22 = 123;
	long k;
	double p1, p2;
	// Component 1 
	p1 = a12 * s11 - a13n * s10;
	k = (long) (p1 / m1);
	p1 -= k * m1;
	if (p1 < 0.0) p1 += m1;
	s10 = s11; s11 = s12; s12 = p1;
	// Component 2 
	p2 = a21 * s22 - a23n * s20;
	k = (long)(p2 / m2);
	p2 -= k * m2; 
	if (p2 < 0.0) p2 += m2;
	s20 = s21; s21 = s22; s22 = p2;
	// Combination 
	if (p1 <= p2) return ((p1 - p2 + m1) * norm);
	else return ((p1 - p2) * norm);
}
#undef norm
#undef m1
#undef m2
#undef a12
#undef a13n
#undef a21
#undef a23n



static unsigned int y = 2463534242U;
unsigned int xorshift(void)
{
	y ^= (y << 13);
	y ^= (y >> 17);
	return y ^= (y << 5);
}






#pragma warning (disable : 4267)

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>





/*========================== constants =================================*/

#define MaxChar 255                  /* max. length of a line */

#define Mess1 "\nUsage:   tcode  <FileIn>  <FileOut>\n\n"
#define Mess2 "\nERROR:   The 2 files must be different\n\n"




/*============================= variables ==============================*/

static char Line[MaxChar + 1] = {0}, /* One line of input */
   FIn[MaxChar + 1] = {0},           /* Input file name */
   FOut[MaxChar + 1] = {0};          /* Output file name */

static FILE *fin,                    /* Input file */
  *fout;                             /* Output file */

static size_t L1, L2, L3, L4, L5, L6, L7, L8; /* Lengths of strings */




/**************************************************************************/

static void Init (int argc, char *argv[])
{
   if (argc != 3) {
      printf (Mess1);
      exit (EXIT_FAILURE);
   }
   sprintf (FIn, "%.*s", MaxChar, argv[1]);
   sprintf (FOut, "%.*s", MaxChar, argv[2]);
   if (!strcmp (FIn, FOut)) {
      printf (Mess2);
      exit (EXIT_FAILURE);
   }

   errno = 0;
   fin = fopen (FIn, "r");
   if (fin == NULL) {
      printf ("\nOpening of %s failed: %s\n\n", FIn, strerror (errno));
      exit (EXIT_FAILURE);
   }
   errno = 0;
   fout = fopen (FOut, "w");
   if (fout == NULL) {
      printf ("\nOpening of %s failed: %s\n\n", FOut, strerror (errno));
      exit (EXIT_FAILURE);
   }

   L1 = strlen ("\\code");
   L2 = strlen ("\\endcode");
   L3 = strlen ("\\hide");
   L4 = strlen ("\\endhide");
   L5 = strlen ("\\iffalse");
   L6 = strlen ("\\fi");
   L7 = strlen ("\\smallc");
   L8 = strlen ("\\smallcode");
}

/**************************************************************************/

static void SearchRep (char *A, const char *sub, unsigned int L)
{
   /* Search for TEX command sub, of length L, in line A */
   char *p, *q;

   p = strstr (A, sub);              /* search for sub in A */
   if (p) {                          /* if found */
      if (isalpha ((int) *(p + L)))  /* is it only part of a command? */
         SearchRep (A + L, sub, L);  /* yes? search rest of line */
      else {
         q = p + L;                  /* no? delete */
         *p++ = ' ';
         while ((*p++ = *q++))
            ;
      }
   }
}

/**************************************************************************/

static void ProcessCode (char *A)
{

   SearchRep (A, "\\hide", L3);
   SearchRep (A, "\\endhide", L4);
   SearchRep (A, "\\iffalse", L5);
   SearchRep (A, "\\fi", L6);
   SearchRep (A, "\\smallcode", L8);
   SearchRep (A, "\\smallc", L7);

   fprintf (fout, "%s", A);
}

/**************************************************************************/

static int ProcessLine (char *line)
{
   /* Process a line (or part of a line) of valid code. If at the end of the
      line, we are still in a region of valid code, then return 1; otherwise,
      return 0. */

   char *p, *rest;
   int code;

   p = strstr (line, "\\endcode");   /* search for "\endcode" */
   if (p) {                          /* if found */
      *p = '\0';                     /* code ends here */
      rest = p + L2;                 /* step over "\endcode" */
      p = strstr (rest, "\\code");   /* search for "\code" */
      if (p) {
         p += L1;                    /* step over "\code" */
         rest = (char*)calloc (MaxChar, sizeof (char));
         strncpy (rest, p, MaxChar);
         ProcessCode (line);
         code = ProcessLine (rest);
         free (rest);
         return code;
      } else {
         ProcessCode (line);
         return 0;
      }
   } else {
      ProcessCode (line);
      return 1;
   }
}

/*
int main (int argc, char *argv[])
{
	char *p, *q;
   int isCode = 0;         /* If isCode == TRUE, we are in a region of valid code; otherwise not. 

   Init (argc, argv);

   while (NULL != fgets (Line, MaxChar, fin)) { // Not EOF and no error
      if (isCode) {
         isCode = ProcessLine (Line);
      } else {
         // search for "\def\code" and drop that line: it is not valid code but the definition of the TEX command \code
         if (strstr (Line, "\\def\\code"))
            ;                             
         else if ((p = strstr (Line, "\\code"))) {
            // search for "\code". If "\code" is found on a line with a % before it, then it is a TEX comment and we do not consider it as starting a region of valid code; 
            *p = '\0';
            q = strchr (Line, '%');
            if (NULL == q)
               // otherwise, it is valid code: process rest of line
               isCode = ProcessLine (p + L1);
         }
      }
   }

   fprintf (fout, "\n");
   fclose (fout);
   fclose (fin);
   return 0;
}
*/

#include "unif01.h"
#include "bbattery.h"
#include <math.h>
#include <conio.h>


void main(void)
{
	unif01_Gen *gen;
	gen = unif01_CreateExternGen01("MRG32k3a", MRG32k3a);
	bbattery_SmallCrush(gen);
	unif01_DeleteExternGen01(gen);
	gen = unif01_CreateExternGenBits("xorshift", xorshift);
	bbattery_SmallCrush(gen);
	unif01_DeleteExternGenBits(gen);
	_getche();
}