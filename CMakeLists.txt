### 3.8.0 for smoother c++17
cmake_minimum_required(VERSION 3.8.0)

project(pupene_example VERSION 0.2.0)

### conan configuration
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    set(conan_download_url "junkdog/cmake-conan")
    message(STATUS "Downloading conan.cmake from https://github.com/${conan_download_url}")
    file(DOWNLOAD "https://raw.githubusercontent.com/${conan_download_url}/master/conan.cmake"
        "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_cmake_run(
    CONANFILE conanfile.txt
    BASIC_SETUP CMAKE_TARGETS
    BUILD missing)

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
