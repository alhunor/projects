#define _CRT_SECURE_NO_WARNINGS
#include "BlackScholes.h"
#include "ExcelConversions.h"
#include "FileSystems.h"
#include "LinearAlgebra.h"
//#include "lm.h"
#include <levmar.h>

#include "optimizer.h"

#include "hftimer.h"
#include "md5.h"
#include "editDistance.h"

//#include "parameters.h"

double sq(double x) {return x*x;}


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


#include "xlw/xlw.h"


// Force export of functions implemented in XlOpenClose.h and required by Excel
#pragma comment (linker, "/export:_xlAutoOpen")
#pragma comment (linker, "/export:_xlAutoClose")

using namespace xlw;
using namespace std;

extern "C" LPXLFOPER __declspec(dllexport) xlOptionPricer(double forward, double strike, double atmVol, double time, XlfOper ModelDescriptor)
{
	typedef enum {BlackScholes=0, NormalBlackScholes, Hagan} modelType;
	EXCEL_BEGIN;

	// Checks if called from the function wizard
	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

	labelValue lv;
	if (!ModelDescriptor.IsMissing())
		lv.add(ModelDescriptor);

	modelType model;
	if (lv.getHandle("Model")==-1) 
	{
		model = BlackScholes;
	} else
	{
		string modelName = lv.getStr("Model");
		if (modelName == "BlackScholes")
		{
			model = BlackScholes;
		} else if (modelName == "NormalBlackScholes")
		{
			model = NormalBlackScholes;
		} else if (modelName == "Hagan")
		{
			model = Hagan;
		}
	}
	double ret;

	switch (model)
	{
		case BlackScholes:
			ret = black(forward, strike, atmVol, time);
			break;
		case NormalBlackScholes:
			ret = normalblack(forward, strike, atmVol, time);
			break;
		case Hagan:
			{
			double sigma0 = lv.getNum("sigma0");
			double volvol = lv.getNum("volvol");
			double rho = lv.getNum("rho");
			double beta = lv.getNum("beta");

			ret = hagan(forward, strike, sigma0, volvol, rho, beta, time);
			}
			break;
		default:
			ret = black(forward, strike, atmVol, time);
	}

	return XlfOper(ret);
	EXCEL_END;
}



extern "C" LPXLFOPER __declspec(dllexport) xlBlackScholes(double forward, double strike, double atmVol, double time)
{
	EXCEL_BEGIN;

	// Checks if called from the function wizard
	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

	double ret = black(forward, strike, atmVol, time);

	return XlfOper(ret);
	EXCEL_END;
}




extern "C" LPXLFOPER __declspec(dllexport) xlCholesky(XlfOper Matrix)
{
	EXCEL_BEGIN;

	// Checks if called from the function wizard
	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

	MyMatrix m = Matrix.AsMatrix();

	unsigned int i,j;

	for (i=0; i<m.size1(); i++)
	{
		for (j=0; j<m.size2(); j++)
		{
			m(i,j) += 10;
		}
	}
	return XlfOper(m);
	EXCEL_END;
}

class Dynamics
{
public:
	Dynamics();
	Dynamics(double _f) : f(_f) {}
	virtual MyArray simulate(int n, double timesteps)=0;
protected:
	double f; // forward at time 0.
};

class LognormalDynamics : public Dynamics
{
public:
	LognormalDynamics();
	LognormalDynamics(double f, double _vol) : Dynamics(f), vol(_vol) {}
	virtual MyArray simulate(int n, double timesteps);
protected:
	double vol;
};



MyArray LognormalDynamics::simulate(int n, double dt)
{
	MyArray ma(n);
	double prev = f;
	double sigma2t= 0.5 *vol*vol* dt;
	double sqrtdt =sqrt(dt);
	double dw = .1;
	for (int i=0; i<n; ++i)
	{
		//ma(i)= prev;
		//dw = ran()*sqrtdt;
		prev *= exp(vol*dw-sigma2t);
	
	}
	//ma(n-1) = prev;
	return ma;
}

Dynamics* createDynamics(labelValue& arg)
{
	const char* dynamics = arg.getStr("dynamics").c_str();
	double f = arg.getNum("forward");
	Dynamics* dyn;

	if (_stricmp(dynamics, "LogNormal")==0)
	{
		double vol = arg.getNum("vol");
		dyn = new LognormalDynamics(f, vol);
		return dyn;
	}
	return NULL;
}






