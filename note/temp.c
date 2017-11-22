int c;              // For CPU
int *dev_c;         // Allocated at CPU memory, pointed to GPU global memory
cudaMalloc((void**)&dev_c, sizeof(int)); 	// Allocate on GPU memory
add<<<1, 1>>>( 2, 7, dev_c );				// Kernel execution on GPU
cudaMemcpy(&c, dev_c);  					// Retrieve dev_c from GPU memory