// http://stackoverflow.com/questions/14827160/how-do-i-enable-syntax-highlighting-of-cuda-cu-files-in-visual-studio-2010
 
#define WIN32
#include <stdio.h>
#include <stdlib.h>

//#include <windows.h>
//#include <iostream>
 
#include <cuda.h>
#include <curand_kernel.h>
#include <curand_mtgp32_host.h>
#include <curand_mtgp32dc_p_11213.h>
 
#include <cuda_runtime.h>
#include <helper_cuda.h>
#include <helper_cuda.h>

#include <dSFMT\dSFMT.h>
#include "hftimer.h"
 
//cudaSuccess
//CURAND_STATUS_SUCCESS
class Curand
{
public:
        Curand(): initialised(false) {}
        ~ Curand() {if (initialised) destroy();}
        void init(int seed, curandRngType_t type=CURAND_RNG_PSEUDO_MTGP32, int _size=16384*4);
        float rand();
        // CURAND_RNG_PSEUDO_MRG32K3A - in double precision only
        // CURAND_RNG_PSEUDO_DEFAULT,
        // CURAND_RNG_PSEUDO_XORWOW, CURAND_RNG_PSEUDO_MTGP32
        // CURAND_RNG_QUASI_SOBOL32
protected:
        bool initialised;
        void destroy();
        curandGenerator_t gen;
        float* devPtr, *hostPtr;
        int size, pos;
};
 
char* curandGetErrorString(curandStatus_t status)
{
        switch ( (int)status)
        {
        case CURAND_STATUS_SUCCESS: //0
                return "No errors";
        case CURAND_STATUS_VERSION_MISMATCH : //100
                return "Header file and linked library version do not match";
        case CURAND_STATUS_NOT_INITIALIZED: //101
                return "Generator not initialized";
        case CURAND_STATUS_ALLOCATION_FAILED: //102
                return "Memory allocation failed";
        case CURAND_STATUS_TYPE_ERROR: //103
                return "Generator is wrong type";
        case CURAND_STATUS_OUT_OF_RANGE: //104
                return "Argument out of range";
        case CURAND_STATUS_LENGTH_NOT_MULTIPLE: //105
                return "Length requested is not a multple of dimension";
        case CURAND_STATUS_DOUBLE_PRECISION_REQUIRED: //106
                return "GPU does not have double precision required by MRG32k3a";
        case CURAND_STATUS_LAUNCH_FAILURE: //201
                return "Kernel launch failure";
        case CURAND_STATUS_PREEXISTING_FAILURE : //202
                return "Preexisting failure on library entry";
        case CURAND_STATUS_INITIALIZATION_FAILED: // 203
                return "Initialization of CUDA failed";
        case CURAND_STATUS_ARCH_MISMATCH: //204
                return "Architecture mismatch, GPU does not support requested feature";
        default:
        case CURAND_STATUS_INTERNAL_ERROR : // 999
                return "Internal library error";
        } //switch
} // char* curandGetErrorString(curandStatus_t status)
 
void Curand::init(int seed, curandRngType_t type, int _size)
{
        curandStatus_t status;
        status = curandCreateGenerator(&gen, type);
        if (status!=CURAND_STATUS_SUCCESS)
        {
                printf("%s - Error in %s at line %d \n", curandGetErrorString(status), __FILE__, __LINE__ );
                exit(-1);
        }
        status = curandSetPseudoRandomGeneratorSeed(gen, seed);
       
        size = _size;
        cudaMalloc((void **)&devPtr, size*sizeof(float));
        hostPtr=new float[size];
        pos=size; // no numbers are available
        initialised=true;
} // Curand::init(int seed, curandRngType_t type, int _size)
 
float Curand::rand()
{
        if (pos<size)
                return hostPtr[pos++];
        curandGenerateUniform(gen,devPtr, size);
        cudaMemcpy(hostPtr, devPtr, size*sizeof(float), cudaMemcpyDeviceToHost);
        pos=1;
        return hostPtr[0];
} // float curand::rand()
 
void Curand::destroy()
{
        if (!initialised)
                return;
 
        curandDestroyGenerator(gen);
        delete[] hostPtr;
        cudaFree(devPtr);
} // curand::destroy()
 
 
__global__ void init_stuff(curandState *state)
{
        int idx = blockIdx.x * blockDim.x + threadIdx.x;
        curand_init(123, idx, 0, &state[idx]);
}
 