extern "C" LPXLFOPER __declspec(dllexport) xlSimulatePath(XlfOper Command, XlfOper Arg)
// returns handle to timeseries object in dbRecordT[] format.
{
	EXCEL_BEGIN;

	// Checks if called from the function wizard
	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);


	labelValue arg;
	arg.add(Command);
	const char* command = arg.getStr("command").c_str();
	if (_stricmp(command, "SimulateDynamics")==0)
	{
		int n = (int)arg.getNum("nbPoints");
		double pointsPerYear = arg.getNum("pointsPerYear");
		Dynamics* dyn = createDynamics(arg);
		if (!dyn) return XlfOper("Unknown dynamics.");

		dyn->simulate(n, 1/pointsPerYear);
	}

	return XlfOper(1.0);
	EXCEL_END;
}


extern "C" LPXLFOPER __declspec(dllexport) xlEigenValuesVectors(XlfOper Matrix)
{
	EXCEL_BEGIN;

	// Checks if called from the function wizard
	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

	MyMatrix m = Matrix.AsMatrix();
	unsigned int dim = m.size1();

	double *data = &m(0,0);

	gsl_vector *eval = gsl_vector_alloc (dim);
	gsl_matrix *evec = gsl_matrix_alloc (dim, dim);

	eigen(data, dim, eval, evec);

	MyMatrix output(dim+1, dim);
	unsigned int i,j;

	for (j=0; j<dim; j++)
	{
		output(0,j)=gsl_vector_get (eval, j);
		for (i=0; i<dim; i++)
		{
			output(i+1,j) = gsl_matrix_get(evec, i,j);
		}
	}

	gsl_vector_free (eval);
	gsl_matrix_free (evec);

	return XlfOper(output);
	EXCEL_END;
} // extern "C" LPXLFOPER __declspec(dllexport) xlEigenValuesVectors(XlfOper Matrix)



extern "C" LPXLFOPER __declspec(dllexport) xlMatProd(XlfOper Matrix1, XlfOper Matrix2)
{
	EXCEL_BEGIN;

	// Checks if called from the function wizard
	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

	MyMatrix m1 = Matrix1.AsMatrix();
	MyMatrix m2 = Matrix2.AsMatrix();

	MyMatrix res = matProd(m1, m2);

	return XlfOper(*res);
//	return XlfOper("To be implemented.");
	EXCEL_END;
}


extern "C" LPXLFOPER __declspec(dllexport) xlWriteData(XlfOper data, XlfOper label, double value)
{
	EXCEL_BEGIN;

	// Checks if called from the function wizard
	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

	if (data.IsMissing())
		return XlfOper("Data is missing.");

	if (label.IsMissing())
		return XlfOper("Label is missing.");

	labelValue arg;
	arg.add(data);

	char* ll = label.AsString();

	arg.setNum(ll, value);

	return XlfOper(arg.toXlfOper());

	EXCEL_END;
}




extern "C" LPXLFOPER __declspec(dllexport) xlFrequencyDistribution(XlfOper data)
{
	EXCEL_BEGIN;

	// Checks if called from the function wizard
	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

	if (data.IsMissing())
		return XlfOper("Data is missing.");

	MyStringArray msa = data.AsStringArray();
	std::map<std::string, int> elements;
	std::map<std::string, int>::iterator mapit;
//	std::pair<std::map<std::string, int>::iterator, bool> res;

	int i, count=msa.size();
	for (i=0; i<count; ++i)
	{
		mapit = elements.find(msa[i]);
		if (mapit!=elements.end())
		{
			mapit->second++;
		} else
		{
			elements[msa[i]]=1;
		};
	} // for (i=0; i<count; ++i)

	labelValue arg;

	for (mapit=elements.begin(); mapit!=elements.end(); ++mapit)
	{
		arg.add(mapit->first, mapit->second);
	}
	return XlfOper(arg.toXlfOper());
	EXCEL_END;
} // xlFrequencyDistribution(XlfOper data)


char hexcode(char c)
{
	return c<10?c+48:c+55;
}

extern "C" LPXLFOPER __declspec(dllexport) xlmd5file(XlfOper filename)
{
	EXCEL_BEGIN;

	// Checks if called from the function wizard
	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

	if (filename.IsMissing())
		return XlfOper("Data is missing.");

	string s = filename.AsString();
	char ret[33];

	MD5_CTX mdc;

	if (MDFile(s.c_str(), &mdc))
	{
		for (int i = 0; i < 16; i++)
		{
			ret[2*i] = hexcode(mdc.digest[i] / 16) ;
			ret[2*i+1] = hexcode(mdc.digest[i] % 16) ;
		}
	} else return XlfOper("MD5 could not me computed.");
	ret[32]=0;

	return XlfOper(ret);
	EXCEL_END;
} // xlmd5file(XlfOper data)



