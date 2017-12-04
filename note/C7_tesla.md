#### Nomen
* SM: Streaming Multi-processor, a SIMD processor
* SP: Streaming processor. SPs in same SM has a shared memory. SP in each block(TPC) receive the same instruction
* SPA: Streaming processor array
* TPC: Thread processing cluster
* CTA: Collaborative thread array

#### Scheduling
* All threads in one block(CTA), is assigned to a SM, using the same shared memory
* SM use 32 threads(a warp) to processa block(CTA). A SM can hold many warp context but only working on one at a time.

#### GT200 arch
* SPA
	* TPC 0...9
		* SM Controller
		* SM 0...2
			* SP 0...7
		* L1 cache
	* Global block scheduler is in charge of thread job scheduling:
		1. GBS receive thread jobs (1...1024)
		2. Assign job to SM, therefore SP (Phyiscal thread)

* TPC: a block in CUDA
	* SM x3
		* SP x8 	<--- Physical thread (8)



