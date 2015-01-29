## Systemic Issues
 - how do we integrate mongoc's error handling with c++'s error handling facilities
 - how do we integrate mongoc's memory management with c++'s memory management facilities
 - header include discipline (eg. do we put package prefixes?)
 - remove type tags on closing braces?

## Build
 - find_libmongocxx cmake helper thing
 - figure out how to generate pc files (package config)
 - understand libc++ inline macros and determine if we need to use them
 - inline visibility macros -- do we want/need? how do we define them?
 - template visibility macros? -- do we want/need? how do we define them?
 - think about the relationship between include directory and package config

## Organization
 - do we need a namespace for base or should we move its contents up a level

## Docs
 - add a cmake target for this