double x_sinx(double x, void* param)
{
	return x*sin(x)-0.5;
}

extern "C" LPXLFOPER __declspec(dllexport) xlTest(XlfOper BLOB)
{
	EXCEL_BEGIN;

	// Checks if called from the function wizard
	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

	// solves for complex function with 1 root.
	double r = gsl_rootFinder(x_sinx, 0, 1, NULL);

	// solves for roots of a polynomial
	double coeff[4];
	coeff[0]=+1; coeff[1]=0; coeff[2]=0; coeff[3]=1;
	HuMatrix<double> res = gsl_polyRoots(4, coeff);


	return XlfOper(*res);

	EXCEL_END;
} // extern "C" LPXLFOPER __declspec(dllexport) xlTest(XlfOper BLOB)





extern "C" LPXLFOPER __declspec(dllexport) xlGraphAlgoTest(XlfOper TestDescriptor, XlfOper matrix)
{
	EXCEL_BEGIN;

	// Checks if called from the function wizard
	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

	labelValue lv;
	if (!TestDescriptor.IsMissing())
		lv.add(TestDescriptor);

	MyMatrix m1 = matrix.AsMatrix();

	const char* algo = lv.getStr("Algo").c_str();
	int startnode;
	if (_stricmp(algo,"dijkstra")==0)
	{
		startnode = (int)lv.getNum("StartNode");
	}

	double ret = m1.size1();

	return XlfOper(ret);
	EXCEL_END;
}




extern "C" LPXLFOPER __declspec(dllexport) xlEditDistance(XlfOper X, XlfOper Y, int mode=0)
{
	EXCEL_BEGIN;

	// Checks if called from the function wizard
	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

	char *x, *y;
	x ="Bamba jancsi";
	y ="aBmba janosika";
	x = "   alma";
	y = "Al ma";

	if (X.IsMissing() || Y.IsMissing())
	{
		return XlfOper("Missing parameter");
	}

	x = X.AsString();
	y = Y.AsString();
	HuMatrix<int> res;

	int d=0;
	switch (mode)
	{
		case 0:
			d = editDistance(x, y);
			break;
		case 1:
			d = editDistanceWithCost(x, y);
			//res = editDistanceWithCost(x, y);
			break;
		case 2:
			d = editDistanceWithOverlapDetection(x, y);
			//res = editDistanceWithOverlapDetection(x, y);
			break;
	}

	//res(0,0)=d;
	return XlfOper(d);
	//return XlfOper(*res);
	EXCEL_END;
} // extern "C" LPXLFOPER __declspec(dllexport) xlEditDistance(XlfOper BLOB)

struct mvT {gsl_vector * v; gsl_matrix *m;};

int gsl_planes (const gsl_vector * x, void * params, gsl_vector * fx)
{
// x - cutrent point, params - function parameters, fx - f(x)
	mvT* mv = (mvT*)params;
	gsl_matrix * m = mv->m;
	gsl_vector * v = mv->v;
	int nbPoints = m->size1;
	int i;
	double dist=0;
	double a = gsl_vector_get(x, 0);
	double b = gsl_vector_get(x, 1);
	double c = gsl_vector_get(x, 2);
	double d = gsl_vector_get(x, 3);

	for (i=0; i<nbPoints; ++i)
	{
		dist += a*gsl_matrix_get(m, i,0)+ b*gsl_matrix_get(m, i,1) +c*gsl_matrix_get(m, i,2)+d - v->data[i];
	}
	// return value
	gsl_vector_set(fx,0,dist);
	gsl_vector_set(fx,1,dist);
	gsl_vector_set(fx,2,dist);
	gsl_vector_set(fx,3,dist);
	return GSL_SUCCESS;
}

