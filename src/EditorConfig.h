#pragma once

#include <string>

struct EditorConfig {
    struct Filter {
        std::string pattern;
        bool show_parents;
        bool request_focus; // FIXME: render as checkbox
    };

    std::string title;
    Filter filter;
};

