#include<cuda.h>
#include<stdio.h>

int main(void) {
	void HostMMM(float *, float *, float *, int);
	void CudaMMM(float *, float *, float *, int);
	const int Width = 5;
	float M[Width*Width], N[Width*Width], P[Width*Width];
	for(int i = 0; i < (Width*Width) ; i++) {
		M[i] = 5;
		N[i] = 5;
		P[i] = 0;
	}

	//HostMMM(M, N, P, Width);
	CudaMMM(M, N, P, Width);

  printf("\n");
	for(int i = 0; i < (Width*Width) ; i++) {
		printf("%f ", P[i]);
		if (i%Width==(Width-1)) printf("\n");
	}
  printf("\n");

	return 0;
}

//HostMMM
void HostMMM(float* A, float* B,  float* C, int n) {
	for (int i = 0; i < n; ++i)
		for (int j = 0; j < n; ++j) {
			double sum = 0;
			for (int k = 0; k < n; ++k) {
				double a = A[i * n + k];
				double b = B[k * n + j];
				sum += a * b;
			}
			C[i * n + j] = sum;
		}
}


//MMMKernel
__global__ void MatrixMulKernel(float *Md, float *Nd, float *Pd, int Width) {
	//2D Thread ID
	int tx = threadIdx.x;
	int ty = threadIdx.y;

	//Pvalue stores the Pd element that is computed by the thread
	float Pvalue = 0;

	for(int k = 0; k < Width ; ++k) {
		float Mdelement = Md[ty*Width + k];
		float Ndelement = Nd[k*Width + tx];
		Pvalue += (Mdelement*Ndelement);
	}

	Pd[ty*Width + tx] = Pvalue;
}

void CudaMMM(float *M, float *N, float *P, int Width) {
	int size = Width*Width*sizeof(float);
	float *Md, *Nd, *Pd;

	//Transfer M and N to device memory
	cudaMalloc((void**)&Md, size);
	cudaMemcpy(Md,M,size,cudaMemcpyHostToDevice);
	cudaMalloc((void**)&Nd, size);
	cudaMemcpy(Nd,N,size,cudaMemcpyHostToDevice);

	//Allocate P on the device
	cudaMalloc((void**)&Pd,size);

	//Setup the execution configuration
	dim3 dimBlock(Width,Width);
	dim3 dimGrid(1,1);

	//Launch the device computation threads!
	MatrixMulKernel<<<dimGrid,dimBlock>>>(Md,Nd,Pd,Width);

	//Transfer P from device to host
	cudaMemcpy(P,Pd,size,cudaMemcpyDeviceToHost);

	//Free device matrices
	cudaFree(Md);
	cudaFree(Nd);
	cudaFree(Pd);
}
