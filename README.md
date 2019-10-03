# cppcson

The *cppcson* C++ library provides a simple api to parse
[*.cson files](https://github.com/bevry/cson).

Features:

* Meta information for parsed values (line, column etc.)
* Proper error reporting
* No dependencies

Tested on:

* Linux g++ 8.3.0
* macOS Clang 10.0.1
* Windows Visual Studio 2019

## Requirements

* CMake (version 3.10 or later)
* A C++11 compatible compiler such as g++ or clang
* Googletest (only for tests)

## How to build?

On Unix systems:

```
mkdir build
cd build

cmake ..      # -DCPPCSON_BUILD_TESTS=OFF to disable tests
make
```

On Windows:

In the Visual Studio command line:

```
mkdir build
cd build

cmake -G "NMake Makefiles" ..      # -DCPPCSON_BUILD_TESTS=OFF to disable tests
nmake
```

In case Googletest is not found download the
[source code](https://github.com/google/googletest), build it and rerun cmake:

```
cmake -G "NMake Makefiles" -DGTEST_LIBRARY=googletest-release-1.8.1\googletest\build\gtestd.lib -DGTEST_MAIN_LIBRARY=googletest-release-1.8.1\googletest\build\gtest_maind.lib -DGTEST_INCLUDE_DIR=googletest-release-1.8.1\googletest\include ..
```

Please also note possible incompatibilities between /MDd and /MTd. For more
information see
[this thread](https://stackoverflow.com/questions/12540970/how-to-make-gtest-build-mdd-instead-of-mtd-by-default-using-cmake).

If tests are enabled, they can be started by executing ```ctest``` in the same folder.

## Usage

The usage of the library is demonstrated in the *Example* and *Tests* folders.
