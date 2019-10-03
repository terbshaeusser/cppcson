# cppcson Example

This folder contains an example application illustrating how the cppcson
library can be used.

The application reads in a passed \*.cson/\*.json file and prints the content in
formatted cson to the console.

Input files for testing can be found in the *Data* folder.

## How to build?

On Unix systems:

```
mkdir build
cd build

cmake ..
make
```

On Windows:

In the Visual Studio command line:

```
mkdir build
cd build

cmake -G "NMake Makefiles" ..
nmake
```
