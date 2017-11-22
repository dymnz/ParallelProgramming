/*
 * Copyright 1993-2010 NVIDIA Corporation.  All rights reserved.
 *
 * NVIDIA Corporation and its licensors retain all intellectual property and
 * proprietary rights in and to this software and related documentation.
 * Any use, reproduction, disclosure, or distribution of this software
 * and related documentation without an express license agreement from
 * NVIDIA Corporation is strictly prohibited.
 *
 * Please refer to the applicable NVIDIA end user license agreement (EULA)
 * associated with this source code for terms and conditions that govern
 * your use of this NVIDIA software.
 *
 */


#include "./common/book.h"

__global__ void swap( int *a, int *b ) {
	int temp = *a;
	*a = *b;
	*b = temp;
}

int main( void ) {
	int a, b;
	int *dev_a, *dev_b;

	a = 7;
	b = 3;
	printf("a:%d b:%d\n", a, b);
	
	HANDLE_ERROR( cudaMalloc( (void**)&dev_a, sizeof(int) ) );
	HANDLE_ERROR( cudaMalloc( (void**)&dev_b, sizeof(int) ) );
	cudaMemcpy(dev_a, (void **) &a, sizeof(int), cudaMemcpyHostToDevice);
	cudaMemcpy(dev_b, (void **) &b, sizeof(int), cudaMemcpyHostToDevice);

	printf("swapping\n");
	swap<<<1,1>>>(dev_a, dev_b);

	HANDLE_ERROR( cudaMemcpy( &a, dev_a, sizeof(int),
	                          cudaMemcpyDeviceToHost ) );
	HANDLE_ERROR( cudaMemcpy( &b, dev_b, sizeof(int),
							  cudaMemcpyDeviceToHost ) );

	printf("a:%d b:%d\n", a, b);
	HANDLE_ERROR( cudaFree(dev_a) );
	HANDLE_ERROR( cudaFree(dev_b) );


	return 0;
}


