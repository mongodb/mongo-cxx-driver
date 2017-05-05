# MongoDB CXX Driver examples

## Prerequisites

* `libmongoc` (see the [installation guide](http://mongoc.org/libmongoc/current/installing.html))

## Building code examples

1. Run `git clone https://github.com/mongodb/mongo-cxx-driver && cd mongo-cxx-driver/build`
2. Follow the instructions [here](https://mongodb.github.io/mongo-cxx-driver/mongocxx-v3/installation/) to install the driver
3. Run `make examples`
4. cd `examples/mongocxx`
5. Start up `mongod` running on the default port (i.e. 27017)
6. Run an example file (e.g. `./aggregate`)

## Building project examples

1. Install `libmongoc` and `mongocxx` following the [installation
   instructions](https://mongodb.github.io/mongo-cxx-driver/mongocxx-v3/installation/)
2. Change to one of the project example directories, e.g. `examples/projects/cmake/mongocxx/shared`
3. Run `./build.sh`
