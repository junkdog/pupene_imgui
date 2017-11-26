#pragma once

#include <glm/vec4.hpp>

#include <cstddef>
#include <pupene/pup.h>
#include <imgui/imgui.h>
#include "EditorConfig.h"

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
    uint8_t byte;
    Color color;
    std::string hallo = "hi there";
};

namespace pupene::fns {
    template<typename P>
    void pup(Pupper<P>& p, vec2f& v, const Meta& meta) {
        pup_object(p, v, meta, [&v](auto&& fpup) {
            fpup(v.x, "x");
            fpup(v.y, "y");
        });
    }

    template<typename P>
    void pup(Pupper<P>& p, vec2i& v, const Meta& meta) {
        pup_object(p, v, meta, [&v](auto&& fpup) {
            fpup(v.x, "x");
            fpup(v.y, "y");
        });
    }

    template<typename P>
    void pup(Pupper<P>& p, vec22fi& v, const Meta& meta) {
        pup_object(p, v, meta, [&v](auto&& fpup) {
            fpup(v.floats, "floats");
            fpup(v.ints, "ints");
        });
    }

    template<typename P>
    void pup(Pupper<P>& p, ImVec2 v, const Meta& meta) {
        pup_object(p, v, meta, [&v](auto&& fpup) {
            fpup(v.x, "x");
            fpup(v.y, "y");
        });
    }

    template<typename P>
    void pup(Pupper<P>& p, Color& v, const Meta& meta) {
        pup_object(p, v, meta, [&v](auto&& fpup) {
            fpup(v.r, "r");
            fpup(v.g, "g");
            fpup(v.b, "b");
            fpup(v.a, "a");
        });
    }

    template<typename P>
    void pup(Pupper<P>& p, complex_thing& v, const Meta& meta) {
        pup_object(p, v, meta, [&v](auto&& fpup) {
            fpup(v.v22fi, "v22fi");
            fpup(v.xy, "xy");
            fpup(v.decimal, "decimal");
            fpup(v.integer, "integer");
            fpup(v.byte, "byte");
            fpup(v.color, "color");
            fpup(v.hallo, "hallo");
        });
    }

    template<typename P>
    void pup(Pupper<P>& p, EditorConfig& v, const Meta& meta) {
        pup_object(p, v, meta, [&v](auto&& fpup) {
            fpup(v.title, "title");
            fpup(v.filter, "filter");
        });
    }

    template<typename P>
    void pup(Pupper<P>& p, EditorConfig::Filter& v, const Meta& meta) {
        pup_object(p, v, meta, [&v](auto&& fpup) {
            fpup(v.pattern, "pattern");
            fpup(v.show_parents, "show_parents");
            fpup(v.request_focus, "request_focus");
        });
    }
}