__global__ void make_rand(curandState *state, float *randArray, int nRuns)
{
        int idx = blockIdx.x * blockDim.x + threadIdx.x;
        float a, b;
        float count = 0;
        int i;
//      count = threadIdx.x;
 
        for (i=0; i<nRuns; i++)
        {
                a = curand_uniform(&state[idx]);
                b = curand_uniform(&state[idx]);
                if (a*a + b*b <=1) count++;
        }
 
        randArray[idx] = count;
}
 
 
// gets info about the <deviceNR>th CUDA device
void cudaDeviceInfo(int deviceNR, cudaDeviceProp& deviceProp)
{
        int deviceCount = 0;
        int driverVersion = 0, runtimeVersion = 0;
 
        cudaError_t error_id = cudaGetDeviceCount(&deviceCount);
 
    if (error_id != cudaSuccess)
    {
        printf("cudaGetDeviceCount returned %d\n-> %s\n", (int)error_id, cudaGetErrorString(error_id));
        exit(EXIT_FAILURE);
    }
        if (deviceNR>deviceCount)
        {
        printf("cudaGetDeviceCount returned %d\n-> %s\n", (int)error_id, cudaGetErrorString(error_id));
        exit(EXIT_FAILURE);    
        }
 
        cudaSetDevice(deviceNR);
        cudaGetDeviceProperties(&deviceProp, deviceNR);
        printf("\nDevice %d: \"%s\"\n", deviceNR, deviceProp.name);
        cudaDriverGetVersion(&driverVersion);
        cudaRuntimeGetVersion(&runtimeVersion);
 
        printf("  CUDA Driver Version / Runtime Version          %d.%d / %d.%d\n", driverVersion/1000, (driverVersion%100)/10, runtimeVersion/1000, (runtimeVersion%100)/10);
    printf("  CUDA Capability Major/Minor version number:    %d.%d\n", deviceProp.major, deviceProp.minor);
    char msg[256];
    sprintf(msg, "  Total amount of global memory:                 %.0f MBytes (%llu bytes)\n",
            (float)deviceProp.totalGlobalMem/1048576.0f, (unsigned long long) deviceProp.totalGlobalMem);
    printf("%s", msg);
 
    printf("  (%2d) Multiprocessors x (%3d) CUDA Cores/MP:    %d CUDA Cores\n",
           deviceProp.multiProcessorCount,
           _ConvertSMVer2Cores(deviceProp.major, deviceProp.minor),
           _ConvertSMVer2Cores(deviceProp.major, deviceProp.minor) * deviceProp.multiProcessorCount);
 
    printf("  GPU Clock rate:                                %.0f MHz (%0.2f GHz)\n", deviceProp.clockRate * 1e-3f, deviceProp.clockRate * 1e-6f);
 
    printf("  Total amount of constant memory:               %lu bytes\n", deviceProp.totalConstMem);
    printf("  Total amount of shared memory per block:       %lu bytes\n", deviceProp.sharedMemPerBlock);
    printf("  Total number of registers available per block: %d\n", deviceProp.regsPerBlock);
    printf("  Warp size:                                     %d\n", deviceProp.warpSize);
    printf("  Maximum number of threads per multiprocessor:  %d\n", deviceProp.maxThreadsPerMultiProcessor);
    printf("  Maximum number of threads per block:           %d\n", deviceProp.maxThreadsPerBlock);
    printf("  Maximum sizes of each dimension of a block:    %d x %d x %d\n",
           deviceProp.maxThreadsDim[0],
           deviceProp.maxThreadsDim[1],
           deviceProp.maxThreadsDim[2]);
    printf("  Maximum sizes of each dimension of a grid:     %d x %d x %d\n",
           deviceProp.maxGridSize[0],
           deviceProp.maxGridSize[1],
           deviceProp.maxGridSize[2]);
    printf("  Maximum memory pitch:                          %lu bytes\n", deviceProp.memPitch);
} // cudaDeviceInfo(int deviceNR)
 
const int nThreads=512;
const int nBlocks= 1;
const double N = nBlocks * nThreads * 200000;
 
