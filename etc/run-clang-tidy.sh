#!/usr/bin/env bash
set -o errexit

if ! which clang-tidy > /dev/null; then
    sudo apt-get install -y clang-tidy
fi
clang-tidy -version

#
# Each check has a name and the checks to run can be chosen using the -checks= option, which specifies a comma-separated
# list of positive and negative (prefixed with -) globs. For example:
#
#    $ clang-tidy test.cpp -checks=-*,clang-analyzer-*,-clang-analyzer-cplusplus*
#
# will disable all default checks (-*) and enable all clang-analyzer-* checks except for clang-analyzer-cplusplus* ones.
#
# The -list-checks option lists all the enabled checks. When used without -checks=, it shows checks enabled by default.
# Use -checks=* to see all available checks or with any other value of -checks= to see which checks are enabled by this
# value.
#
# see https://clang.llvm.org/extra/clang-tidy
#
CMD="clang-tidy -p=build"

echo "Running clang-tidy with configuration:"
eval $CMD -dump-config

# all source and header files, excluding third party libraries
FIND="find src -type f \( -name \*.hh -o -name \*.hpp -o -name \*.cpp \) -not -path \"*third_party*\""
eval $FIND | xargs $CMD
