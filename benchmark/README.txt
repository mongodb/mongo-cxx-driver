This suite implements the benchmarks described in this spec: https://docs.google.com/document/d/1x7dCHHx_owOFYIPZ6VQwPerSWRhdkMSiepyFrNSFlLw/edit#

In order to run the microbenchmarks, first run etc/microbenchmark-test-data.sh to download the data.

In order to run specific tests, just specify their names as arguments. If run with no arguments,
all benchmarks will be run.
e.g. build/benchmark/microbenchmarks BSONBench MultiBench

Full list of options:
BSONBench
SingleBench
ParallelBench
ReadBench
WriteBench
RunCommandBench

Note: make sure you run both the download script and the microbenchmarks binary from the project root.

See the spec for details on these benchmarks.

In case there is trouble running the benchmarks, they were written and will run with: 
 -libbson at commit 1c5b90022bbbdf0fa8095b337a25b218a2651241 
 -mongoc driver at commit a3c8a760ce90f144fd65d8a4a6e3606cbeea2f6b

Note that in order to compare against the other drivers, an inMemory mongod instance should be 
used.

At this point, bson_decoding has not been added to the benchmarking suite due to the fact that
extended_bson has not been added to the C++ driver (CXX-1241).

Also note that the BSONBench tests are implemented to mirror the C driver's interpretation of the spec.