### 3.8.0 for smoother c++17
cmake_minimum_required(VERSION 3.8.0)

### hunter init
include("cmake/HunterGate.cmake")
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.19.99.tar.gz"
    SHA1 "55ca271e9276b9973176e3f5a8c0d0dc25e46a53"
    LOCAL # use cmake/hunter/config.cmake
)

project(pupene_example VERSION 0.1.1)

### compiler settings
set(CMAKE_CXX_STANDARD 17)
add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Wabstract-final-class
    -Wabstract-vbase-init
    -Wauto-import
    -Wdouble-promotion
    -Wfloat-conversion
    -Wfloat-equal
    -Wheader-hygiene
    -Widiomatic-parentheses
    -Wimplicit-fallthrough
    -Winconsistent-missing-override
    -fno-omit-frame-pointer
)

### build it
add_subdirectory(src)
