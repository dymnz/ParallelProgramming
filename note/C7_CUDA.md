#### CUDA
* Platform defined by NVIDIA, hardware made by NVIDIA
* CUDA C, C support for CUDA


#### Host and device
* Host: CPU
* Device: GPU and GPU memory
    * GPU RAM split into two part
        * Local memory for each CU
        * Global memory
* Kernel: Function executing on the device

#### NVCC, Nvidia compiler
* Host code
    * For CPU compiled by standard C compiler
* NV Assembly (PTX)
    * For GPU driver


#### Code
Host function call:
```
#include <iostream> 
int main (void){
    printf(“Hello World!\n”); 
    return 0;
}
```

Kernel function call:
```
#include <iostream> 
__global__ void kernel (void){ } 
int main (void){
    kernel<<<1,1>>>(); 
    printf(“Hello World!\n”); 
    return 0;
}
```
* Qualifier `__global__` to run on GPU
* Embelishment `<<<1,1>>>` argument to pass at runtime, not compiled in device code

Argument passing:
```
int c;              // For CPU
int *dev_c;         // Allocated at CPU mem, pointed to GPU global mem
cudaMalloc((void**)&dev_c, sizeof(int));    // Allocate on GPU mem
add<<<1, 1>>>( 2, 7, dev_c );               // Kernel execution on GPU
cudaMemcpy(&c, dev_c);                      // Retrieve dev_c from GPU memo
cudaFree(dev_c);                            // Freeing memory allocated on GPU mem
```
* Copying from and to GPU memory creates many overhead
#### Thrust
* STL for CUDA C