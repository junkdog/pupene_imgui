#include <pupene/pup.h>

struct Color {
    float r, g, b, a = 1.f;
};

struct FontReference {
    std::string path;
    Color inner;
    Color outer;
};

namespace pupene::fns {
    template<typename P>
    void pup(Pupper<P>& p,            // the serializer in use
             Color& v,                // always as non-const references
             const Meta& meta) {      // metadata for color

        // idiomatic boilerplate
        pup_object(p, v, meta, [&v](auto&& fpup) {
            fpup(v.r, "r");           // enumerate each field
            fpup(v.g, "g");
            fpup(v.b, "b");
            fpup(v.a, "a");
        });
    }

    template<typename P>
    void pup(Pupper<P>& p,
             FontReference& v,
             const Meta& meta) {

        pup_object(p, v, meta, [&v](auto&& fpup) {
            fpup(v.path,  "path");
            fpup(v.inner, "inner");   // dispatches to pup for Color
            fpup(v.outer, "outer");
        });
    }
}