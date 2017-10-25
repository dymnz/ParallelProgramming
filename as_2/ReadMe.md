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
    _unlock
    
    find delta distance at start and end after swap (distance(swapped))

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