## Stack Stroll

A library for demonstrating how to walk the stack of a live process.

### Overview

TODO

### Building

#### Prerequisites

TODO

#### CMake

```console
$ cmake -B build -S . -DLLVM_DIR="$(llvm-config --cmakedir)"  -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=On
```

Then

```console
$ cmake --build build 
```
