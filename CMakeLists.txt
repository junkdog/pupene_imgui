### 3.8.0 for smoother c++17
cmake_minimum_required(VERSION 3.8.0)

project(pupene_example VERSION 0.2.0)

### conan configuration
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    # forked; always use libstdc++11 for clang
    # ref https://github.com/conan-io/cmake-conan/issues/47
    set(conan_download_url "junkdog/cmake-conan")
    message(STATUS "Downloading conan.cmake from https://github.com/${conan_download_url}")
    file(DOWNLOAD "https://raw.githubusercontent.com/${conan_download_url}/master/conan.cmake"
        "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_cmake_run(
    REQUIRES
        SDL2/2.0.4@lasote/stable
        glew/2.0.0@coding3d/stable
        glm/0.9.8.5@g-truc/stable
        pupene/0.2.0@junkdog/stable
        imgui/1.52@junkdog/stable
    OPTIONS
        SDL2:shared=False
        pupene:json=True
    BASIC_SETUP CMAKE_TARGETS
    BUILD missing)

### compiler settings
set(CMAKE_CXX_STANDARD 17)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL Clang)
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
        -fno-omit-frame-pointer)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL gcc)
    add_compile_options(
        -Wall
        -fno-omit-frame-pointer)
endif()
### build it
add_subdirectory(src)
