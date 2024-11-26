# Initialize the C++ Driver

## Basic Usage

@snippet api/mongocxx/examples/instance/basic_usage.cpp Example

## With Static Lifetime

@warning This pattern depends on an exit-time destructor with indeterminate order relative to other objects with static lifetime being destroyed.

@snippet api/mongocxx/examples/instance/current.cpp Example

# Errors

## Instance Recreation

@snippet api/mongocxx/examples/instance/recreation.cpp Example

## Destroyed Instance

@snippet api/mongocxx/examples/instance/destroyed.cpp Example