int lm_func (const gsl_vector * x, void * params, gsl_vector * fx)
{
// x - cutrent point, params - function parameters, fx - f(x)
	mvT* mv = (mvT*)params;
	gsl_matrix * m = mv->m;
	gsl_vector * v = mv->v;
	int nbPoints = m->size1;
	int i;
	double dist=0;
	double a = gsl_vector_get(x, 0);
	double b = gsl_vector_get(x, 1);
	double c = gsl_vector_get(x, 2);
	double d = gsl_vector_get(x, 3);

	for (i=0; i<nbPoints; ++i)
	{
		dist += a*gsl_matrix_get(m, i,0)+ b*gsl_matrix_get(m, i,1) +c*gsl_matrix_get(m, i,2)+d - v->data[i];
	}
	// return value
	gsl_vector_set(fx,0,dist);
	gsl_vector_set(fx,1,dist);
	gsl_vector_set(fx,2,dist);
	gsl_vector_set(fx,3,dist);
	return GSL_SUCCESS;
}


void lm_func(double *x, double *fx, int nbVars, int n, void *data)
{
	// fills <fx = f(x)> vector for levmar algorithm

	gsl_matrix * m = (gsl_matrix * )data;
	int nbPoints = m->size1;
	int i;
	double dist=0;
	double a = 1; // we divided all params by a to make sure we exclude trivial solution (i.e. all coeffs at zero)
	double b = x[0];
	double c = x[1];
	double d = x[2];

	for (i=0; i<nbPoints; ++i)
	{
		fx[i] = gsl_matrix_get(m, i,0)+ b*gsl_matrix_get(m, i,1) +c*gsl_matrix_get(m, i,2) + d;
	}
} //void qual(double *p, double *x, int m, int n, void *data)



extern "C" LPXLFOPER __declspec(dllexport) xlMultiLinearRegression(XlfOper xGrid, XlfOper b)
{
	EXCEL_BEGIN;

	// Checks if called from the function wizard
	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

	if (xGrid.IsMissing() || b.IsMissing())
	{
		return XlfOper("Missing parameter");
	}

	gsl_matrix* m = xGrid.AsGSLMatrix();
	gsl_vector* v = b.AsGSLVector();

	int nbDataPoints = v->size;
	int nbVars = m->size2;
	if (nbVars > nbDataPoints) throw "Not enough datapoints.";

	MyMatrix res(nbVars, 1);

	if (true)
	{
		// optimization control parameters; passing to levmar NULL instead of opts reverts to defaults
		double opts[LM_OPTS_SZ], info[LM_INFO_SZ];
		opts[0]=LM_INIT_MU;
		opts[1]=1E-15;
		opts[2]=1E-15;
		opts[3]=1E-20;
		opts[4]=LM_DIFF_DELTA; // relevant only if the finite difference Jacobian version is used 
		int i;
		// initial guess;
		double *x0 = (double *) calloc(4, sizeof(double));
		if (x0!=NULL)
		{
			double *target = new double[nbDataPoints];
			for (i=0; i<nbDataPoints; ++i)
			{
				target[i]=0;
			}
			// solves for complex functions with many roots
			double ret=dlevmar_dif( lm_func, x0, target, 3, nbDataPoints, 100, opts, info, NULL, NULL, m); // without Jacobian
//			printf("Levenberg-Marquardt returned in %g iter, reason %g, sumsq %g [%g]\n", info[5], info[6], info[1], info[0]);
			delete[] target;

			for (int i=0; i<3; ++i)
			{
				res(i,0) = x0[i];
			}
		} // if (x0!=NULL)
		free(x0);
	} else
	{
		gsl_vector* a = gsl_multiLinearRegression(m,v);
		for (int i=0; i<nbVars; ++i) res(i,0) = a->data[i];
		gsl_vector_free (a);
	}

	gsl_vector_free (v);
	gsl_matrix_free (m);

	return XlfOper(res);

	EXCEL_END;
} // extern "C" LPXLFOPER __declspec(dllexport) xlMultiLinearRegression(XlfOper xGrid, XlfOper b)




namespace
{

	//Test
	XLRegistration::Arg TestArg[] = {
        { "BLOB", "BLOB", "XLF_OPER" }
    };

    XLRegistration::XLFunctionRegistrationHelper registerTest
	(   "xlTest", "Test", "Test Data",
        "FinLib", TestArg, 1);

	//xlEditDistance
	XLRegistration::Arg EditDistanceArgs[] = {
        { "X", "Y", "XLF_OPER" },
        { "Y", "Y", "XLF_OPER" },
		{ "WeigthedDistance", "int DistanceType {0- standard, 1 - weighted, 2 - weigted overlapping}", "I" }
    };
    XLRegistration::XLFunctionRegistrationHelper registerEditDistanceArgs
	(   "xlEditDistance", "EditDistance", "Computes the edit distance between two strings X and Y.",
        "FinLib", EditDistanceArgs, 3);

