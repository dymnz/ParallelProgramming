延續 pre test 的內容，這次我們要做的是 2-opt local search algorithm with parallelization on TSP。

這次實作的內容跟上次的 pre test 獨立，所以請不用擔心上次做得好不好。

說明如附檔，但有幾項要特別注意一下：
1. 因為時執行間限制 10 分鐘，所以大家可能要注意 File I/O 的處理，以免最後檔案來不及寫完。
2. 請大家一定要用2-opt來完成，因為後面的距離評比，主要是比較大家在一定時間內做出最多次 2opt 判斷。
3. 這次要請各位寫一下結報，主要是講一下怎麼實作、遇到甚麼困難、以及結果。


#### Serial version:

```
num_city: number of cities to connect

2opt_swap(start, end);

for (i = 1; i < num_city - 1; ++i) {
	for (j = i + 1; j < num_city; ++j) {
		2opt_swap(i, j);
	}
}
```

#### Can be changed to:

```
start from (1) to (num_city - 1)
	2opt_swap with *depth* (2) to (num_city - i)

for (i = 1; i < num_city - 1; ++i) {
	for (depth = 1; depth < num_city - i; ++depth) {
		2opt_swap(i, i + depth);
	}
}
```
*depth* is the length between the 2opt_swap indices


#### Idea: Split *depth* across threads

```
num_thread: number of thread available
num_city: number of cities to connect

maximum_depth = num_city - 1
depth_for_each_thread = maximum_depth / num_thread

2opt_swap(start, end):
	_read_lock
		create new_route
	_unlock

	assert(distance(new_route) < distance(current_route))
	_write_lock
		current_route = new_route
	_unlcok
end
```


A race condition may occur when a thread pass through assertion,
but not yet change the `current_route`; a better `current_route` may 
be overwritten, so the assertion have to be done again after applying
`_write_lock`


The aforementioned race condition also prevents partial update of 
the `current_route`, so a new array(new_route) is created every operation.


`distance(current_route)` can be cached.


There are also two ways to split the *depth*
```
Example 9 depth, 3 threads

Chunck: 
Thread 1: 1/2/3 depth
Thread 2: 4/5/6 depth
Thread 3: 7/8/9 depth

Balanced:
Thread 1: 1/4/7 depth
Thread 2: 2/5/8 depth
Thread 3: 3/6/9 depth
```

