## 

A small demo exploring automatically building editor widgets for
structs. Objects are introspected using [pupene][pupene], while
[imgui][imgui] provides the underlying UI.

 [pupene]: https://github.com/ocornut/imgui
 [imgui]:  https://github.com/ocornut/imgui

Code:

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

UI:

![pupene_imgui demo](pupene_imgui.gif?raw=true "UI")



### A quick breakdown

`pup()` functions work with all puppers. As such, for any type
for which a UI can be constructed, binary and json serialization 
is available too.  

### Project setup

#### Configure

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