本次作業延續第一次作業內容，請把平行程式技巧代換成OpenMP
題目內容與第一次作業相同，但結報內容有些不同，
建議各位先看一下結報範例之後，再開始撰寫程式。

提醒：
1. 這次需要各位計算 2-opt 比較的次數
2. 計算時間時請用 "time"，(用 clock， 會計算 CPU clock，多個thread 執行時，他可能會回傳每個 thread 使用的 CPU clock 做加總。)
http://www.cplusplus.com/reference/ctime/time/


#### Required Operation

```
2opt_swap(start, end):
    _read_lock
        find delta distance at start and end (distance(original))
        find delta distance at start and end after swap (distance(swapped))
    _unlock

    assert(distance(swapped) < distance(original))
    
    _read_lock
        copy the route with partial reversed route
        find the total distance of new_route
    _unlock
                        <------- race condtion here
    _write_lock
        current_route = new_route
    _unlcok
end
```


#### 2-opt

##### Concept
Check if the route can be shorten by reversing a segment of route; if so, reverse the route

##### Make it faster
* A reversed segment has the same route length, the total route distance is changed at the start and the end of the segment
* If every segment on a route is only processed by one thread, threads can work on the route simultaneously

#### Route Splitting
To process 2opt operation on every route:
```
for (depth = 1; depth < num_city - 1; ++depth) {
    for (start = 1; start < num_city - depth; ++start) {
        2opt_swap(start, start + depth);
    }
```

Idea: Split `start` threads, and avoid "contention". The splitting is fair because each thread is processing segments with the same length.

#### Contention
Contention is where two or more thread works on neighboring/crossed segment, and race condition may happen.

To avoid this, `start` is splitted in chunks, so contention will only happen between two consecutive threads.

With contention only happening between two consecutive threads, it's easy to check if contention happens, and the thread with the smaller thread_num can simply wait until the next thread moves on:
```
In thread(i): 
while (segment_start_index[i + 1] <= segment_end_index[i] + 1)
    wait;    
```


