# Assignment 2 Readme

I ran into trouble trying to compile and build the project with the
instructions from the `README.md` file (Windows WSL Ubuntu).

After running `cmake ..`, I got this error for ZLIB not being
found

```
-- Could NOT find LibEdit (missing: LibEdit_INCLUDE_DIRS LibEdit_LIBRARIES)
-- Could NOT find ZLIB (missing: ZLIB_LIBRARY ZLIB_INCLUDE_DIR)
-- Could NOT find zstd (missing: zstd_LIBRARY zstd_INCLUDE_DIR)
-- Could NOT find CURL (missing: CURL_LIBRARY CURL_INCLUDE_DIR)
-- Found LLVM 18.1.3
-- Using LLVMConfig.cmake in: /usr/lib/llvm-18/cmake
-- Configuring done (5.1s)
CMake Error at /usr/lib/llvm-18/lib/cmake/llvm/LLVMExports.cmake:73 (set_target_properties):
  The link interface of target "LLVMSupport" contains:

    ZLIB::ZLIB

  but the target was not found.  Possible reasons include:

    * There is a typo in the target name.
    * A find_package call is missing for an IMPORTED target.
    * An ALIAS target is missing.

Call Stack (most recent call first):
  /usr/lib/llvm-18/cmake/LLVMConfig.cmake:375 (include)
  CMakeLists.txt:7 (find_package)


-- Generating done (0.2s)
CMake Generate step failed.  Build files cannot be regenerated correctly.
```

To fix this and get the code to compile I ran the following commands

1. `sudo apt update` - given in the starter
2. `sudo apt install cmake` - given in the starter
3. `sudo apt install llvm-19 clang-19 llvm-19-dev` - given in the starter
4. `sudo apt install zlib1g-dev libzstd-dev libedit-dev libcurl4-openssl-dev` - a new command I ran to get the project running that installs ZLIB
5. `mkdir build` - given in the starter
6. `cd build` - given in the starter
7. `cmake .. ` - given in the starter
8. `cmake --build .` - given in the starter