double computePiOnHost()
{
        dsfmt_t dsfmt;
        double x,y;
        double count=0;
        int i;
       
        dsfmt_init_gen_rand(&dsfmt, 123);
 
        for (i=0; i<N; i++)
        {
                x = dsfmt_genrand_close_open(&dsfmt);
                y = dsfmt_genrand_close_open(&dsfmt);
                if (x*x +y*y<=1) count++;
        }
        return 4*count/N;
}
 
double computePiOnHost2()
{
        double x,y;
        double count=0;
        int i;
       
        Curand c;
        c.init(123);
 
        for (i=0; i<N; i++)
        {
                x = c.rand();
                y = c.rand();
                if (x*x +y*y<=1) count++;
        }
        return 4*count/N;
}
 
double computePiOnGPU()
{
        curandState *d_state;
        cudaMalloc(&d_state, nThreads * nBlocks * sizeof(curandState) );
 
        float* randArray;
    cudaMalloc((void**) &randArray, sizeof(float)* nBlocks*nThreads);
 
    // setup seeds
        init_stuff<<<nBlocks, nThreads>>>(d_state);
 
        int nRuns = (int)N/(nBlocks * nThreads);
        double count = 0;
    float host[nThreads * nBlocks];
 
        int j;
 
        make_rand<<<nBlocks, nThreads>>>(d_state, randArray, nRuns);
    cudaMemcpy(host, randArray, sizeof(float)*nThreads * nBlocks, cudaMemcpyDeviceToHost);
        for (j=0; j<nThreads * nBlocks; j++) count += host[j];
 
        cudaFree(d_state);
        cudaFree(randArray);
 
        return 4*count/N;
} // double computePiOnGPU()
 
 
__global__ void computePiOnGPU2_kernel(curandStateMtgp32 * devState, float sampleCount, float* devArray)
{
        int id = threadIdx.x + blockIdx.x*256;
        float count=0;
        int i;
        float x,y;
        //curandStateMtgp32 localState = devState[blockIdx.x];
        for (i=0; i<sampleCount; ++i)
        {
                x = curand_uniform(&devState[blockIdx.x]);
                y = curand_uniform(&devState[blockIdx.x]);
                //x = curand_uniform(&localState);
                //y = curand_uniform(&localState);
                if (x*x+y*y<=1) count++;
        }
        //devState[blockIdx.x] = localState;
        devArray[id] += count/sampleCount;
} // computePiOnGPU2_kernel
 
 
double computePiOnGPU2()
{       // using MTGP32
       
    int i;
    double total=0;
    curandStateMtgp32 * devState;
    mtgp32_kernel_params * devKernelParams;
    float* devArray, hostArray[64*256];
    float sampleCount = 100;
    const int nbRuns = 100;

    cudaMalloc((void**)&devState, 64 * sizeof(curandStateMtgp32) );
    cudaMalloc((void**)&devKernelParams, sizeof(mtgp32_kernel_params) );
    cudaMalloc((void**) &devArray, sizeof(float)*64*256);
 
    cudaMemset(devArray, 0, 64*256*sizeof(float));

    int status = (int ) curandMakeMTGP32Constants(mtgp32dc_params_fast_11213, devKernelParams);
    status = (int) curandMakeMTGP32KernelState(devState, mtgp32dc_params_fast_11213, devKernelParams, 64, 1234);

    for (i=0; i<nbRuns; i++)
    {
            computePiOnGPU2_kernel<<<64, 256>>>(devState, sampleCount, devArray);
    }
 
    cudaMemcpy(hostArray, devArray, sizeof(float)*64*256, cudaMemcpyDeviceToHost);

    cudaFree(devState);
    cudaFree(devArray);

    for (i=0; i<64*256; ++i)
    {
            total += hostArray[i];
    }
    total=(total*4)/(256*64*nbRuns);

    return total;
} // double computePiOnGPU2()
 
 
int main()
{
	hftimer timer; 
	cudaDeviceProp deviceProp;

    // first device starts at 0.
    // Get CUDA device capabilities
    cudaDeviceInfo(0, deviceProp);
   
     printf("Reference value of PI is 3.1415 9265 3589 793 23846 26433 \n");
    long t;
 
	timer.startTimer();
 //   printf("Host PI=%f\n",computePiOnHost2() );
	t = timer.getTime();
	timer.startTimer();
//    printf("Host PI=%f\n",computePiOnHost() );
	printf("GPU PI=%f\n",computePiOnGPU2() );
	t = timer.getTime();
	 
	return 0;
} // int main()