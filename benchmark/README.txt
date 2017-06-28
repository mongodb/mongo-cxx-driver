In order to run the microbenchmarks, download the necessary test files from:
https://github.com/mongodb-labs/driver-performance-test-data

Untar them and place the files in the directory you will be running the microbenchmarks from.

In order to select specific tests, use the flags:
 TODO: add flags

In case there is trouble running the benchmarks, they were written and will run with: 
 -libbson at commit 1c5b90022bbbdf0fa8095b337a25b218a2651241 
 -mongoc driver at commit a3c8a760ce90f144fd65d8a4a6e3606cbeea2f6b

Note that in order to compare against the other drivers, an inMemory mongod instance should be 
used.
At this point, bson_decoding has not been added to the benchmarking suite due to the fact that 
extended_bson has not been added to the C++ driver (CXX-1241).