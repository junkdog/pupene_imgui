#pragma once

#include <pupene/pup.h>
#include <imgui/imgui.h>

struct vec2f {
    float x, y;
};

struct vec2i {
    int x, y;
};

struct vec22fi {
    vec2f floats;
    vec2i ints;
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
}