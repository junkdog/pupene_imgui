## pupene_imgui

A small demo exploring automatically building editor widgets for
structs. Objects are introspected using [pupene][pupene], while
[imgui][imgui] provides the underlying UI.


A struct:

```cpp
struct complex_thing {
    vec22fi v22fi;
    vec2f xy;
    float decimal;
    int32_t integer;
    uint8_t byte;
    Color color;
    std::string hallo = "hi there";
};
```

Called every frame:

```cpp
ui::widget::object_editor(object, config);
```

![pupene_imgui demo](pupene_imgui.gif?raw=true "UI")


### A quick intro

[pupene][pupene] requires that a `pup()` function is provided for
each serializable type. These functions work with all puppers. As
such, for any type for which a UI can be constructed, binary and json
serialization is available too.

Simplified usage:

```cpp
    // object to edit
    auto ct = complex_thing{};

    // the config object holds state for the current editor
    auto config = EditorConfig{"object"};
    config.filter.pattern.reserve(50); // rough edges

    while (!poll_events(key_callback)) {
        render_frame(window, [&ct, &config]() {
            
            // called each frame
            ui::widget::object_editor(ct, config);
        });
    }
``` 

[pup-fns.h][pups] contains all necessary `pup()` functions and types.


### Project setup

This has only been tested on Kubuntu 17.10, using clang-5.0 and
gcc 7.2. It should/might work elsewhere too.

**Requirements:**
- Recent compiler with C++17 support
- CMake 3.8+
- Python and [conan](https://conan.io/) (`pip install conan`)

Add [this conan remote][conan-jd] for imgui and pupene dependencies:  

```bash
conan remote add junkdog https://api.bintray.com/conan/junkdog/conan
```


#### Configure

This step also builds the dependencies and installs them into the
local conan cache.

```bash
cmake -H. -B_builds \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=_install
```

#### Build

```bash
 cmake --build _builds --target install -- -j 8
```


#### Run

Unless running the executable from a folder containing`SourceCodePro-Semibold.ttf`,
the default imgui font will be used. 

```bash
cd _install/bin
./example
```

 [conan-jd]: https://bintray.com/junkdog/conan
 [pupene]:   https://github.com/junkdog/pupene
 [imgui]:    https://github.com/ocornut/imgui
 [pups]:     https://github.com/junkdog/pupene_imgui/blob/master/src/pup-fns.h
