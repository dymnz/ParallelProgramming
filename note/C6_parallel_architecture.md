#### Parallel Architecture

---
#### Interconnection network
* Shared
	* Broadcasting on shared medium
* Switched
	* Support for point to point communication
	* 1 processor - 1 switch
	* Concurrent communication
	* Scalable
	
#### Switched network topology
* Direct: Switch:Processor = 1:1
* Indirect: Switch:Processor > 1:1

# TODO, fill description and meaning
* Criteria
	* Diamemter
	* Bisection width
	* Edge per node
	* Constant edge length


---
* Processor array
	* 1 control unit w/ N processor

* Multi-processor
	* Type
		* UMA
		* NUMA

+ Multi-computer
	+ Type
		+ Multi-computer
		+ Cluster
	+ Communication
		+ IPC
		+ Message passing
		+ Bandwidth

#### Flynn's Taxonomy
	* SISD
	* SIMD
	* MISD
	* MIMD

#### Cache Coherence
* UMA, snoopy hardware 
	* Snoopy cache (Write invalidate protocol)
* NUMA, no snoopy hardware 
	* Directory-based protocol
		* Status
			* Uncached
			* Shared
			* Exclusive
		* Lazy copy

