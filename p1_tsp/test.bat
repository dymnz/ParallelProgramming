for /l %%x in (0, 1, 4) do (
   start tsp.exe ./test/test%%x.i test%%x.o
   FC test%%x.o ans/gtest%%x.o
)