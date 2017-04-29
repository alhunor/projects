#define _CRT_SECURE_NO_WARNINGS
#include "myStuff/FileSystems.h"
#include "myError.h"
#include "ExcelConversions.h"

#include "myStuff/hftimer.h"


/*
A	Boolean	short (0=false or 1=true)
B	double	
C, F	char *	Null-terminated ASCII byte string.
D, G	unsigned char *	Length -counted ASCII byte string.
H	unsigned short [int]	WORD
I	[signed] short [int]	16-bit
J	[signed long] int	32-bit
K	FP	Floating-point array structure.
R	XLOPER	Values, arrays, and range references.
*/

inline double square(double x) {return x*x;}

#include <xlw\xlw.h>
#include "randomNumbers.h"
#include "myStuff/Matrix.h"


const int N = 10;

class ToyBGM
{
public:
	bool init();
	void simulate();
	ToyBGM() : nbSteps(N), f(N + 1, N + 1), ninv(N+2, N+2), lib(N + 1, N + 1) { init(); }

protected:
	int nbSteps;
	double DFs[N+2] = {1,0.9985, 0.9950, 0.9930, 0.9900, 0.9876, 0.9853, 0.9820, 0.9801, 0.9777, 0.9743, 0.9712};
	double DCFs[N+1] = {0.23, 0.25, 0.27, 0.26, 0.25, 0.265, 0.2, 0.3, 0.25, 0.255, 0.25};
	double tenors[N+1] = {0, 0.25, 0.5, 0.75, 1, 1.25, 1.50, 1.75, 2, 2.25, 2.5}; // set times
	double alpha[N + 1] = { 0.2, 0.21, 0.18, 0.22, 0.19, 0.195, 0.17, 0.15, 0.155, 0.16, 0.2 };
//	double alpha[N + 1] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	double beta[N+1] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1,1};
	double fwd[N+1] = {0.01, 0.012, 0.0125, 0.012, 0.011, 0.010, 0.009, 0.095, 0.01, 0.088, 0.01};
	double sqrt_dt[N + 1], dt[N + 1];
	double spread[N + 1];

	double dw[N+1], dw0[N+1]; // Brownian increments in the simulation measure. dw[j] is between t[j] and t[j+1]

	HuMatrix<double> ninv; // inverse numeraire used for moving cashflows

public:
	HuMatrix<double> f; // f[Libor_i][time_j]
	HuMatrix<double> lib; // f[Libor_i][time_j]
	double payoff;
}; // class ToyBGM


bool ToyBGM::init()
{
	int i, j;
	double gamma;
	for (i = 0; i <= N; ++i)
	{
		dt[i] = tenors[i + 1] - tenors[i];
		sqrt_dt[i] = sqrt(dt[i]);
		alpha[i] /= 10;
		for (j = 1; j <= N; ++j) // iterate over time steps
		{
			f[i][j] = 0;
		}

		f(i,0) = (DFs[i] / DFs[i + 1] - 1) / (DCFs[i]);
		spread[i] = fwd[i] - f[i][0];
		gamma = fwd[i];
	}
	initRandomNumbers(13);
	payoff = 0;

	return true;
} // bool ToyBGM::init()


void ToyBGM::simulate()
{
	// simulation is done in terminal measure
	int i, j; // Libor_i, time_j
	double gamma = 0; // change of measure coeff
	static bool havePath = false;

	if (!havePath)
	{
		for (j = 0; j <= N; ++j)
		{
			dw0[j] = sqrt_dt[j] * rand<Normalnversion>();
			dw[j] = dw0[j];
		}
		havePath = true;
	}
	else
	{
		for (j = 0; j <= N; ++j)
		{
			dw[j] = -dw0[j];
		}
		havePath = false;
	}

	for (j = 1; j <= N; ++j)
	{
		f(N,j) = f(N,j-1) + dw[j-1] * alpha[N] * beta[j-1];
	}
	for (i = N - 1; i > 0; --i) // iterate over libors
	{
		for (j = 1; j <= i; ++j) // iterate over time steps
		{
			// convert dw to the previous measure
			gamma = alpha[i + 1] * beta[j - 1] / (1 + DCFs[j - 1] * f(i + 1,j - 1));
			dw[j - 1] = dw[j - 1] - gamma * dt[j - 1];
			// compuet libor rate
			f(i,j) = f(i,j-1) + dw[j-1] * alpha[i] * beta[j-1];
		}
		// compute inverse numeraires
		ninv(i, N+1) = 1;
		for (j = N-1; j >= i; --j) // iterate over time steps
		{
			gamma = ninv(i, j + 1) * (1 + DCFs[j] * f(i, j));
			ninv(i, j) = ninv(i, j+1) * (1+ DCFs[j]*f(i, j));
		}
	}

	// Libor fixing wlll be in L[i][i]
	for (i = 0; i <= N; ++i) // iterate over libors
	{
		for (j = 0; j <= i; ++j)
		{
			lib[i][j] = f[i][j] + spread[i];
		}
		for (; j <= N; ++j)
		{
			lib[i][j] = 0;
		}
	}
	gamma = lib[N - 1][N - 1];
	gamma *= ninv(N - 1, N - 1);
	payoff += lib[N - 1][N - 1] * ninv(N - 1, N - 1);
} // void ToyBGM::simulate()

using namespace xlw;

extern "C" LPXLFOPER __declspec(dllexport) xlToyBGM(XlfOper bgm)
{
	EXCEL_BEGIN;

	// Checks if called from the function wizard
	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);
	try {
		ToyBGM tbgm;
		tbgm.init();
		double ff = 0;
		for (int i = 0; i < 1000; ++i)
		{
			tbgm.simulate();
			ff += tbgm.payoff;
		}

		return XlfOper(tbgm.lib);
	}
	 catch (myError& m)
	{
		return XlfOper(m.msg);
	}
	catch (...)
	{
		return XlfOper("Something went wrong.");
	}
	EXCEL_END;
}




namespace
{

//Toy BGM
	XLRegistration::Arg xlToyBGM[] = {
        { "Anything", "BGM data (not used)", "XLF_OPER" }
    };

    XLRegistration::XLFunctionRegistrationHelper registerToyBGM
	(   "xlToyBGM", "ToyBGM", "Starts the ToyBGM model.",
        "FinLib", xlToyBGM, 1);

}
