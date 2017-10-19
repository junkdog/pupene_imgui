#pragma once

#include <glm/vec4.hpp>

#include <cstddef>
#include <pupene/pup.h>
#include <imgui/imgui.h>

using byte = unsigned char;

struct vec2f {
    float x, y;
};

struct vec2i {
    int x, y;
};

struct Color {
    float r, g, b, a;
};

struct vec22fi {
    vec2f floats;
    vec2i ints;
};






struct complex_thing {
    vec22fi v22fi;
    vec2f xy;
    float decimal;
    int32_t integer;
    Color color;
    std::string hallo = "omg";
};









namespace pupene::fns {
    template<typename P>
    void pup(Pupper<P>& p, vec2f& v, const Meta& meta) {
        pup_object(p, v, meta.name, [&v](auto&& fpup) {
            fpup(v.x, {"x"s});
            fpup(v.y, {"y"s});
        });
    }

    template<typename P>
    void pup(Pupper<P>& p, vec2i& v, const Meta& meta) {
        pup_object(p, v, meta.name, [&v](auto&& fpup) {
            fpup(v.x, {"x"s});
            fpup(v.y, {"y"s});
        });
    }

    template<typename P>
    void pup(Pupper<P>& p, vec22fi& v, const Meta& meta) {
        pup_object(p, v, meta.name, [&v](auto&& fpup) {
            fpup(v.floats, {"floats"s});
            fpup(v.ints, {"ints"s});
        });
    }

    template<typename P>
    void pup(Pupper<P>& p, ImVec2 v, const Meta& meta) {
        pup_object(p, v, meta.name, [&v](auto&& fpup) {
            fpup(v.x, {"x"s});
            fpup(v.y, {"y"s});
        });
    }

    template<typename P>
    void pup(Pupper<P>& p, Color& v, const Meta& meta) {
        pup_object(p, v, meta.name, [&v](auto&& fpup) {
            fpup(v.r, {"r"s});
            fpup(v.g, {"g"s});
            fpup(v.b, {"b"s});
            fpup(v.a, {"a"s});
        });
    }

    template<typename P>
    void pup(Pupper<P>& p, complex_thing& v, const Meta& meta) {
        pup_object(p, v, meta.name, [&v](auto&& fpup) {
            fpup(v.v22fi, {"v22fi"s});
            fpup(v.xy, {"xy"s});
            fpup(v.decimal, {"decimal"s});
            fpup(v.integer, {"integer"s});
            fpup(v.color, {"color"s});
            fpup(v.hallo, {"hallo"s});
        });
    }
}