	//WriteData
	XLRegistration::Arg WriteDataArgs[] = {
        { "Data", "Data", "XLF_OPER" },
        { "Label", "Label", "XLF_OPER" },
        { "Value", "Value", "B" }
    };
    XLRegistration::XLFunctionRegistrationHelper registerWriteDataArgs
	(   "xlWriteData", "WriteData", "Changes the value of a label in <labelValue> structure.",
        "FinLib", WriteDataArgs, 3);

//OptionPricer
	XLRegistration::Arg OptionPricerArgs[] = {
        { "Forward", "Forward", "B" },
        { "Strike", "Strike of the Option", "B" },
        { "AtmVol", "ATM Volatility", "B" },
        { "Time", "Time to Expiry", "B" },
        { "Model", "Model Descriptor", "XLF_OPER" }
    };

    XLRegistration::XLFunctionRegistrationHelper registerOptionPricer
	(   "xlOptionPricer", "OptionPricer", "Computes a call option price",
        "FinLib", OptionPricerArgs, 5);


//BlackScholes
	XLRegistration::Arg BlackScholesArgs[] = {
        { "Forward", "Forward", "B" },
        { "Strike", "Strike of the Option", "B" },
        { "AtmVol", "ATM Volatility", "B" },
        { "Time", "Time to Expiry", "B" }
    };

    XLRegistration::XLFunctionRegistrationHelper registerBlackScholes
	(   "xlBlackScholes", "BlackScholes", "BS formula for Call option",
        "FinLib", BlackScholesArgs, 4);


//FrequencyDistribution
	XLRegistration::Arg FrequencyDistributionArgs[] = {
        { "Data", "Data", "XLF_OPER" }
	};
    XLRegistration::XLFunctionRegistrationHelper registerFrequencyDistribution
	(   "xlFrequencyDistribution", "FrequencyDistribution", "Computes a Frequency Distribution",
        "FinLib", FrequencyDistributionArgs, 1);

//md5file
	XLRegistration::Arg md5fileArgs[] = {
        { "Filename", "File name", "XLF_OPER" }
	};
    XLRegistration::XLFunctionRegistrationHelper registermd5file
	(   "xlmd5file", "md5file", "Computes  MD5 code for the file",
        "FinLib", md5fileArgs, 1);

//Cholesky
    XLRegistration::Arg CholeskyArgs[] = {
        { "Matrix", "SemiDefinitePositive Matrix", "XLF_OPER" }
    };

    XLRegistration::XLFunctionRegistrationHelper registerCholesky
	(   "xlCholesky", "Cholesky", "Computes a Cholesky decomposition",
        "FinLib", CholeskyArgs, 1);

//EigenValuesVectors
    XLRegistration::Arg EigenValuesVectorsArgs[] = {
        { "Matrix", "Matrix", "XLF_OPER" }
    };

    XLRegistration::XLFunctionRegistrationHelper registerEigenValuesVectors
	(   "xlEigenValuesVectors", "EigenValuesVectors", "Computes an eigenvalues decomposition",
        "FinLib", EigenValuesVectorsArgs, 1);


//OptionPricer
	XLRegistration::Arg MatProdArgs[] = {
        { "MatrixA", "MatrixA", "XLF_OPER" },
        { "MatrixB", "MatrixB", "XLF_OPER" }
    };


    XLRegistration::XLFunctionRegistrationHelper registerMatProd
	(   "xlMatProd", "MatProd", "Matrix product",
        "FinLib", MatProdArgs, 2);



	//xlMultiLinearRegression
	XLRegistration::Arg MultiLinearRegressionArg[] = {
        { "xGrid", "xGrid", "XLF_OPER" },
        { "b", "b", "XLF_OPER" }
    };
    XLRegistration::XLFunctionRegistrationHelper registerMultiLinearRegressionArgs
	(   "xlMultiLinearRegression", "MultiLinearRegression", "Computes the MultiLinearRegression aX=b ",
        "FinLib", MultiLinearRegressionArg, 2);



//GraphAlgoTest
	XLRegistration::Arg xlGraphAlgoTestArg[] = {
        { "TestDescriptor", "TestDescriptor", "XLF_OPER" },
        { "matrix", "matrix", "XLF_OPER" }
    };

    XLRegistration::XLFunctionRegistrationHelper registerGraphAlgoTest
	(   "xlGraphAlgoTest", "GraphAlgoTest", "Executes a Graph Algorithm.",
        "FinLib", xlGraphAlgoTestArg, 2);
}
