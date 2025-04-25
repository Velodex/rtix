# C++ CMake Import Example

This example tests the ability to import the package using CMake.  The CMakeLists.txt is completely independent from the project CMakeLists.txt.  After building and installing rtix in the docker container, cd into this folder and run the following
- `mkdir build && cd build`
- `cmake ..`
- `make -j`
- `./rtix-cmake-import`

Follow this example for importing in your own project